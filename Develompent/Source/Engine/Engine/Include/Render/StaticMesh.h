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
#include "System/Package.h"
#include "Render/Material.h"
#include "Render/VertexFactory/StaticMeshVertexFactory.h"
#include "RHI/BaseBufferRHI.h"
#include "RHI/TypesRHI.h"

 /**
  * @ingroup Engine Engine
  * @brief Reference to FTexture2D
  */
typedef TRefCountPtr< class FStaticMesh >				FStaticMeshRef;

/**
 * @ingroup Engine
 * Surface in static mesh
 */
struct FStaticMeshSurface
{
	uint32			materialID;				/**< Material ID */
	uint32			startVertexIndex;		/**< Start vertex index in buffer */
	uint32			numVertexIndeces;		/**< Number vertex indeces in surface */
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
	void SetData( const std::vector< FStaticMeshVertexType >& InVerteces, const std::vector< uint32 >& InIndeces, const std::vector< FStaticMeshSurface >& InSurfaces, std::vector< FMaterialRef >& InMaterials );

	/**
	 * Set material
	 * 
	 * @param[in] InMaterialIndex Index material
	 * @param[in] InNewMaterial New material
	 */
	void SetMaterial( uint32 InMaterialIndex, FMaterialRef InNewMaterial );

	/**
	 * Get vertex factory
	 * @return Return vertex factory
	 */
	FORCEINLINE TRefCountPtr< FStaticMeshVertexFactory > GetVertexFactory() const
	{
		return vertexFactory;
	}

private:
	TRefCountPtr< FStaticMeshVertexFactory >	vertexFactory;			/**< Vertex factory */
	std::vector< FMaterialRef >					materials;				/**< Array materials in mesh */
	std::vector< FStaticMeshSurface >			surfaces;				/**< Array surfaces in mesh */
	std::vector< FStaticMeshVertexType >		verteces;				/**< Array verteces to create RHI vertex buffer */
	std::vector< uint32 >						indeces;				/**< Array indeces to create RHI index buffer */
	FVertexBufferRHIRef							vertexBufferRHI;		/**< RHI vertex buffer */
	FIndexBufferRHIRef							indexBufferRHI;			/**< RHI index buffer */
};

//
// Serialization
//

FORCEINLINE FArchive& operator<<( FArchive& InArchive, FStaticMeshSurface& InValue )
{
	InArchive << InValue.materialID;
	InArchive << InValue.startVertexIndex;
	InArchive << InValue.numVertexIndeces;
	return InArchive;
}

FORCEINLINE FArchive& operator<<( FArchive& InArchive, const FStaticMeshSurface& InValue )
{
	InArchive << InValue.materialID;
	InArchive << InValue.startVertexIndex;
	InArchive << InValue.numVertexIndeces;
	return InArchive;
}

FORCEINLINE FArchive& operator<<( FArchive& InArchive, FStaticMeshRef& InValue )
{
	if ( InArchive.IsSaving() )
	{
		InArchive << ( InValue ? InValue->GetAssetReference() : FAssetReference() );
	}
	else
	{
		FAssetReference			assetReference;
		InArchive << assetReference;
		if ( assetReference.IsValid() )
		{
			InValue = GPackageManager->FindAsset( assetReference.pathPackage, assetReference.hash );
		}
	}

	return InArchive;
}

FORCEINLINE FArchive& operator<<( FArchive& InArchive, const FStaticMeshRef& InValue )
{
	check( InArchive.IsSaving() );
	InArchive << ( InValue ? InValue->GetAssetReference() : FAssetReference() );
	return InArchive;
}

#endif // !STATICMESH_H