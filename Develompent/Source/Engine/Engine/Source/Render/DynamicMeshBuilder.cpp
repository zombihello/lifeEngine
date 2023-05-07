#include "Misc/EngineGlobals.h"
#include "Render/DynamicMeshBuilder.h"
#include "Render/Shaders/Shader.h"
#include "Render/Shaders/ShaderManager.h"
#include "RHI/BaseRHI.h"
#include "RHI/StaticStatesRHI.h"
#include "Render/SceneRendering.h"
#include "Render/Scene.h"

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
		vertexBufferRHI = g_RHI->CreateVertexBuffer( TEXT( "DynamicMesh" ), sizeof( SDynamicMeshVertexType ) * numVerteces, ( byte* )verteces.data(), RUF_Static );

		// Initialize vertex factory
		vertexFactory->AddVertexStream( SVertexStream{ vertexBufferRHI, sizeof( SDynamicMeshVertexType ) } );		// 0 stream slot
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
	SMeshBatch		meshBatch;
	meshBatch.indexBufferRHI	= indexBufferRHI;
	meshBatch.baseVertexIndex	= 0;
	meshBatch.firstIndex		= 0;
	meshBatch.numInstances		= 1;
	meshBatch.numPrimitives		= numPrimitives;
	meshBatch.primitiveType		= PT_TriangleList;
	meshBatch.instances.push_back( SMeshInstance{ InLocalToWorld, 
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