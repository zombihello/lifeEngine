#include "Misc/EngineGlobals.h"
#include "Containers/String.h"
#include "RHI/BaseRHI.h"
#include "RHI/BaseStateRHI.h"
#include "RHI/TypesRHI.h"
#include "Render/VertexFactory/VertexFactory.h"
#include "Render/SceneUtils.h"
#include "Render/Scene.h"
#include "Render/DrawingPolicy.h"

FMeshDrawingPolicy::FMeshDrawingPolicy()
	: bInit( false )
	, depthBias( 0.f )
	, hash( INVALID_HASH )
{}

FMeshDrawingPolicy::~FMeshDrawingPolicy()
{}

void FMeshDrawingPolicy::InitInternal( class FVertexFactory* InVertexFactory, const TWeakPtr<FMaterial>& InMaterial, float InDepthBias /* = 0.f */ )
{
	TSharedPtr<FMaterial>		materialRef = InMaterial.Pin();
	checkMsg( InVertexFactory && materialRef, TEXT( "Vertex factory and material must be valid for init drawing policy" ) );

	uint64			vertexFactoryHash = InVertexFactory->GetType()->GetHash();
	vertexShader	= materialRef->GetShader( vertexFactoryHash, SF_Vertex );
	pixelShader		= materialRef->GetShader( vertexFactoryHash, SF_Pixel );

	hash			= appMemFastHash( material, InVertexFactory->GetTypeHash() );
	vertexFactory	= InVertexFactory;
	material		= InMaterial;
	depthBias		= InDepthBias;
	bInit			= true;
}

void FMeshDrawingPolicy::SetRenderState( class FBaseDeviceContextRHI* InDeviceContextRHI )
{
	check( bInit );

	vertexFactory->Set( InDeviceContextRHI );
	GRHI->SetRasterizerState( InDeviceContextRHI, GetRasterizerState() );
	GRHI->SetBoundShaderState( InDeviceContextRHI, GetBoundShaderState() );
}

void FMeshDrawingPolicy::SetShaderParameters( class FBaseDeviceContextRHI* InDeviceContextRHI )
{
	check( bInit );

	TSharedPtr<FMaterial>		materialRef = material.Pin();
	if ( !materialRef )
	{
		return;
	}

	vertexShader->SetConstantParameters( InDeviceContextRHI, vertexFactory, materialRef );
	pixelShader->SetConstantParameters( InDeviceContextRHI, vertexFactory, materialRef );
}

void FMeshDrawingPolicy::Draw( class FBaseDeviceContextRHI* InDeviceContextRHI, const struct FMeshBatch& InMeshBatch, const class FSceneView& InSceneView )
{
	TSharedPtr<FMaterial>		materialRef = material.Pin();
	SCOPED_DRAW_EVENT( EventDraw, DEC_MATERIAL, FString::Format( TEXT( "Material %s" ), materialRef ? materialRef->GetAssetName().c_str() : TEXT( "Unloaded" ) ).c_str());

	// If vertex factory not support instancig - draw without it
	if ( !vertexFactory->SupportsInstancing() )
	{
		if ( InMeshBatch.indexBufferRHI )
		{
			for ( uint32 instanceId = 0; instanceId < InMeshBatch.numInstances; ++instanceId )
			{
				vertexShader->SetMesh( InDeviceContextRHI, InMeshBatch, vertexFactory, &InSceneView, 1, instanceId );
				GRHI->CommitConstants( InDeviceContextRHI );
				GRHI->DrawIndexedPrimitive( InDeviceContextRHI, InMeshBatch.indexBufferRHI, InMeshBatch.primitiveType, InMeshBatch.baseVertexIndex, InMeshBatch.firstIndex, InMeshBatch.numPrimitives );
			}
		}
		else
		{
			for ( uint32 instanceId = 0; instanceId < InMeshBatch.numInstances; ++instanceId )
			{
				vertexShader->SetMesh( InDeviceContextRHI, InMeshBatch, vertexFactory, &InSceneView, 1, instanceId );
				GRHI->CommitConstants( InDeviceContextRHI );
				GRHI->DrawPrimitive( InDeviceContextRHI, InMeshBatch.primitiveType, InMeshBatch.baseVertexIndex, InMeshBatch.numPrimitives );
			}
		}
	}
	// Else we draw geometry with help instancing
	else
	{
		vertexShader->SetMesh( InDeviceContextRHI, InMeshBatch, vertexFactory, &InSceneView, InMeshBatch.numInstances );
		GRHI->CommitConstants( InDeviceContextRHI );

		if ( InMeshBatch.indexBufferRHI )
		{
			GRHI->DrawIndexedPrimitive( InDeviceContextRHI, InMeshBatch.indexBufferRHI, InMeshBatch.primitiveType, InMeshBatch.baseVertexIndex, InMeshBatch.firstIndex, InMeshBatch.numPrimitives, InMeshBatch.numInstances );
		}
		else
		{
			GRHI->DrawPrimitive( InDeviceContextRHI, InMeshBatch.primitiveType, InMeshBatch.baseVertexIndex, InMeshBatch.numPrimitives, InMeshBatch.numInstances );
		}
	}
}

uint64 FMeshDrawingPolicy::GetTypeHash() const
{
	return hash;
}

FBoundShaderStateRHIRef FMeshDrawingPolicy::GetBoundShaderState() const
{
	if ( !boundShaderState )
	{
		TSharedPtr<FMaterial>		materialRef = material.Pin();
		check( materialRef && vertexFactory && vertexShader && pixelShader );
		
		boundShaderState = GRHI->CreateBoundShaderState(
			materialRef->GetAssetName().c_str(),
			vertexFactory->GetDeclaration(),
			vertexShader->GetVertexShader(),
			pixelShader->GetPixelShader() );
	}

	return boundShaderState;
}

FRasterizerStateRHIRef FMeshDrawingPolicy::GetRasterizerState() const
{
	TSharedPtr<FMaterial>		materialRef = material.Pin();
	const FRasterizerStateInitializerRHI		initializer =
	{
		materialRef->IsWireframe() ? FM_Wireframe : FM_Solid,
		materialRef->IsTwoSided() ? CM_None : CM_CW,
		depthBias,
		0.f,
		true
	};

	if ( !rasterizerState || rasterizerState->GetInitializer() != initializer )
	{
		rasterizerState = GRHI->CreateRasterizerState( initializer );
	}

	return rasterizerState;
}

bool FMeshDrawingPolicy::Matches( const FMeshDrawingPolicy& InOtherDrawer ) const
{
	return
		material == InOtherDrawer.material &&
		vertexFactory == InOtherDrawer.vertexFactory &&
		vertexShader == InOtherDrawer.vertexShader &&
		pixelShader == InOtherDrawer.pixelShader;
}

bool FMeshDrawingPolicy::IsValid() const
{
	return material && vertexFactory && vertexShader && pixelShader;
}