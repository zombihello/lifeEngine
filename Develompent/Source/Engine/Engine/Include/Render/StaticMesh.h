/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef STATICMESH_H
#define STATICMESH_H

#include <string>
#include <vector>
#include <unordered_map>

#include "RenderResource.h"
#include "Containers/BulkData.h"
#include "Misc/SharedPointer.h"
#include "System/Package.h"
#include "Render/Material.h"
#include "Render/VertexFactory/StaticMeshVertexFactory.h"
#include "Render/Scene.h"
#include "RHI/BaseBufferRHI.h"
#include "RHI/TypesRHI.h"

/**
 * @ingroup Engine
 * Surface in static mesh
 */
struct FStaticMeshSurface
{
	uint32			materialID;				/**< Material ID */
	uint32			baseVertexIndex;		/**< First index vertex in vertex buffer */
	uint32			firstIndex;				/**< First index */
	uint32			numPrimitives;			/**< Number primitives in the surface */
};

/**
 * @ingroup Engine
 * @brief Implementation for static mesh
 */
class FStaticMesh : public FAsset, public FRenderResource
{
public:
	/**
	 * @brief Typedef of drawing policy link
	 */
	typedef FMeshDrawList<FStaticMeshDrawPolicy>::FDrawingPolicyLink		FDrawingPolicyLink;

	/**
	 * @brief Typedef of reference on drawing policy link in scene
	 */
	typedef FMeshDrawList<FStaticMeshDrawPolicy>::FDrawingPolicyLinkRef		FDrawingPolicyLinkRef;

	/**
	 * @brief Elements of drawing policy link to SDG
	 */
	struct FElementDrawingPolicyLink
	{
		bool									bDirty;					/**< Is dirty this element */
		std::vector<FDrawingPolicyLinkRef>		drawingPolicyLinks;		/**< Array of reference to drawing policy link in scene */
		std::vector<const FMeshBatch*>			meshBatchLinks;			/**< Array of references to mesh batch in drawing policy link */
	};

	/**
	 * Constructor
	 */
	FStaticMesh();

	/**
	 * Destructor
	 */
	~FStaticMesh();

	/**
	 * @brief Initializes the RHI resources used by this resource.
	 * Called when the resource is initialized.
	 * This is only called by the rendering thread.
	 */
	virtual void InitRHI() override;

	/**
	 * @brief Releases the RHI resources used by this resource.
	 * Called when the resource is released.
	 * This is only called by the rendering thread.
	 */
	virtual void ReleaseRHI() override;

	/**
	 * Serialize
	 *
	 * @param[in] InArchive Archive
	 */
	virtual void Serialize( class FArchive& InArchive ) override;

	/**
	 * Set data mesh
	 * 
	 * @param[in] InVerteces Array mesh verteces
	 * @param[in] InIndeces Array mesh indeces
	 * @param[in] InSurfaces Array surfaces in mesh
	 * @param[in] InMaterials Array materials in mesh
	 */
	void SetData( const std::vector< FStaticMeshVertexType >& InVerteces, const std::vector< uint32 >& InIndeces, const std::vector< FStaticMeshSurface >& InSurfaces, std::vector< TAssetHandle<FMaterial> >& InMaterials );

	/**
	 * Set material
	 * 
	 * @param[in] InMaterialIndex Index material
	 * @param[in] InNewMaterial New material
	 */
	void SetMaterial( uint32 InMaterialIndex, const TAssetHandle<FMaterial>& InNewMaterial );

	/**
	 * Get vertex factory
	 * @return Return vertex factory
	 */
	FORCEINLINE TRefCountPtr< FStaticMeshVertexFactory > GetVertexFactory() const
	{
		return vertexFactory;
	}

	/**
	 * Get number of surfaces
	 * @return Return number of surfaces in array
	 */
	FORCEINLINE uint32 GetNumSurfaces() const
	{
		return surfaces.size();
	}

