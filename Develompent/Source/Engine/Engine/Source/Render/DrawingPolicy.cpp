#include "Misc/EngineGlobals.h"
#include "Containers/String.h"
#include "RHI/BaseRHI.h"
#include "RHI/BaseStateRHI.h"
#include "RHI/TypesRHI.h"
#include "Render/VertexFactory/VertexFactory.h"
#include "Render/DrawingPolicy.h"

FMeshDrawingPolicy::FMeshDrawingPolicy()
	: bInit( false )
	, depthBias( 0.f )
	, hash( INVALID_HASH )
{}

FMeshDrawingPolicy::FMeshDrawingPolicy( class FVertexFactory* InVertexFactory, class FMaterial* InMaterial, float InDepthBias /* = 0.f */ ) 
	: bInit( false )
	, material( InMaterial )
	, vertexFactory( InVertexFactory )
	, depthBias( InDepthBias )
	, hash( INVALID_HASH )
{
	InitInternal( InVertexFactory, InMaterial, InDepthBias );
}

FMeshDrawingPolicy::~FMeshDrawingPolicy()
{}

void FMeshDrawingPolicy::InitInternal( class FVertexFactory* InVertexFactory, class FMaterial* InMaterial, float InDepthBias /* = 0.f */ )
{
	checkMsg( InVertexFactory && InMaterial, TEXT( "Vertex factory and material must be valid for init drawing policy" ) );

	uint64			vertexFactoryHash = InVertexFactory->GetType()->GetHash();
	vertexShader	= InMaterial->GetShader( vertexFactoryHash, SF_Vertex );
	pixelShader		= InMaterial->GetShader( vertexFactoryHash, SF_Pixel );

	hash			= appMemFastHash( material, InVertexFactory->GetTypeHash() );
	vertexFactory	= InVertexFactory;
	material		= InMaterial;
	depthBias		= InDepthBias;
	bInit			= true;
}

void FMeshDrawingPolicy::SetRenderState( class FBaseDeviceContextRHI* InDeviceContextRHI )
{
	check( bInit );
	const FRasterizerStateInitializerRHI		initializer =
	{
		material->IsWireframe() ? FM_Wireframe : FM_Solid,
		material->IsTwoSided() ? CM_None : CM_CW,
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
	check( bInit );
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

bool FMeshDrawingPolicy::IsValid() const
{
	return material && vertexFactory && vertexShader && pixelShader;
}