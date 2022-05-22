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

#include "RenderResource.h"
#include "Containers/BulkData.h"
#include "System/Package.h"
#include "Render/Material.h"
#include "Render/VertexFactory/StaticMeshVertexFactory.h"
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
	FORCEINLINE const std::vector< TAssetHandle<FMaterial> > GetMaterials() const
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

private:
	TRefCountPtr< FStaticMeshVertexFactory >	vertexFactory;			/**< Vertex factory */
	std::vector< TAssetHandle<FMaterial> >		materials;				/**< Array materials in mesh */
	std::vector< FStaticMeshSurface >			surfaces;				/**< Array surfaces in mesh */
	FBulkData< FStaticMeshVertexType >			verteces;				/**< Array verteces to create RHI vertex buffer */	
	FBulkData< uint32 >							indeces;				/**< Array indeces to create RHI index buffer */
	FVertexBufferRHIRef							vertexBufferRHI;		/**< RHI vertex buffer */
	FIndexBufferRHIRef							indexBufferRHI;			/**< RHI index buffer */
};

//
// Serialization
//

FORCEINLINE FArchive& operator<<( FArchive& InArchive, FStaticMeshSurface& InValue )
{
	InArchive << InValue.materialID;
	InArchive << InValue.baseVertexIndex;
	InArchive << InValue.firstIndex;
	InArchive << InValue.numPrimitives;
	return InArchive;
}

FORCEINLINE FArchive& operator<<( FArchive& InArchive, const FStaticMeshSurface& InValue )
{
	InArchive << InValue.materialID;
	InArchive << InValue.baseVertexIndex;
	InArchive << InValue.firstIndex;
	InArchive << InValue.numPrimitives;
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