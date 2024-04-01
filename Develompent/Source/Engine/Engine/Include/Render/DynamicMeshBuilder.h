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

#include "Misc/RefCounted.h"
#include "Misc/RefCountPtr.h"
#include "System/Threading.h"
#include "Render/Material.h"
#include "Render/VertexFactory/DynamicMeshVertexFactory.h"
#include "Render/RenderResource.h"
#include "Render/DrawingPolicy.h"
#include "Render/HitProxies.h"
#include "RHI/BaseBufferRHI.h"

/**
 * @ingroup Engine
 * Reference to CDynamicMeshBuilder
 */
typedef TRefCountPtr< class CDynamicMeshBuilder >			DynamicMeshBuilderRef_t;

/**
 * @ingroup Engine
 * @brief Class for build and draw dynamic mesh
 */
class CDynamicMeshBuilder : public CRenderResource, public CRefCounted
{
public:
	/**
	 * @brief Constructor
	 */
	CDynamicMeshBuilder();

	/**
	 * @brief Add vertex
	 * 
	 * @param InVertex		Vertex
	 * @return Return index of vertex
	 */
	FORCEINLINE uint32 AddVertex( const DynamicMeshVertexType& InVertex )
	{
		CScopeLock	scopeLock( &readWriteMutex );
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
		CScopeLock	scopeLock( &readWriteMutex );
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
	 * @param InDrawingPolicy		Drawing policy
	 * @param InSceneView			Scene view
	 */
	template<typename TDrawingPolicyType>
	FORCEINLINE void Draw( class CBaseDeviceContextRHI* InDeviceContextRHI, const Matrix& InLocalToWorld, const TAssetHandle<CMaterial>& InMaterial, const class CSceneView& InSceneView ) const
	{
		AssertMsg( vertexFactory && vertexBufferRHI, TEXT( "Before draw dynamic mesh need call CDynamicMeshBuilder::Build" ) );

		TDrawingPolicyType		drawingPolicy;
		drawingPolicy.Init( vertexFactory, InMaterial );
		if ( drawingPolicy.IsValid() )
		{
			Draw( InDeviceContextRHI, InLocalToWorld, InMaterial, drawingPolicy, InSceneView );
		}
	}

	/**
	 * @brief Draw dynamic mesh
	 *
	 * @param InDeviceContextRHI	RHI device context
	 * @param InLocalToWorld		Local to world matrix
	 * @param InMaterial			Material
	 * @param InDrawingPolicy		Drawing policy
	 * @param InSceneView			Scene view
	 */
	void Draw( class CBaseDeviceContextRHI* InDeviceContextRHI, const Matrix& InLocalToWorld, const TAssetHandle<CMaterial>& InMaterial, CMeshDrawingPolicy& InDrawingPolicy, const class CSceneView& InSceneView ) const;

#if ENABLE_HITPROXY
	/**
	 * @brief Set hit proxy id
	 * @param InNewHitProxyId	New hit proxy id
	 */
	FORCEINLINE void SetHitProxyId( const CHitProxyId& InNewHitProxyId )
	{
		hitProxyId = InNewHitProxyId;
	}

	/**
	 * @brief Get hit proxy id
	 * @return Return hit proxy id
	 */
	FORCEINLINE CHitProxyId GetHitProxyId() const
	{
		return hitProxyId;
	}
#endif // ENABLE_HITPROXY

	/**
	 * @brief Get array of verteces
	 * @return Return array of verteces
	 */
	FORCEINLINE const std::vector< DynamicMeshVertexType >& GetVerteces() const
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

	uint32										numPrimitives;		/**< Number primitives in builded mesh */
	CThreadMutex								readWriteMutex;		/**< Read and write critical section */
	std::vector< DynamicMeshVertexType >		verteces;			/**< Array of verteces */
	std::vector< uint32 >						indeces;			/**< Array of indeces */
	VertexBufferRHIRef_t						vertexBufferRHI;	/**< Vertex buffer RHI */
	IndexBufferRHIRef_t							indexBufferRHI;		/**< Index buffer RHI */
	TRefCountPtr< CDynamicMeshVertexFactory >	vertexFactory;		/**< Vertex factory */

#if WITH_EDITOR
	CHitProxyId									hitProxyId;			/**< Hit proxy Id */
#endif // WITH_EDITOR
};

#endif // !DYNAMICMESHBUILDER_H