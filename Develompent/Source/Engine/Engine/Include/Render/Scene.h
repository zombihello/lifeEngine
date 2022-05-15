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
#include "Math/Color.h"
#include "Render/CameraTypes.h"
#include "Render/Material.h"
#include "Render/SceneRendering.h"
#include "Render/Frustum.h"
#include "Render/BatchedSimpleElements.h"
#include "Components/CameraComponent.h"
#include "Components/PrimitiveComponent.h"
#include "RHI/BaseRHI.h"
#include "RHI/BaseBufferRHI.h"
#include "RHI/TypesRHI.h"

/**
 * @ingroup Engine
 * Typedef of show flags
 */
typedef uint64		EShowFlags;

/**
 * @ingroup Engine
 * Enumeration of show flag
 */
enum EShowFlag
{
	// General and game flags
	SHOW_None				= 0,			/**< Nothig show */
	SHOW_Sprite				= 1 << 0,		/**< Sprite show */
	SHOW_StaticMesh			= 1 << 1,		/**< Static mesh show */
	SHOW_Wireframe			= 1 << 2,		/**< Show all geometry in wireframe mode */
	SHOW_SimpleElements		= 1 << 3,		/**< Show simple elements (only for debug and WorldEd) */
	SHOW_DynamicElements	= 1 << 4,		/**< Show dynamic elements */

	SHOW_DefaultGame		= SHOW_Sprite | SHOW_StaticMesh | SHOW_SimpleElements | SHOW_DynamicElements,	/**< Default show flags for game */
	SHOW_DefaultEditor		= SHOW_Sprite | SHOW_StaticMesh | SHOW_SimpleElements | SHOW_DynamicElements	/**< Default show flags for editor */
};

/**
 * @ingroup Engine
 * A projection from scene space into a 2D screen region
 */
class FSceneView
{
public:
	/**
	 * Constructor
	 * 
	 * @param InProjectionMatrix		Projection matrix
	 * @param InViewMatrix				View matrix
	 * @param InSizeX					Size of viewport by X
	 * @param InSizeY					Size of viewport by Y
	 * @param InBackgroundColor			Background color
	 * @param InShowFlags				Show flags
	 */
	FSceneView( const FMatrix& InProjectionMatrix, const FMatrix& InViewMatrix, float InSizeX, float InSizeY, const FColor& InBackgroundColor, EShowFlags InShowFlags );

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

	/**
	 * Get background color
	 * @return Return background color
	 */
	FORCEINLINE const FColor& GetBackgroundColor() const
	{
		return backgroundColor;
	}

	/**
	 * Get show flags
	 * @return Return show flags
	 */
	FORCEINLINE EShowFlags GetShowFlags() const
	{
		return showFlags;
	}

	/**
	 * Get size of viewport by X
	 * @return Return size of viewport by X
	 */
	FORCEINLINE float GetSizeX() const
	{
		return sizeX;
	}

	/**
	 * Get size of viewport by Y
	 * @return Return size of viewport by Y
	 */
	FORCEINLINE float GetSizeY() const
	{
		return sizeY;
	}

private:
	FMatrix			viewMatrix;				/**< View matrix */
	FMatrix			projectionMatrix;		/**< Projection matrix */
	FMatrix			viewProjectionMatrix;	/**< View * Projection matrix */
	FFrustum		frustum;				/**< Frustum */
	FColor			backgroundColor;		/**< Background color */
	EShowFlags		showFlags;				/**< Show flags for the view */
	float			sizeX;					/**< Size X of viewport */
	float			sizeY;					/**< Size Y of viewport */
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
template< typename TDrawingPolicyType, bool InAllowWireframe = true >
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
		 * 
		 * @param InWireframeColor		Wireframe color
		 */
		FDrawingPolicyLink( const FColor& InWireframeColor = FColor::red )
#if !SHIPPING_BUILD
			: wireframeColor( InWireframeColor )
#endif // !SHIPPING_BUILD
		{}

		/**
		 * @brief Constructor
		 * 
		 * @param InDrawingPolicy		Drawing policy
		 * @param InWireframeColor		Wireframe color
		 */
		FDrawingPolicyLink( const TDrawingPolicyType& InDrawingPolicy, const FColor& InWireframeColor = FColor::red )
			: drawingPolicy( InDrawingPolicy )
#if !SHIPPING_BUILD
			, wireframeColor( InWireframeColor )
#endif // !SHIPPING_BUILD
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

#if !SHIPPING_BUILD
		FColor									wireframeColor;			/**< Wireframe color */
#endif // !SHIPPING_BUILD
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

		// Wireframe drawing available only with editor
#if !SHIPPING_BUILD
		TWireframeMeshDrawingPolicy< TDrawingPolicyType >		wireframeDrawingPolicy;		// Drawing policy for wireframe mode
		bool													bWireframe = InAllowWireframe && ( InSceneView.GetShowFlags() & SHOW_Wireframe );
#endif // !SHIPPING_BUILD

