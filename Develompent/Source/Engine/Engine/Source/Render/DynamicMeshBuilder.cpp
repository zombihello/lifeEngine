#include "Misc/EngineGlobals.h"
#include "Render/DynamicMeshBuilder.h"
#include "Render/Shaders/Shader.h"
#include "Render/Shaders/ShaderManager.h"
#include "RHI/BaseRHI.h"
#include "RHI/StaticStatesRHI.h"
#include "Render/SceneRendering.h"
#include "Render/Scene.h"

FDynamicMeshBuilder::FDynamicMeshBuilder()
	: numPrimitives( 0 )
{}

void FDynamicMeshBuilder::InitRHI()
{
	FScopeLock		scopeLock( &readWriteCS );

	// Create vertex buffer
	uint32			numVerteces = verteces.size();
	if ( numVerteces > 0 )
	{
		vertexBufferRHI = GRHI->CreateVertexBuffer( TEXT( "DynamicMesh" ), sizeof( FDynamicMeshVertexType ) * numVerteces, ( byte* )verteces.data(), RUF_Static );

		// Initialize vertex factory
		vertexFactory = new FDynamicMeshVertexFactory();
		vertexFactory->AddVertexStream( FVertexStream{ vertexBufferRHI, sizeof( FDynamicMeshVertexType ) } );		// 0 stream slot
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

void FDynamicMeshBuilder::ReleaseRHI()
{
	vertexBufferRHI.SafeRelease();
	indexBufferRHI.SafeRelease();
	vertexFactory.SafeRelease();
}

void FDynamicMeshBuilder::Draw( class FBaseDeviceContextRHI* InDeviceContextRHI, const FMatrix& InLocalToWorld, const TAssetHandle<FMaterial>& InMaterial, const class FSceneView& InSceneView ) const
{
	checkMsg( vertexFactory && vertexBufferRHI, TEXT( "Before draw dynamic mesh need call FDynamicMeshBuilder::Build" ) );
	
	// Init mesh batch
	FMeshBatch		meshBatch;
	meshBatch.indexBufferRHI	= indexBufferRHI;
	meshBatch.baseVertexIndex	= 0;
	meshBatch.firstIndex		= 0;
	meshBatch.numInstances		= 1;
	meshBatch.numPrimitives		= numPrimitives;
	meshBatch.primitiveType		= PT_TriangleList;
	meshBatch.transformationMatrices.push_back( InLocalToWorld );

	// Draw mesh
	FStaticMeshDrawPolicy		drawPolicy;
	drawPolicy.Init( vertexFactory, InMaterial );
	if ( drawPolicy.IsValid() )
	{
		drawPolicy.SetRenderState( InDeviceContextRHI );
		drawPolicy.SetShaderParameters( InDeviceContextRHI );
		drawPolicy.Draw( InDeviceContextRHI, meshBatch, InSceneView );
	}
}