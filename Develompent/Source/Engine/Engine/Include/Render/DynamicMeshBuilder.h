/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef DYNAMICMESHBUILDER_H
#define DYNAMICMESHBUILDER_H

#include <vector>

#include "System/ThreadingBase.h"
#include "Render/Material.h"
#include "Render/VertexFactory/DynamicMeshVertexFactory.h"
#include "Render/RenderResource.h"
#include "RHI/BaseBufferRHI.h"

/**
 * @ingroup Engine
 * @brief Class for build and draw dynamic mesh
 */
class FDynamicMeshBuilder : public FRenderResource
{
public:
	/**
	 * @brief Constructor
	 */
	FDynamicMeshBuilder();

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
	 * @brief Add vertex
	 * 
	 * @param InVertex		Vertex
	 * @return Return index of vertex
	 */
	FORCEINLINE uint32 AddVertex( const FDynamicMeshVertexType& InVertex )
	{
		FScopeLock	scopeLock( &readWriteCS );
		uint32		idVertex = verteces.size();
		verteces.push_back( InVertex );
		return idVertex;
	}

	/**
	 * @brief Add indeces of triangle
	 * @param InVerId0	Index of vertex 0
	 * @param InVerId1	Index of vertex 1
	 * @param InVerId2	Index of vertex 2
	 */
	FORCEINLINE void AddTriangle( uint32 InVerId0, uint32 InVerId1, uint32 InVerId2 )
	{
		FScopeLock	scopeLock( &readWriteCS );
		indeces.push_back( InVerId0 );
		indeces.push_back( InVerId1 );
		indeces.push_back( InVerId2 );
	}

	/**
	 * @brief Build mesh
	 */
	FORCEINLINE void Build()
	{
		numPrimitives = indeces.size() / 3;
		BeginUpdateResource( this );
	}

	/**
	 * @brief Clear data in mesh builder
	 */
	FORCEINLINE void Clear()
	{
		verteces.clear();
		indeces.clear();
		BeginReleaseResource( this );
	}

	/**
	 * @brief Draw dynamic mesh
	 *
	 * @param InDeviceContextRHI	RHI device context
	 * @param InLocalToWorld		Local to world matrix
	 * @param InMaterial			Material
	 * @param InSceneView			Scene view
	 */
	void Draw( class FBaseDeviceContextRHI* InDeviceContextRHI, const FMatrix& InLocalToWorld, FMaterial* InMaterial, const class FSceneView& InSceneView ) const;

	/**
	 * @brief Get array of verteces
	 * @return Return array of verteces
	 */
	FORCEINLINE const std::vector< FDynamicMeshVertexType >& GetVerteces() const
	{
		return verteces;
	}

	/**
	 * @brief Get array of indeces
	 * @return Return array of indeces
	 */
	FORCEINLINE const std::vector< uint32 >& GetIndeces() const
	{
		return indeces;
	}

private:
	uint32										numPrimitives;		/**< Number primitives in builded mesh */
	FCriticalSection							readWriteCS;		/**< Read and write critical section */
	std::vector< FDynamicMeshVertexType >		verteces;			/**< Array of verteces */
	std::vector< uint32 >						indeces;			/**< Array of indeces */
	FVertexBufferRHIRef							vertexBufferRHI;	/**< Vertex buffer RHI */
	FIndexBufferRHIRef							indexBufferRHI;		/**< Index buffer RHI */
	TRefCountPtr< FDynamicMeshVertexFactory >	vertexFactory;		/**< Vertex factory */
};

#endif // !DYNAMICMESHBUILDER_H