		for ( FMapDrawData::const_iterator it = meshes.begin(), itEnd = meshes.end(); it != itEnd; ++it )
		{
			bool						bIsInitedRenderState	= false;
			FDrawingPolicyLinkRef		drawingPolicyLink		= *it;
			FMeshDrawingPolicy*			drawingPolicy			= nullptr;

#if !SHIPPING_BUILD
			drawingPolicy				= !bWireframe ? &drawingPolicyLink->drawingPolicy : &wireframeDrawingPolicy;

			// If we use wireframe drawing policy - init him
			if ( bWireframe )
			{
				wireframeDrawingPolicy.Init( drawingPolicyLink->drawingPolicy.GetVertexFactory(), drawingPolicyLink->wireframeColor, drawingPolicyLink->drawingPolicy.GetDepthBias() );
			}
#else
			drawingPolicy				= &drawingPolicyLink->drawingPolicy;
#endif // !SHIPPING_BUILD

			// If drawing policy is not valid - skip meshes
			if ( !drawingPolicy->IsValid() )
			{
				continue;
			}

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
					drawingPolicy->SetRenderState( InDeviceContext );
					drawingPolicy->SetShaderParameters( InDeviceContext );
					bIsInitedRenderState = true;
				}

				// Draw mesh batch
				drawingPolicy->Draw( InDeviceContext, *itMeshBatch, InSceneView );
			}
		}
	}

private:
	FMapDrawData		meshes;						/**< Map of meshes sorted by materials for draw */
};

/**
 * @ingroup Engine
 * @brief Enumeration of scene depth group
 */
enum ESceneDepthGroup
{
	SDG_WorldEdBackground,	/**< Background of viewport in WorldEd */
	SDG_World,				/**< World */
	SDG_WorldEdForeground,	/**< Foreground of viewport in WorldEd */
	SDG_Max					/**< Num depth groups */
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
#if !SHIPPING_BUILD
		simpleElements.Clear();
#endif // !SHIPPING_BUILD

		dynamicMeshElements.Clear();
		staticMeshDrawList.Clear();
		spriteDrawList.Clear();
	}

	/**
	 * @brief Is empty
	 * @return Return TRUE if SDG is empty, else return FALSE
	 */
	FORCEINLINE bool IsEmpty() const
	{
		return staticMeshDrawList.GetNum() <= 0 && spriteDrawList.GetNum() <= 0 && dynamicMeshElements.GetNum() <= 0
#if !SHIPPING_BUILD
			&& simpleElements.IsEmpty()
#endif // !SHIPPING_BUILD
			;
	}

	// Simple elements use only for debug and WorldEd
#if !SHIPPING_BUILD
	FBatchedSimpleElements								simpleElements;			/**< Batched simple elements (lines, points, etc) */
#endif // !SHIPPING_BUILD

	FMeshDrawList< FStaticMeshDrawPolicy >				dynamicMeshElements;	/**< Draw list of dynamic meshes */
	FMeshDrawList< FStaticMeshDrawPolicy >				staticMeshDrawList;		/**< Draw list of static meshes */
	FMeshDrawList< FStaticMeshDrawPolicy >				spriteDrawList;			/**< Draw list of sprites */
};

/**
 * @ingroup Engine
 * @brief Base implementation of the scene manager
 */
class FBaseScene
{
public:
	/**
	 * @brief Destructor
	 */
	virtual ~FBaseScene() {}

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
	 * @brief Clear scene
	 */
	virtual void Clear() {}

	/**
	 * @brief Build SDGs for render scene from current view
	 * 
	 * @param InSceneView Current view of scene
	 */
	virtual void BuildSDGs( const FSceneView& InSceneView ) {}

	/**
	 * @brief Clear all instances in SDGs
	 */
	virtual void ClearSDGs() {}
};

/**
 * @ingroup Engine
 * @brief Main of scene manager containing all primitive components
 */
class FScene : public FBaseScene
{
public:
	/**
	 * @brief Destructor
	 */
	virtual ~FScene();

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
	 * @brief Clear scene
	 */
	virtual void Clear() override;

	/**
	 * @brief Build SDGs for render scene from current view
	 *
	 * @param InSceneView Current view of scene
	 */
	virtual void BuildSDGs( const FSceneView& InSceneView ) override;

	/**
	 * @brief Clear all instances in SDGs
	 */
	virtual void ClearSDGs() override;

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

//
// Serialization
//

FORCEINLINE FArchive& operator<<( FArchive& InArchive, ESceneDepthGroup& InValue )
{
	InArchive.Serialize( &InValue, sizeof( InValue ) );
	return InArchive;
}

FORCEINLINE FArchive& operator<<( FArchive& InArchive, const ESceneDepthGroup& InValue )
{
	check( InArchive.IsSaving() );
	InArchive.Serialize( ( void* )&InValue, sizeof( InValue ) );
	return InArchive;
}

#endif // !SCENE_H