	/**
	 * Get surfaces
	 * @return Return array surfaces
	 */
	FORCEINLINE const std::vector< FStaticMeshSurface > GetSurfaces() const
	{
		return surfaces;
	}

	/**
	 * Get materials
	 * @return Return array materials
	 */
	FORCEINLINE const std::vector< TAssetHandle<FMaterial> >& GetMaterials() const
	{
		return materials;
	}

	/**
	 * Get material by index
	 * 
	 * @param[in] InMaterialIndex Material index
	 * @return Return material, if index not valid return nullptr
	 */
	FORCEINLINE TAssetHandle<FMaterial> GetMaterial( uint32 InMaterialIndex ) const
	{
		if ( InMaterialIndex >= materials.size() )
		{
			return nullptr;
		}

		return materials[ InMaterialIndex ];
	}

	/**
	 * Get RHI vertex buffer
	 * @return Return RHI vertex buffer, if not created return nullptr
	 */
	FORCEINLINE FVertexBufferRHIRef GetVertexBufferRHI() const
	{
		return vertexBufferRHI;
	}

	/**
	 * Get RHI index buffer
	 * @return Return RHI index buffer, if not created return nullptr
	 */
	FORCEINLINE FIndexBufferRHIRef GetIndexBufferRHI() const
	{
		return indexBufferRHI;
	}

	/**
	 * Get number materials
	 * @return Return number materials in mesh
	 */
	FORCEINLINE uint32 GetNumMaterials() const
	{
		return materials.size();
	}

	/**
	 * @brief Get array of verteces
	 * @return Return array of verteces
	 */
	FORCEINLINE const FBulkData<FStaticMeshVertexType>& GetVerteces() const
	{
		return verteces;
	}

	/**
	 * @brief Get array of indeces
	 * @return Return array of indeces
	 */
	FORCEINLINE const FBulkData<uint32>& GetIndeces() const
	{
		return indeces;
	}

	/**
	 * @brief Adds a drawing policy link in SDGs
	 * @note When FStaticMesh is deleting, all this drawing policy links unlinks from scenes
	 * 
	 * @param InSDG		Scene depth group
	 * @return Return pointer to drawing policy link for SDG
	 */
	FORCEINLINE TSharedPtr<FElementDrawingPolicyLink> LinkDrawList( FSceneDepthGroup& InSDG )
	{
		// If already added drawing policy link for this scene depth group - return exist element
		{
			auto	itElement = elementDrawingPolicyMap.find( &InSDG );
			if ( itElement != elementDrawingPolicyMap.end() )
			{
				return itElement->second;
			}
		}

		// Allocate new element
		TSharedPtr<FElementDrawingPolicyLink>		element = MakeCustomDrawingPolicyLink( InSDG );

		// Add to cache and return created element
		elementDrawingPolicyMap[ &InSDG ] = element;
		return element;
	}

	/**
	 * @brief Adds a drawing policy link with override materials in SDGs
	 * @note When FStaticMesh is deleting, all this drawing policy links unlinks from scenes
	 * 
	 * @param InSDG					Scene depth group
	 * @param InOverrideMaterials	Array of override materials
	 * @return Return pointer to drawing policy link for SDG
	 */
	FORCEINLINE TSharedPtr<FElementDrawingPolicyLink> LinkDrawList( FSceneDepthGroup& InSDG, const std::vector< TAssetHandle<FMaterial> >& InOverrideMaterials )
	{
		// TODO BS yehor.pohuliaka - AHTUNG! Need change key in 'elementDrawingPolicyMap' for correct caching custom drawing policy link
		// If already added drawing policy link for this scene depth group - return exist element
		{
			auto	itElement = elementDrawingPolicyMap.find( &InSDG );
			if ( itElement != elementDrawingPolicyMap.end() )
			{
				return itElement->second;
			}
		}

		// Allocate new element
		TSharedPtr<FElementDrawingPolicyLink>		element = MakeCustomDrawingPolicyLink( InSDG, ( std::vector< TAssetHandle<FMaterial> >* ) &InOverrideMaterials );

		// Add to cache and return created element
		elementDrawingPolicyMap[ &InSDG ] = element;
		return element;
	}

