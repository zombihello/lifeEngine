#include "Misc/EngineGlobals.h"
#include "Containers/String.h"
#include "RHI/BaseRHI.h"
#include "RHI/BaseStateRHI.h"
#include "RHI/TypesRHI.h"
#include "Render/VertexFactory/VertexFactory.h"
#include "Render/DrawingPolicy.h"

FMeshDrawingPolicy::FMeshDrawingPolicy( class FVertexFactory* InVertexFactory, class FMaterial* InMaterial, float InDepthBias /* = 0.f */ ) :
	material( InMaterial ),
	vertexFactory( InVertexFactory ),
	depthBias( InDepthBias )
{
	check( InVertexFactory && InMaterial );

	uint32			vertexFactoryHash = vertexFactory->GetType()->GetHash();
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
		material->IsTwoSided() ? CM_None : CM_CW,
		depthBias,
		0.f,
		true
	};
	
	vertexFactory->Set( InDeviceContextRHI );
	GRHI->SetRasterizerState( InDeviceContextRHI, GRHI->CreateRasterizerState( initializer ) );
	GRHI->SetBoundShaderState( InDeviceContextRHI, GRHI->CreateBoundShaderState( 
		FString::Format( TEXT( "%s" ), material->GetAssetName().c_str() ).c_str(),
		vertexFactory->GetDeclaration(), 
		vertexShader->GetVertexShader(), 
		pixelShader->GetPixelShader() ) );
}

void FMeshDrawingPolicy::SetShaderParameters( class FBaseDeviceContextRHI* InDeviceContextRHI )
{
	check( vertexFactory );
	vertexFactory->SetShaderParameters( InDeviceContextRHI );
}