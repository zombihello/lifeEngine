#include "Misc/EngineGlobals.h"
#include "Containers/String.h"
#include "RHI/BaseRHI.h"
#include "RHI/BaseStateRHI.h"
#include "RHI/TypesRHI.h"
#include "Render/VertexFactory/VertexFactory.h"
#include "Render/SceneUtils.h"
#include "Render/Scene.h"
#include "Render/DrawingPolicy.h"

CMeshDrawingPolicy::CMeshDrawingPolicy()
	: bInit( false )
	, depthBias( 0.f )
	, hash( INVALID_HASH )
{}

CMeshDrawingPolicy::~CMeshDrawingPolicy()
{}

void CMeshDrawingPolicy::InitInternal( class CVertexFactory* InVertexFactory, const TAssetHandle<CMaterial>& InMaterial, float InDepthBias /* = 0.f */ )
{
	// If material is not valid, we use default material
	TSharedPtr<CMaterial>		materialRef = InMaterial.ToSharedPtr();
	if ( !materialRef )
	{
		materialRef = GEngine->GetDefaultMaterial().ToSharedPtr();
	}
	checkMsg( InVertexFactory && materialRef, TEXT( "Vertex factory and material must be valid for init drawing policy" ) );

	uint64			vertexFactoryHash = InVertexFactory->GetType()->GetHash();
	vertexShader	= materialRef->GetShader( vertexFactoryHash, SF_Vertex );
	pixelShader		= materialRef->GetShader( vertexFactoryHash, SF_Pixel );

	hash			= appMemFastHash( materialRef, InVertexFactory->GetTypeHash() );
	vertexFactory	= InVertexFactory;
	material		= materialRef->GetAssetHandle();
	depthBias		= InDepthBias;
	bInit			= true;
}

void CMeshDrawingPolicy::SetRenderState( class CBaseDeviceContextRHI* InDeviceContextRHI )
{
	check( bInit );

	vertexFactory->Set( InDeviceContextRHI );
	GRHI->SetRasterizerState( InDeviceContextRHI, GetRasterizerState() );
	GRHI->SetBoundShaderState( InDeviceContextRHI, GetBoundShaderState() );
}

void CMeshDrawingPolicy::SetShaderParameters( class CBaseDeviceContextRHI* InDeviceContextRHI )
{
	check( bInit );

	TSharedPtr<CMaterial>		materialRef = material.ToSharedPtr();
	if ( !materialRef )
	{
		return;
	}

	vertexShader->SetConstantParameters( InDeviceContextRHI, vertexFactory, materialRef );
	pixelShader->SetConstantParameters( InDeviceContextRHI, vertexFactory, materialRef );
}

void CMeshDrawingPolicy::Draw( class CBaseDeviceContextRHI* InDeviceContextRHI, const struct SMeshBatch& InMeshBatch, const class CSceneView& InSceneView )
{
	TSharedPtr<CMaterial>		materialRef = material.ToSharedPtr();
	SCOPED_DRAW_EVENT( EventDraw, DEC_MATERIAL, ÑString::Format( TEXT( "Material %s" ), materialRef ? materialRef->GetAssetName().c_str() : TEXT( "Unloaded" ) ).c_str());

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

uint64 CMeshDrawingPolicy::GetTypeHash() const
{
	return hash;
}

BoundShaderStateRHIRef_t CMeshDrawingPolicy::GetBoundShaderState() const
{
	if ( !boundShaderState )
	{
		TSharedPtr<CMaterial>		materialRef = material.ToSharedPtr();
		check( materialRef && vertexFactory && vertexShader && pixelShader );
		
		boundShaderState = GRHI->CreateBoundShaderState(
			materialRef->GetAssetName().c_str(),
			vertexFactory->GetDeclaration(),
			vertexShader->GetVertexShader(),
			pixelShader->GetPixelShader() );
	}

	return boundShaderState;
}

RasterizerStateRHIRef_t CMeshDrawingPolicy::GetRasterizerState() const
{
	TSharedPtr<CMaterial>		materialRef = material.ToSharedPtr();
	const SRasterizerStateInitializerRHI		initializer =
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

bool CMeshDrawingPolicy::Matches( const CMeshDrawingPolicy& InOtherDrawer ) const
{
	return
		material == InOtherDrawer.material &&
		vertexFactory == InOtherDrawer.vertexFactory &&
		vertexShader == InOtherDrawer.vertexShader &&
		pixelShader == InOtherDrawer.pixelShader;
}

bool CMeshDrawingPolicy::IsValid() const
{
	return material.IsAssetValid() && vertexFactory && vertexShader && pixelShader;
}