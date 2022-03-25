/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <set>

#include "Math/Math.h"
#include "Render/CameraTypes.h"
#include "Render/Material.h"
#include "Render/SceneRendering.h"
#include "Render/Frustum.h"
#include "Components/CameraComponent.h"
#include "Components/PrimitiveComponent.h"
#include "RHI/BaseRHI.h"
#include "RHI/BaseBufferRHI.h"
#include "RHI/TypesRHI.h"

/**
 * @ingroup Engine
 * A projection from scene space into a 2D screen region
 */
class FSceneView
{
public:
	/**
	 * Constructor
	 */
	FSceneView();

	/**
	 * Set camera view
	 * 
	 * @param[in] InCameraView View info of camera
	 */
	void SetCameraView( const FCameraView& InCameraView );

	/**
	 * Set camera view by camera component
	 * 
	 * @param[in] InCameraComponent Camera component
	 */
	FORCEINLINE void SetCameraView( class LCameraComponent* InCameraComponent )
	{
		check( InCameraComponent );

		FCameraView		cameraView;
		InCameraComponent->GetCameraView( cameraView );
		SetCameraView( cameraView );
	}

	/**
	 * Get camera view
	 * @return Return camera view info
	 */
	FORCEINLINE const FCameraView& GetCameraView() const
	{
		return cameraView;
	}

	/**
	 * Get view matrix
	 * @return Return view matrix
	 */
	FORCEINLINE const FMatrix& GetViewMatrix() const
	{
		return viewMatrix;
	}

	/**
	 * Get projection matrix
	 * @return Return projection matrix
	 */
	FORCEINLINE const FMatrix& GetProjectionMatrix() const
	{
		return projectionMatrix;
	}

	/**
	 * Get View * Projection matrix
	 * @return Return View * Projection matrix
	 */
	FORCEINLINE const FMatrix& GetViewProjectionMatrix() const
	{
		return viewProjectionMatrix;
	}

	/**
	 * Get frustum
	 * @return Return frustum
	 */
	FORCEINLINE const FFrustum& GetFrustum() const
	{
		return frustum;
	}

private:
	FMatrix			viewMatrix;				/**< View matrix */
	FMatrix			projectionMatrix;		/**< Projection matrix */
	FMatrix			viewProjectionMatrix;	/**< View * Projection matrix */
	FCameraView		cameraView;				/**< View info of camera */
	FFrustum		frustum;				/**< Frustum */
};

/**
 * @ingroup Engine
 * A batch of mesh elements, all with the same material and vertex buffer
 */
struct FMeshBatch
{
	/**
	 * @brief Functions to extract the mesh batch from FMeshBatch as a key for std::set
	 */
	struct FMeshBatchKeyFunc
	{
		/**
		 * @brief Compare FMeshBatch
		 *
		 * @param InA First mesh batch
		 * @param InB Second mesh batch
		 * @return Return true if InA and InB equal, else returning false
		 */
		FORCEINLINE bool operator()( const FMeshBatch& InA, const FMeshBatch& InB ) const
		{
			// Calculate hash for InA
			uint64		hashA = appMemFastHash( InA.indexBufferRHI );
			hashA = appMemFastHash( InA.primitiveType, hashA );
			hashA = appMemFastHash( InA.baseVertexIndex, hashA );
			hashA = appMemFastHash( InA.firstIndex, hashA );
			hashA = appMemFastHash( InA.numPrimitives, hashA );

			// Calculate hash for InB
			uint64		hashB = appMemFastHash( InB.indexBufferRHI );
			hashB = appMemFastHash( InB.primitiveType, hashB );
			hashB = appMemFastHash( InB.baseVertexIndex, hashB );
			hashB = appMemFastHash( InB.firstIndex, hashB );
			hashB = appMemFastHash( InB.numPrimitives, hashB );

			return hashA < hashB;
		}
	};

	FIndexBufferRHIRef					indexBufferRHI;				/**< Index buffer */
	EPrimitiveType						primitiveType;				/**< Primitive type */
	uint32								baseVertexIndex;			/**< First index vertex in vertex buffer */
	uint32								firstIndex;					/**< First index */
	uint32								numPrimitives;				/**< Number primitives to render */
	mutable uint32						numInstances;				/**< Number instances of mesh */
	mutable std::vector< FMatrix >		transformationMatrices;		/**< Array of transformation matrix for instances */
};

