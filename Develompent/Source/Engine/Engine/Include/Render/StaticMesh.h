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
struct SStaticMeshSurface
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
class CStaticMesh : public CAsset, public CRenderResource
{
public:
	/**
	 * @brief Typedef of drawing policy link
	 */
	typedef CMeshDrawList<CStaticMeshDrawPolicy>::SDrawingPolicyLink					DrawingPolicyLink_t;

	/**
	 * @brief Typedef of reference on drawing policy link in scene
	 */
	typedef CMeshDrawList<CStaticMeshDrawPolicy>::DrawingPolicyLinkRef_t				DrawingPolicyLinkRef_t;

#if ENABLE_HITPROXY
	/**
	 * @brief Typedef of hit proxy drawing policy link
	 */
	typedef CMeshDrawList<CHitProxyDrawingPolicy, false>::SDrawingPolicyLink			HitProxyDrawingPolicyLink_t;

	/**
	 * @brief Typedef of reference on drawing policy link in scene
	 */
	typedef CMeshDrawList<CHitProxyDrawingPolicy, false>::DrawingPolicyLinkRef_t		HitProxyDrawingPolicyLinkRef_t;
#endif // ENABLE_HITPROXY

	/**
	 * @brief Elements of drawing policy link to SDG
	 */
	struct SElementDrawingPolicyLink
	{
		/**
		 * @brief Constructor
		 */
		FORCEINLINE SElementDrawingPolicyLink()
			: bDirty( false )
			, overrideHash( 0 )
		{}

		bool											bDirty;					/**< Is dirty this element */
		std::vector<DrawingPolicyLinkRef_t>				drawingPolicyLinks;		/**< Array of reference to drawing policy link in scene */
		std::vector<const SMeshBatch*>					meshBatchLinks;			/**< Array of references to mesh batch in drawing policy link */
		uint64											overrideHash;			/**< Hash of overrided segments (custom materials) */

#if ENABLE_HITPROXY
		std::vector<HitProxyDrawingPolicyLinkRef_t>		hitProxyDrawingPolicyLinks;		/**< Array of references to hit proxy drawing policy link in scene */
#endif // ENABLE_HITPROXY
	};

	/**
	 * Constructor
	 */
	CStaticMesh();

	/**
	 * Destructor
	 */
	~CStaticMesh();

	/**
	 * Serialize
	 *
	 * @param[in] InArchive Archive
	 */
	virtual void Serialize( class CArchive& InArchive ) override;

	/**
	 * Set data mesh
	 * 
	 * @param[in] InVerteces Array mesh verteces
	 * @param[in] InIndeces Array mesh indeces
	 * @param[in] InSurfaces Array surfaces in mesh
	 * @param[in] InMaterials Array materials in mesh
	 */
	void SetData( const std::vector< SStaticMeshVertexType >& InVerteces, const std::vector< uint32 >& InIndeces, const std::vector< SStaticMeshSurface >& InSurfaces, std::vector< TAssetHandle<CMaterial> >& InMaterials );

	/**
	 * Set material
	 * 
	 * @param[in] InMaterialIndex Index material
	 * @param[in] InNewMaterial New material
	 */
	void SetMaterial( uint32 InMaterialIndex, const TAssetHandle<CMaterial>& InNewMaterial );

	/**
	 * Get vertex factory
	 * @return Return vertex factory
	 */
	FORCEINLINE TRefCountPtr< CStaticMeshVertexFactory > GetVertexFactory() const
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
	FORCEINLINE const std::vector< SStaticMeshSurface > GetSurfaces() const
	{
		return surfaces;
	}

	/**
	 * Get materials
	 * @return Return array materials
	 */
	FORCEINLINE const std::vector< TAssetHandle<CMaterial> >& GetMaterials() const
	{
		return materials;
	}

	/**
	 * Get material by index
	 * 
	 * @param[in] InMaterialIndex Material index
	 * @return Return material, if index not valid return nullptr
	 */
	FORCEINLINE TAssetHandle<CMaterial> GetMaterial( uint32 InMaterialIndex ) const
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
	FORCEINLINE VertexBufferRHIRef_t GetVertexBufferRHI() const
	{
		return vertexBufferRHI;
	}

	/**
	 * Get RHI index buffer
	 * @return Return RHI index buffer, if not created return nullptr
	 */
	FORCEINLINE IndexBufferRHIRef_t GetIndexBufferRHI() const
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
	FORCEINLINE const ÑBulkData<SStaticMeshVertexType>& GetVerteces() const
	{
		return verteces;
	}

	/**
	 * @brief Get array of indeces
	 * @return Return array of indeces
	 */
	FORCEINLINE const ÑBulkData<uint32>& GetIndeces() const
	{
		return indeces;
	}

	/**
	 * @brief Get dependent assets
	 * @param OutDependentAssets	Output set of dependent assets
	 * @param InFilter				Filter of getting assets by type. If setted AT_Unknown return all types
	 */
	virtual void GetDependentAssets( SetDependentAssets_t& OutDependentAssets, EAssetType InFilter = AT_Unknown ) const override;

	/**
	 * @brief Reload dependent assets
	 * @param InForce	Is need force reload assets
	 */
	virtual void ReloadDependentAssets( bool InForce = false );

	/**
	 * @brief Adds a drawing policy link in SDGs
	 * @note When CStaticMesh is deleting, all this drawing policy links unlinks from scenes
	 * 
	 * @param InSDG		Scene depth group
	 * @return Return pointer to drawing policy link for SDG
	 */
	TSharedPtr<SElementDrawingPolicyLink> LinkDrawList( SSceneDepthGroup& InSDG );

