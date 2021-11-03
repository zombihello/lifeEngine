#include "Misc/EngineGlobals.h"
#include "RHI/BaseRHI.h"
#include "RHI/BaseStateRHI.h"
#include "RHI/TypesRHI.h"
#include "Render/DrawingPolicy.h"

FMeshDrawingPolicy::FMeshDrawingPolicy( class FMaterial* InMaterial, float InDepthBias /* = 0.f */ ) :
	material( InMaterial ),
	depthBias( InDepthBias )
{}

FMeshDrawingPolicy::~FMeshDrawingPolicy()
{}

void FMeshDrawingPolicy::SetRenderState( class FBaseDeviceContextRHI* InDeviceContextRHI, FVertexDeclarationRHIParamRef InVertexDeclaration )
{
	check( material );
	const FRasterizerStateInitializerRHI		initializer =
	{
		material->IsWireframe() ? FM_Wireframe : FM_Solid,
		material->IsTwoSided() ? CM_None : CM_CW,
		depthBias,
		0.f,
		true
	};

	FVertexShaderRHIRef		vertexShader = material->GetShader( SF_Vertex )->GetVertexShader();
	FPixelShaderRHIRef		pixelShader = material->GetShader( SF_Pixel )->GetPixelShader();

	GRHI->SetRasterizerState( InDeviceContextRHI, GRHI->CreateRasterizerState( initializer ) );
	GRHI->SetBoundShaderState( InDeviceContextRHI, GRHI->CreateBoundShaderState( TEXT( "BoundShaderState" ), InVertexDeclaration, vertexShader, pixelShader ) );
}

void FMeshDrawingPolicy::SetShaderParameters( class FBaseDeviceContextRHI* InDeviceContextRHI )
{}