#include "Misc/EngineGlobals.h"
#include "Render/DynamicMeshBuilder.h"
#include "Render/Shaders/Shader.h"
#include "Render/Shaders/ShaderManager.h"
#include "RHI/BaseRHI.h"
#include "RHI/StaticStatesRHI.h"
#include "Render/SceneRendering.h"
#include "Render/Scene.h"

CDynamicMeshBuilder::CDynamicMeshBuilder()
	: numPrimitives( 0 )
	, vertexFactory( new CDynamicMeshVertexFactory() )
{}

void CDynamicMeshBuilder::InitRHI()
{
	CScopeLock		scopeLock( &readWriteCS );

	// Create vertex buffer
	uint32			numVerteces = verteces.size();
	if ( numVerteces > 0 )
	{
		vertexBufferRHI = GRHI->CreateVertexBuffer( TEXT( "DynamicMesh" ), sizeof( SDynamicMeshVertexType ) * numVerteces, ( byte* )verteces.data(), RUF_Static );

		// Initialize vertex factory
		vertexFactory->AddVertexStream( SVertexStream{ vertexBufferRHI, sizeof( SDynamicMeshVertexType ) } );		// 0 stream slot
		vertexFactory->Init();
	}

	// Create index buffer
	uint32			numIndeces = indeces.size();
	if ( numIndeces > 0 )
	{
		indexBufferRHI = GRHI->CreateIndexBuffer( TEXT( "DynamicMesh" ), sizeof( uint32 ), sizeof( uint32 ) * numIndeces, ( byte* )indeces.data(), RUF_Static );
	}

	// Free source data
	verteces.clear();
	indeces.clear();
}

void CDynamicMeshBuilder::ReleaseRHI()
{
	vertexBufferRHI.SafeRelease();
	indexBufferRHI.SafeRelease();
	vertexFactory->ReleaseResource();
}

void CDynamicMeshBuilder::Draw( class CBaseDeviceContextRHI* InDeviceContextRHI, const Matrix& InLocalToWorld, const TAssetHandle<CMaterial>& InMaterial, const class CSceneView& InSceneView ) const
{
	checkMsg( vertexFactory && vertexBufferRHI, TEXT( "Before draw dynamic mesh need call CDynamicMeshBuilder::Build" ) );
	
	// Init mesh batch
	SMeshBatch		meshBatch;
	meshBatch.indexBufferRHI	= indexBufferRHI;
	meshBatch.baseVertexIndex	= 0;
	meshBatch.firstIndex		= 0;
	meshBatch.numInstances		= 1;
	meshBatch.numPrimitives		= numPrimitives;
	meshBatch.primitiveType		= PT_TriangleList;
	meshBatch.instances.push_back( SMeshInstance{ InLocalToWorld }  );

	// Draw mesh
	CStaticMeshDrawPolicy		drawPolicy;
	drawPolicy.Init( vertexFactory, InMaterial );
	if ( drawPolicy.IsValid() )
	{
		drawPolicy.SetRenderState( InDeviceContextRHI );
		drawPolicy.SetShaderParameters( InDeviceContextRHI );
		drawPolicy.Draw( InDeviceContextRHI, meshBatch, InSceneView );
	}
}