	/**
	 * @brief Removes a drawing policy link from SDGs
	 * @warning After work of this method pointer InDrawingPolicyLink will is not valid 
	 * @note When FStaticMesh is deleting, all this drawing policy links unlinks from scenes
	 * 
	 * @param InSDG					Scene depth group
	 * @param InDrawingPolicyLink	Drawing policy link to delete
	 */
	void UnlinkDrawList( FSceneDepthGroup& InSDG, TSharedPtr<FElementDrawingPolicyLink>& InDrawingPolicyLink );

	/**
	 * @brief Get drawing policy link for SDG
	 * 
	 * @param InSDG		Scene depth group
	 * @return Return pointer to drawing policy link for SDG. If not exist return NULL
	 */
	FORCEINLINE TSharedPtr<FElementDrawingPolicyLink> GetDrawingPolicyLink( const FSceneDepthGroup& InSDG ) const
	{
		// Find element in cache drawing policy map
		auto		itElement = elementDrawingPolicyMap.find( ( FSceneDepthGroup* ) &InSDG );
		if ( itElement == elementDrawingPolicyMap.end() )
		{
			return nullptr;
		}

		return itElement->second;
	}

private:
	/**
	 * @brief Create element drawing policy link
	 * @note For cache this drawing policy link need call LinkDrawList for add him on cache
	 *
	 * @param InSDG					Scene depth group
	 * @param InOverrideMaterials	Optional. Pointer to array of override materials
	 * @return Return pointer to drawing policy link for SDG
	 */
	TSharedPtr<FElementDrawingPolicyLink> MakeCustomDrawingPolicyLink( FSceneDepthGroup& InSDG, std::vector< TAssetHandle<FMaterial> >* InOverrideMaterials = nullptr );

	TRefCountPtr< FStaticMeshVertexFactory >										vertexFactory;				/**< Vertex factory */
	std::vector< TAssetHandle<FMaterial> >											materials;					/**< Array materials in mesh */
	std::vector< FStaticMeshSurface >												surfaces;					/**< Array surfaces in mesh */
	FBulkData< FStaticMeshVertexType >												verteces;					/**< Array verteces to create RHI vertex buffer */
	FBulkData< uint32 >																indeces;					/**< Array indeces to create RHI index buffer */
	FVertexBufferRHIRef																vertexBufferRHI;			/**< RHI vertex buffer */
	FIndexBufferRHIRef																indexBufferRHI;				/**< RHI index buffer */
	std::unordered_map<FSceneDepthGroup*, TSharedPtr<FElementDrawingPolicyLink>>	elementDrawingPolicyMap;	/**< Map of adds a drawing policy link to SDGs */
};

//
// Serialization
//

FORCEINLINE FArchive& operator<<( FArchive& InArchive, FStaticMeshSurface& InValue )
{
	InArchive.Serialize( &InValue, sizeof( FStaticMeshSurface ) );
	return InArchive;
}

FORCEINLINE FArchive& operator<<( FArchive& InArchive, const FStaticMeshSurface& InValue )
{
	InArchive.Serialize( ( FStaticMeshSurface* ) &InValue, sizeof( FStaticMeshSurface ) );
	return InArchive;
}

FORCEINLINE FArchive& operator<<( FArchive& InArchive, TAssetHandle<FStaticMesh>& InValue )
{
	TAssetHandle<FAsset>	asset = InValue;
	InArchive << asset;

	if ( InArchive.IsLoading() )
	{
		InValue = asset;
	}
	return InArchive;
}

FORCEINLINE FArchive& operator<<( FArchive& InArchive, const TAssetHandle<FStaticMesh>& InValue )
{
	check( InArchive.IsSaving() );
	InArchive << ( TAssetHandle<FAsset> )InValue;
	return InArchive;
}

#endif // !STATICMESH_H