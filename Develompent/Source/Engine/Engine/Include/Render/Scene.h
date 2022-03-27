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
			return InA.GetTypeHash() < InB.GetTypeHash();
		}

		/**
		 * @brief Calculate hash of the FMeshBatch
		 *
		 * @param InMeshBatch Mesh batch
		 * @return Return hash of this FMeshBatch
		 */
		FORCEINLINE std::size_t operator()( const FMeshBatch& InMeshBatch ) const
		{
			return InMeshBatch.GetTypeHash();
		}
	};

	/**
	 * Constructor
	 */
	FORCEINLINE FMeshBatch()
		: baseVertexIndex( 0 ), firstIndex( 0 ), numPrimitives( 0 ), numInstances( 0 )
	{}

	/**
	 * Overrload operator ==
	 */
	FORCEINLINE friend bool operator==( const FMeshBatch& InA, const FMeshBatch& InB )
	{
		return	InA.indexBufferRHI == InB.indexBufferRHI		&& 
				InA.primitiveType == InB.primitiveType			&& 
				InA.baseVertexIndex == InB.baseVertexIndex		&& 
				InA.firstIndex == InB.firstIndex				&& 
				InA.numPrimitives == InB.numPrimitives;
	}

	/**
	 * Overrload operator !=
	 */
	FORCEINLINE friend bool operator!=( const FMeshBatch& InA, const FMeshBatch& InB )
	{
		return !( InA == InB );
	}

	/**
	 * Get type hash
	 * @return Return calculated hash
	 */
	FORCEINLINE uint64 GetTypeHash() const
	{
		uint64		hash = appMemFastHash( indexBufferRHI );
		hash = appMemFastHash( primitiveType, hash );
		hash = appMemFastHash( baseVertexIndex, hash );
		hash = appMemFastHash( firstIndex, hash );
		hash = appMemFastHash( numPrimitives, hash );
		return hash;
	}

	FIndexBufferRHIRef					indexBufferRHI;				/**< Index buffer */
	EPrimitiveType						primitiveType;				/**< Primitive type */
	uint32								baseVertexIndex;			/**< First index vertex in vertex buffer */
	uint32								firstIndex;					/**< First index */
	uint32								numPrimitives;				/**< Number primitives to render */
	mutable uint32						numInstances;				/**< Number instances of mesh */
	mutable std::vector< FMatrix >		transformationMatrices;		/**< Array of transformation matrix for instances */
};

/**
 * @brief Typedef set of mesh batches
 */