/**
 * @ingroup Engine
 * @brief Draw list of scene for mesh type
 */
template< typename TDrawingPolicyType >
class FMeshDrawList
{
public:
	/**
	 * @brief A set of draw list elements with the same drawing policy
	 */
	struct FDrawingPolicyLink
	{
		/**
		 * @brief Typedef set of mesh batches
		 */
		typedef std::set< FMeshBatch, FMeshBatch::FMeshBatchKeyFunc >		FMeshBatchList;

		/**
		 * @brief Constructor
		 */
		FDrawingPolicyLink( const TDrawingPolicyType& InDrawingPolicy )
			: drawingPolicy( InDrawingPolicy )
		{
			boundShaderState = drawingPolicy.GetBoundShaderState();
		}

		mutable FMeshBatchList					meshBatchList;			/**< Mesh batch list */
		mutable TDrawingPolicyType				drawingPolicy;			/**< Drawing policy */
		mutable FBoundShaderStateRHIRef			boundShaderState;		/**< Bound shader state */
	};

	/**
	 * @brief Functions to extract the drawing policy from FDrawingPolicyLink as a key for std::set
	 */
	struct FDrawingPolicyKeyFunc
	{
		/**
		 * @brief Compare FDrawingPolicyLink
		 * 
		 * @param InA First drawing policy
		 * @param InB Second drawing policy
		 * @return Return true if InA and InB equal, else returning false
		 */
		FORCEINLINE bool operator()( const FDrawingPolicyLink& InA, const FDrawingPolicyLink& InB ) const
		{
			// Calculate hash for InA
			uint64 hashA = appMemFastHash( InA.boundShaderState->GetHash(), InA.drawingPolicy.GetTypeHash() );

			// Calculate hash for InB
			uint64 hashB = appMemFastHash( InB.boundShaderState->GetHash(), InB.drawingPolicy.GetTypeHash() );

			return hashA < hashB;
		}
	};

	/**
	 * @brief Typedef map of draw data
	 */
	typedef std::set< FDrawingPolicyLink, FDrawingPolicyKeyFunc >		FMapDrawData;

	/**
	 * @brief Add item
	 * 
	 * @param InDrawingPolicyLink Drawing policy link
	 */
	void AddItem( const FDrawingPolicyLink& InDrawingPolicyLink )
	{
		// Get drawing policy link in std::set
		FMapDrawData::iterator	it = meshes.find( InDrawingPolicyLink );
		
		// If drawing policy link is not exist - we insert
		if ( it == meshes.end() )
		{
			it = meshes.insert( InDrawingPolicyLink ).first;
		}	
		// Else add mesh batches to link
		else
		{
			for ( auto itNewBatch = InDrawingPolicyLink.meshBatchList.begin(), itNewBatchEnd = InDrawingPolicyLink.meshBatchList.end(); itNewBatch != itNewBatchEnd; ++itNewBatch )
			{
				// If in new batch num instances less 0 - we continue to next step
				if ( itNewBatch->numInstances <= 0 )
				{
					continue;
				}

				// Find already created mesh batch, if not exist - we insert new batch
				FDrawingPolicyLink::FMeshBatchList::iterator		itOldBatch = it->meshBatchList.find( *itNewBatch );
				if ( itOldBatch == it->meshBatchList.end() )
				{
					it->meshBatchList.insert( *itNewBatch );
				}
				// Else we update numInstances and add new transformation matrix for new instances
				else
				{
					uint32		srcNumInstances = itOldBatch->numInstances;
					uint32		needNumInstances = itNewBatch->numInstances;
					uint32		newNumInstances = srcNumInstances + needNumInstances;
					itOldBatch->numInstances = newNumInstances;

					itOldBatch->transformationMatrices.resize( newNumInstances );
					for ( uint32 globalIndex = srcNumInstances, localIndex = 0; globalIndex < newNumInstances && localIndex < newNumInstances; ++globalIndex, ++localIndex )
					{
						itOldBatch->transformationMatrices[ globalIndex ] = itNewBatch->transformationMatrices[ localIndex ];
					}
				}
			}
		}
	}

	/**
	 * @brief Clear draw list
	 */
	FORCEINLINE void Clear()
	{
		meshes.clear();
	}

	/**
	 * @brief Get number items in list
	 * @return Return number items in list
	 */
	FORCEINLINE uint32 GetNum() const
	{
		return meshes.size();
	}

