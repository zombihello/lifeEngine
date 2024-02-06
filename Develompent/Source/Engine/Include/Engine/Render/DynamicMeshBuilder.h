/**
 * @file
 * @addtogroup Engine Engine
 *
 * ************************************************************
 *                  This file is part of:
 *                      LIFEENGINE
 *          https://github.com/zombihello/lifeEngine
 * ************************************************************
 * Copyright (C) 2024 Yehor Pohuliaka.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef DYNAMICMESHBUILDER_H
#define DYNAMICMESHBUILDER_H

#include <vector>

#include "Core/Misc/RefCounted.h"
#include "Core/Misc/RefCountPtr.h"
#include "Core/System/Threading.h"
#include "Engine/Render/Material.h"
#include "Engine/Render/VertexFactory/DynamicMeshVertexFactory.h"
#include "Engine/Render/RenderResource.h"
#include "Engine/Render/DrawingPolicy.h"
#include "Engine/Render/HitProxies.h"
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