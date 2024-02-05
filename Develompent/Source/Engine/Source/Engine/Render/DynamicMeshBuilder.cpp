/**
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

#include "RHI/BaseRHI.h"
#include "RHI/StaticStatesRHI.h"
#include "Engine/Misc/EngineGlobals.h"
#include "Engine/Render/DynamicMeshBuilder.h"
#include "Engine/Render/Shaders/Shader.h"
#include "Engine/Render/Shaders/ShaderManager.h"
#include "Engine/Render/SceneRendering.h"
#include "Engine/Render/Scene.h"

/*
==================
CDynamicMeshBuilder::CDynamicMeshBuilder
==================
*/
CDynamicMeshBuilder::CDynamicMeshBuilder()
	: numPrimitives( 0 )
	, vertexFactory( new CDynamicMeshVertexFactory() )
{}

/*
==================
CDynamicMeshBuilder::InitRHI
==================
*/
void CDynamicMeshBuilder::InitRHI()
{
	CScopeLock		scopeLock( &readWriteCS );

	// Create vertex buffer
	uint32			numVerteces = verteces.size();
	if ( numVerteces > 0 )
	{
		vertexBufferRHI = g_RHI->CreateVertexBuffer( TEXT( "DynamicMesh" ), sizeof( DynamicMeshVertexType ) * numVerteces, ( byte* )verteces.data(), RUF_Static );

		// Initialize vertex factory
		vertexFactory->AddVertexStream( VertexStream{ vertexBufferRHI, sizeof( DynamicMeshVertexType ) } );		// 0 stream slot
		vertexFactory->Init();
	}

	// Create index buffer
	uint32			numIndeces = indeces.size();
	if ( numIndeces > 0 )
	{
		indexBufferRHI = g_RHI->CreateIndexBuffer( TEXT( "DynamicMesh" ), sizeof( uint32 ), sizeof( uint32 ) * numIndeces, ( byte* )indeces.data(), RUF_Static );
	}

	// Free source data
	verteces.clear();
	indeces.clear();
}

/*
==================
CDynamicMeshBuilder::ReleaseRHI
==================
*/
void CDynamicMeshBuilder::ReleaseRHI()
{
	vertexBufferRHI.SafeRelease();
	indexBufferRHI.SafeRelease();
	vertexFactory->ReleaseResource();
}

/*
==================
CDynamicMeshBuilder::Draw
==================
*/
void CDynamicMeshBuilder::Draw( class CBaseDeviceContextRHI* InDeviceContextRHI, const Matrix& InLocalToWorld, const TAssetHandle<CMaterial>& InMaterial, CMeshDrawingPolicy& InDrawingPolicy, const class CSceneView& InSceneView ) const
{
	AssertMsg( vertexFactory && vertexBufferRHI, TEXT( "Before draw dynamic mesh need call CDynamicMeshBuilder::Build" ) );
	
	// Init mesh batch
	MeshBatch		meshBatch;
	meshBatch.indexBufferRHI	= indexBufferRHI;
	meshBatch.baseVertexIndex	= 0;
	meshBatch.firstIndex		= 0;
	meshBatch.numInstances		= 1;
	meshBatch.numPrimitives		= numPrimitives;
	meshBatch.primitiveType		= PT_TriangleList;
	meshBatch.instances.push_back( MeshInstance{ InLocalToWorld, 
#if ENABLE_HITPROXY
								   hitProxyId 
#endif // ENABLE_HITPROXY
								   }  );

	// Draw mesh
	if ( InDrawingPolicy.IsValid() )
	{
		InDrawingPolicy.SetRenderState( InDeviceContextRHI );
		InDrawingPolicy.SetShaderParameters( InDeviceContextRHI );
		InDrawingPolicy.Draw( InDeviceContextRHI, meshBatch, InSceneView );
	}
	else
	{
		AssertMsg( false, TEXT( "Need init drawing policy before drawing" ) );
	}
}