typedef std::unordered_set< FMeshBatch, FMeshBatch::FMeshBatchKeyFunc >		FMeshBatchList;

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
	struct FDrawingPolicyLink : public FRefCounted
	{
		/**
		 * @brief Constructor
		 */
		FDrawingPolicyLink( const TDrawingPolicyType& InDrawingPolicy )
			: drawingPolicy( InDrawingPolicy )
		{}

		/**
		 * Overrload operator ==
		 */
		FORCEINLINE friend bool operator==( const FDrawingPolicyLink& InA, const FDrawingPolicyLink& InB )
		{
			return InA.drawingPolicy.GetTypeHash() == InB.drawingPolicy.GetTypeHash();
		}

		/**
		 * Overrload operator !=
		 */
		FORCEINLINE friend bool operator!=( const FDrawingPolicyLink& InA, const FDrawingPolicyLink& InB )
		{
			return !( InA == InB );
		}

		/**
		 * Get type hash
		 * @return Return calculated hash
		 */
		FORCEINLINE uint64 GetTypeHash() const
		{
			return drawingPolicy.GetTypeHash();
		}

		mutable FMeshBatchList					meshBatchList;			/**< Mesh batch list */
		mutable TDrawingPolicyType				drawingPolicy;			/**< Drawing policy */
	};

	/**
	 * @brief Typedef of reference to drawing policy link
	 */
	typedef TRefCountPtr< FDrawingPolicyLink >		FDrawingPolicyLinkRef;

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
		FORCEINLINE bool operator()( const FDrawingPolicyLinkRef& InA, const FDrawingPolicyLinkRef& InB ) const
		{
			return InA->GetTypeHash() < InB->GetTypeHash();
		}

		/**
		 * @brief Calculate hash of the FDrawingPolicyLink
		 *
		 * @param InDrawingPolicyLink Drawing policy link
		 * @return Return hash of this FDrawingPolicyLink
		 */
		FORCEINLINE std::size_t operator()( const FDrawingPolicyLinkRef& InDrawingPolicyLink ) const
		{
			return InDrawingPolicyLink->GetTypeHash();
		}
	};

	/**
	 * @brief Functions for custom equal the drawing policy in std::set 
	 */
	struct FDrawingPolicyEqualFunc
	{
		/**
		  * @brief Compare FDrawingPolicyLinkRef
		  *
		  * @param InA First drawing policy
		  * @param InB Second drawing policy
		  * @return Return true if InA and InB equal, else returning false
		  */
		FORCEINLINE bool operator()( const FDrawingPolicyLinkRef& InA, const FDrawingPolicyLinkRef& InB ) const
		{
			return ( *InA ) == ( *InB );
		}
	};

	/**
	 * @brief Typedef map of draw data
	 */
	typedef std::unordered_set< FDrawingPolicyLinkRef, FDrawingPolicyKeyFunc, FDrawingPolicyEqualFunc >		FMapDrawData;

	/**
	 * @brief Add item
	 * 
	 * @param InDrawingPolicyLink Drawing policy link
	 * @return Return reference to drawing policy link in SDG
	 */
	FDrawingPolicyLinkRef AddItem( const FDrawingPolicyLinkRef& InDrawingPolicyLink )
	{
		check( InDrawingPolicyLink );

		// Get drawing policy link in std::set
		FMapDrawData::iterator		it = meshes.find( InDrawingPolicyLink );

		// If drawing policy link is not exist - we insert
		if ( it == meshes.end() )
		{
			it = meshes.insert( InDrawingPolicyLink ).first;
		}

		// Return drawing policy link in SDG
		return *it;
	}

	/**
	 * @brief Remove item
	 *
	 * @param InDrawingPolicyLink Drawing policy link
	 */
	void RemoveItem( const FDrawingPolicyLinkRef& InDrawingPolicyLink )
	{
		check( InDrawingPolicyLink );
		if ( InDrawingPolicyLink->GetRefCount() <= 2 )
		{
			meshes.erase( InDrawingPolicyLink );
		}
	}

	/**
	 * @brief Clear draw list
	 */
	FORCEINLINE void Clear()
	{
		for ( FMapDrawData::const_iterator it = meshes.begin(), itEnd = meshes.end(); it != itEnd; ++it )
		{
			FDrawingPolicyLinkRef		drawingPolicyLink = *it;
			for ( FMeshBatchList::const_iterator itMeshBatch = drawingPolicyLink->meshBatchList.begin(), itMeshBatchEnd = drawingPolicyLink->meshBatchList.end(); itMeshBatch != itMeshBatchEnd; ++itMeshBatch )
			{
				itMeshBatch->numInstances = 0;
				itMeshBatch->transformationMatrices.clear();
			}
		}
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
			bool						bIsInitedRenderState = false;
			FDrawingPolicyLinkRef		drawingPolicyLink = *it;

			// Draw all mesh batches
			for ( FMeshBatchList::const_iterator itMeshBatch = drawingPolicyLink->meshBatchList.begin(), itMeshBatchEnd = drawingPolicyLink->meshBatchList.end(); itMeshBatch != itMeshBatchEnd; ++itMeshBatch )
			{
				// If in mesh batch not exist instance - continue to next step
				if ( itMeshBatch->numInstances <= 0 )
				{
					continue;
				}

				// If we not initialized render state - do it!
				if ( !bIsInitedRenderState )
				{
					drawingPolicyLink->drawingPolicy.SetRenderState( InDeviceContext );
					drawingPolicyLink->drawingPolicy.SetShaderParameters( InDeviceContext );
					bIsInitedRenderState = true;
				}

				// Draw mesh batch
				drawingPolicyLink->drawingPolicy.Draw( InDeviceContext, *itMeshBatch, InSceneView );
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
	FSceneDepthGroup							SDGs[ SDG_Max ];			/**< Scene depth groups */
	std::list< LPrimitiveComponentRef >			primitives;					/**< List of primitives on scene */
};

#endif // !SCENE_H
