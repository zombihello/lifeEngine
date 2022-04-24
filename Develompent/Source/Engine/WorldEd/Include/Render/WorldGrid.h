/**
 * @file
 * @addtogroup WorldEd WorldEd
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef WORLDGRID_H
#define WORLDGRID_H

#include <string>
#include <vector>

#include "Render/RenderResource.h"
#include "Containers/BulkData.h"
#include "Render/Scene.h"
#include "Render/VertexFactory/WorldGridVertexFactory.h"
#include "RHI/BaseBufferRHI.h"
#include "RHI/TypesRHI.h"

 /**
  * @ingroup WorldEd
  * @brief Reference to FWorldGrid
  */
typedef TRefCountPtr< class FWorldGrid >				FWorldGridRef;

/**
 * @ingroup WorldEd
 * @brief Implementation for world grid
 */
class FWorldGrid : public FRenderResource, public FRefCounted
{
public:
	/**
	 * Constructor
	 */
	FWorldGrid();

	/**
	 * Destructor
	 */
	~FWorldGrid();

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
	 * Update mesh of world grid
	 *
	 * @param[in] InMinSizeZ Min size grid by Z
	 * @param[in] InMaxSizeZ Max size grid by Z
	 * @param[in] InStepSize Step size
	 */
	void Update( float InMinSizeZ, float InMaxSizeZ, float InStepSize );

	/**
	 * Get base mesh batch info
	 * @return Return base mesh batch info without instances
	 */
	FORCEINLINE FMeshBatch GetMeshBatch() const
	{
		FMeshBatch		meshBatch;
		meshBatch.primitiveType		= PT_LineList;
		meshBatch.baseVertexIndex	= 0;
		meshBatch.firstIndex		= 0;
		meshBatch.numPrimitives		= numVerteces / 2;
		return meshBatch;
	}

	/**
	 * Get vertex factory
	 * @return Return vertex factory
	 */
	FORCEINLINE TRefCountPtr< FWorldGridVertexFactory > GetVertexFactory() const
	{
		return vertexFactory;
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
	 * Get number of verteces
	 * @return Return number of verteces
	 */
	FORCEINLINE uint32 GetNumVerteces() const
	{
		return numVerteces;
	}

private:
	TRefCountPtr< FWorldGridVertexFactory >		vertexFactory;			/**< Vertex factory */
	FBulkData< FWorldGridVertexType >			verteces;				/**< Array verteces to create RHI vertex buffer */
	FVertexBufferRHIRef							vertexBufferRHI;		/**< RHI vertex buffer */
	uint32										numVerteces;			/**< Number of verteces */
};

#endif // !WORLDGRID_H