	/**
	 * @brief Draw list
	 * 
	 * @param[in] InDeviceContext Device context
	 * @param InSceneView Current view of scene
	 */
	FORCEINLINE void Draw( class FBaseDeviceContextRHI* InDeviceContext, const FSceneView& InSceneView )
	{
		check( IsInRenderingThread() );
		for ( FMapDrawData::const_iterator it = meshes.begin(), itEnd = meshes.end(); it != itEnd; ++it )
		{
			it->drawingPolicy.SetRenderState( InDeviceContext );
			it->drawingPolicy.SetShaderParameters( InDeviceContext );

			// Draw all mesh batches
			for ( FDrawingPolicyLink::FMeshBatchList::const_iterator itMeshBatch = it->meshBatchList.begin(), itMeshBatchEnd = it->meshBatchList.end(); itMeshBatch != itMeshBatchEnd; ++itMeshBatch )
			{
				it->drawingPolicy.Draw( InDeviceContext, *itMeshBatch, InSceneView );
			}
		}
	}

private:
	FMapDrawData		meshes;		/**< Map of meshes sorted by materials for draw */
};

/**
 * @ingroup Engine
 * @brief Enumeration of scene depth group
 */
enum ESceneDepthGroup
{
	SDG_World,		/**< World */
	SDG_Max			/**< Num depth groups */
};

/**
 * @ingroup Engine
 * @brief Scene depth group
 */
struct FSceneDepthGroup
{
	/**
	 * @brief Clear
	 */
	FORCEINLINE void Clear()
	{
		staticMeshDrawList.Clear();
		spriteDrawList.Clear();
	}

	FMeshDrawList< FStaticMeshDrawPolicy >		staticMeshDrawList;		/**< Draw list of static meshes */
	FMeshDrawList< FStaticMeshDrawPolicy >		spriteDrawList;			/**< Draw list of sprites */
};

/**
 * @ingroup Engine
 * @brief Base implementation of the scene manager
 */
class FBaseScene
{
public:
	/**
	 * @brief Add new primitive component to scene
	 *
	 * @param InPrimitive Primitive component to add
	 */
	virtual void AddPrimitive( class LPrimitiveComponent* InPrimitive ) {}

	/**
	 * @brief Remove primitive component from scene
	 *
	 * @param InPrimitive Primitive component to remove
	 */
	virtual void RemovePrimitive( class LPrimitiveComponent* InPrimitive ) {}

	/**
	 * @brief Build SDGs for render scene from current view
	 * 
	 * @param InSceneView Current view of scene
	 */
	virtual void BuildSDGs( const FSceneView& InSceneView ) {}
};

/**
 * @ingroup Engine
 * @brief Main of scene manager
 */
class FScene : public FBaseScene
{
public:
	/**
	 * @brief Add new primitive component to scene
	 *
	 * @param InPrimitive Primitive component to add
	 */
	virtual void AddPrimitive( class LPrimitiveComponent* InPrimitive ) override;

	/**
	 * @brief Remove primitive component from scene
	 *
	 * @param InPrimitive Primitive component to remove
	 */
	virtual void RemovePrimitive( class LPrimitiveComponent* InPrimitive ) override;

	/**
	 * @brief Build SDGs for render scene from current view
	 *
	 * @param InSceneView Current view of scene
	 */
	virtual void BuildSDGs( const FSceneView& InSceneView ) override;

	/**
	 * @brief Get depth group
	 * 
	 * @param InSDGType SDG type
	 * @return Return reference to depth group
	 */
	FORCEINLINE FSceneDepthGroup& GetSDG( ESceneDepthGroup InSDGType )
	{
		check( InSDGType < SDG_Max );
		return SDGs[ InSDGType ];
	}

	/**
	 * @brief Get depth group
	 *
	 * @param InSDGType SDG type
	 * @return Return reference to depth group
	 */
	FORCEINLINE const FSceneDepthGroup& GetSDG( ESceneDepthGroup InSDGType ) const
	{
		check( InSDGType < SDG_Max );
		return SDGs[ InSDGType ];
	}

private:
	FSceneDepthGroup							SDGs[ SDG_Max ];			/** Scene depth groups */
	std::list< LPrimitiveComponentRef >			primitives;					/**< List of primitives on scene */
};

#endif // !SCENE_H
