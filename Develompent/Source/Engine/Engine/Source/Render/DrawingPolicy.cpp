#include "Misc/EngineGlobals.h"
#include "Containers/String.h"
#include "RHI/BaseRHI.h"
#include "RHI/BaseStateRHI.h"
#include "RHI/TypesRHI.h"
#include "Render/VertexFactory/VertexFactory.h"
#include "Render/DrawingPolicy.h"

FMeshDrawingPolicy::FMeshDrawingPolicy( class FVertexFactory* InVertexFactory, class FMaterial* InMaterial, float InDepthBias /* = 0.f */ ) 
	: material( InMaterial )
	, vertexFactory( InVertexFactory )
	, depthBias( InDepthBias )
	, hash( appMemFastHash( material, vertexFactory ? vertexFactory->GetTypeHash() : 0 ) )
{
	check( InVertexFactory && InMaterial );

	uint64			vertexFactoryHash = vertexFactory->GetType()->GetHash();
	vertexShader	= material->GetShader( vertexFactoryHash, SF_Vertex );
	pixelShader		= material->GetShader( vertexFactoryHash, SF_Pixel );
	check( vertexShader && pixelShader );
}

FMeshDrawingPolicy::~FMeshDrawingPolicy()
{}

void FMeshDrawingPolicy::SetRenderState( class FBaseDeviceContextRHI* InDeviceContextRHI )
{
	check( material && vertexFactory );
	const FRasterizerStateInitializerRHI		initializer =
	{
		material->IsWireframe() ? FM_Wireframe : FM_Solid,
		CM_None,
		depthBias,
		0.f,
		true
	};
	
	vertexFactory->Set( InDeviceContextRHI );
	GRHI->SetRasterizerState( InDeviceContextRHI, GRHI->CreateRasterizerState( initializer ) );
	GRHI->SetBoundShaderState( InDeviceContextRHI, GetBoundShaderState() );
}

void FMeshDrawingPolicy::SetShaderParameters( class FBaseDeviceContextRHI* InDeviceContextRHI )
{
	check( vertexFactory );
	vertexShader->SetConstantParameters( InDeviceContextRHI, vertexFactory, material );
	pixelShader->SetConstantParameters( InDeviceContextRHI, vertexFactory, material );
}

uint64 FMeshDrawingPolicy::GetTypeHash() const
{
	return hash;
}

FBoundShaderStateRHIRef FMeshDrawingPolicy::GetBoundShaderState() const
{
	if ( !boundShaderState )
	{
		check( material && vertexFactory && vertexShader && pixelShader );
		boundShaderState = GRHI->CreateBoundShaderState(
			material->GetAssetName().c_str(),
			vertexFactory->GetDeclaration(),
			vertexShader->GetVertexShader(),
			pixelShader->GetPixelShader() );
	}

	return boundShaderState;
}

bool FMeshDrawingPolicy::Matches( const FMeshDrawingPolicy& InOtherDrawer ) const
{
	return
		material == InOtherDrawer.material &&
		vertexFactory == InOtherDrawer.vertexFactory &&
		vertexShader == InOtherDrawer.vertexShader &&
		pixelShader == InOtherDrawer.pixelShader;
}