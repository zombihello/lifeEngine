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
 * A batch mesh element definition
 */
struct FMeshBatchElement
{
	FIndexBufferRHIRef			indexBufferRHI;				/**< Index buffer */
	uint32						baseVertexIndex;			/**< First index vertex in vertex buffer */
	uint32						firstIndex;					/**< First index */
	uint32						numPrimitives;				/**< Number primitives to render */
	FMatrix						transformationMatrix;		/**< Transformation matrix of element */
};

/**
 * @ingroup Engine
 * A batch of mesh elements, all with the same material and vertex buffer
 */
struct FMeshBatch
{
	std::vector< FMeshBatchElement >	elements;		/**< Elements in batch */
	EPrimitiveType						primitiveType;	/**< Primitive type */
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
		 * @brief Constructor
		 */
		FDrawingPolicyLink( const TDrawingPolicyType& InDrawingPolicy )
			: drawingPolicy( InDrawingPolicy )
		{
			boundShaderState = drawingPolicy.GetBoundShaderState();
		}

		mutable FMeshBatch						meshBatch;				/**< Mesh batch */
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
			uint32		hashA = InA.drawingPolicy.GetTypeHash();
			hashA = appMemFastHash( InA.boundShaderState, hashA );
			hashA = appMemFastHash( InA.meshBatch.primitiveType, hashA );

			// Calculate hash for InB
			uint32		hashB = InB.drawingPolicy.GetTypeHash();
			hashB = appMemFastHash( InB.boundShaderState, hashB );
			hashB = appMemFastHash( InB.meshBatch.primitiveType, hashB );

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
	 * @param InMeshElements Array of mesh elements
	 */
	void AddItem( const FDrawingPolicyLink& InDrawingPolicyLink, const std::vector< FMeshBatchElement >& InMeshElements )
	{
		// Get drawing policy link in std::set
		auto	it = meshes.find( InDrawingPolicyLink );
		if ( it == meshes.end() )
		{
			it = meshes.insert( InDrawingPolicyLink ).first;
		}

		// Add elements to link
		if ( !InMeshElements.empty() )
		{
			uint32		srcNum = it->meshBatch.elements.size();
			uint32		needNum = InMeshElements.size();
			uint32		newNum = srcNum + needNum;

			it->meshBatch.elements.resize( newNum );
			for ( uint32 globalIndex = srcNum, localIndex = 0; globalIndex < newNum && localIndex < newNum; ++globalIndex, ++localIndex )
			{
				it->meshBatch.elements[ globalIndex ] = InMeshElements[ localIndex ];
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
			it->drawingPolicy.Draw( InDeviceContext, it->meshBatch, InSceneView );
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
