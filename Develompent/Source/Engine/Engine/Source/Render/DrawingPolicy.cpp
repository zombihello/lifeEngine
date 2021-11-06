#include "Misc/EngineGlobals.h"
#include "RHI/BaseRHI.h"
#include "RHI/BaseStateRHI.h"
#include "RHI/TypesRHI.h"
#include "Render/VertexFactory/VertexFactory.h"
#include "Render/DrawingPolicy.h"

FMeshDrawingPolicy::FMeshDrawingPolicy( class FMaterial* InMaterial, class FVertexFactory* InVertexFactory, float InDepthBias /* = 0.f */ ) :
	material( InMaterial ),
	vertexFactory( InVertexFactory ),
	depthBias( InDepthBias )
{}

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

	FShaderRef			materialShader = material->GetShader();
	check( materialShader );

	vertexFactory->Set( InDeviceContextRHI );
	GRHI->SetRasterizerState( InDeviceContextRHI, GRHI->CreateRasterizerState( initializer ) );
	GRHI->SetBoundShaderState( InDeviceContextRHI, GRHI->CreateBoundShaderState( 
		TEXT( "BoundShaderState" ), 
		vertexFactory->GetDeclaration(), 
		vertexFactory->GetShader()->GetVertexShader(), 
		materialShader->GetPixelShader() ) );
}

void FMeshDrawingPolicy::SetShaderParameters( class FBaseDeviceContextRHI* InDeviceContextRHI )
{
	check( vertexFactory );
	vertexFactory->SetShaderParameters( InDeviceContextRHI );
}