	/**
	 * @brief Adds a drawing policy link with override materials in SDGs
	 * @note When CStaticMesh is deleting, all this drawing policy links unlinks from scenes
	 * 
	 * @param InSDG					Scene depth group
	 * @param InOverrideMaterials	Array of override materials
	 * @return Return pointer to drawing policy link for SDG
	 */
	TSharedPtr<SElementDrawingPolicyLink> LinkDrawList( SSceneDepthGroup& InSDG, const std::vector< TAssetHandle<CMaterial> >& InOverrideMaterials );

	/**
	 * @brief Removes a drawing policy link from SDGs
	 * @warning After work of this method pointer InDrawingPolicyLink will is not valid 
	 * @note When CStaticMesh is deleting, all this drawing policy links unlinks from scenes
	 * 
	 * @param InSDG					Scene depth group
	 * @param InDrawingPolicyLink	Drawing policy link to delete
	 */
	void UnlinkDrawList( SSceneDepthGroup& InSDG, TSharedPtr<SElementDrawingPolicyLink>& InDrawingPolicyLink );

protected:
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

private:
	/**
	 * @brief Key for containing element drawing policy link in map
	 */
	struct SElementKeyDrawingPolicyLink
	{
		/**
		 * @brief Hash function for STL containers
		 */
		struct SHashFunction
		{
			/**
			 * @brief Calculate hash
			 * @param InElementKey	Element key of drawing policy link
			 */
			FORCEINLINE std::size_t operator()( const SElementKeyDrawingPolicyLink& InElementKey ) const
			{
				return appMemFastHash( &( *InElementKey.SDG ), InElementKey.overrideHash );
			}
		};

		SSceneDepthGroup*		SDG;			/**< Scene depth group */
		uint64					overrideHash;	/**< Hash of overrided segments (custom materials) */

		/**
		 * @brief Override operator ==
		 * @return Return TRUE if two values is equal
		 */
		FORCEINLINE bool operator==( const SElementKeyDrawingPolicyLink& InRight ) const
		{
			return SDG == InRight.SDG && overrideHash == InRight.overrideHash;
		}
	};

	/**
	 * @brief Typedef of element drawing policy map
	 */
	typedef std::unordered_map< SElementKeyDrawingPolicyLink, TSharedPtr<SElementDrawingPolicyLink>, SElementKeyDrawingPolicyLink::SHashFunction >		ElementDrawingPolicyMap_t;

	/**
	 * @brief Create element drawing policy link
	 * @note For cache this drawing policy link need call LinkDrawList for add him on cache
	 *
	 * @param InSDG					Scene depth group
	 * @param InOverrideHash		Hash of overrided segments (custom materials)
	 * @param InOverrideMaterials	Optional. Pointer to array of override materials
	 * @return Return pointer to drawing policy link for SDG
	 */
	TSharedPtr<SElementDrawingPolicyLink> MakeDrawingPolicyLink( SSceneDepthGroup& InSDG, uint64 InOverrideHash = 0, std::vector< TAssetHandle<CMaterial> >* InOverrideMaterials = nullptr );

	/**
	 * @brief Mark dirty all element drawing polices
	 */
	FORCEINLINE void MarkDirtyAllElementDrawingPolices()
	{
		// Mark dirty all drawing policy links
		for ( auto itElement = elementDrawingPolicyMap.begin(), itElementEnd = elementDrawingPolicyMap.end(); itElement != itElementEnd; ++itElement )
		{
			itElement->second->bDirty = true;
		}
	}

	TRefCountPtr< CStaticMeshVertexFactory >	vertexFactory;				/**< Vertex factory */
	std::vector< TAssetHandle<CMaterial> >		materials;					/**< Array materials in mesh */
	std::vector< SStaticMeshSurface >			surfaces;					/**< Array surfaces in mesh */
	ÑBulkData< SStaticMeshVertexType >			verteces;					/**< Array verteces to create RHI vertex buffer */
	ÑBulkData< uint32 >							indeces;					/**< Array indeces to create RHI index buffer */
	VertexBufferRHIRef_t						vertexBufferRHI;			/**< RHI vertex buffer */
	IndexBufferRHIRef_t							indexBufferRHI;				/**< RHI index buffer */
	ElementDrawingPolicyMap_t					elementDrawingPolicyMap;	/**< Map of adds a drawing policy link to SDGs */
};

//
// Serialization
//

FORCEINLINE CArchive& operator<<( CArchive& InArchive, SStaticMeshSurface& InValue )
{
	InArchive.Serialize( &InValue, sizeof( SStaticMeshSurface ) );
	return InArchive;
}

FORCEINLINE CArchive& operator<<( CArchive& InArchive, const SStaticMeshSurface& InValue )
{
	InArchive.Serialize( ( SStaticMeshSurface* ) &InValue, sizeof( SStaticMeshSurface ) );
	return InArchive;
}

FORCEINLINE CArchive& operator<<( CArchive& InArchive, TAssetHandle<CStaticMesh>& InValue )
{
	TAssetHandle<CAsset>	asset = InValue;
	InArchive << asset;

	if ( InArchive.IsLoading() )
	{
		InValue = asset;
	}
	return InArchive;
}

FORCEINLINE CArchive& operator<<( CArchive& InArchive, const TAssetHandle<CStaticMesh>& InValue )
{
	check( InArchive.IsSaving() );
	InArchive << ( TAssetHandle<CAsset> )InValue;
	return InArchive;
}

#endif // !STATICMESH_H