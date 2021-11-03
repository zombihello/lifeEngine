#include "Render/SceneRendering.h"
#include "Misc/EngineGlobals.h"
#include "RHI/BaseRHI.h"
#include "RHI/TypesRHI.h"
#include "Render/Texture.h"

FTestDrawPolicy::FTestDrawPolicy( class FMaterial* InMaterial, float InDepthBias /* = 0.f */ ) :
	FMeshDrawingPolicy( InMaterial, InDepthBias )
{
	pixelShader = InMaterial->GetShader( SF_Pixel );
}

void FTestDrawPolicy::SetShaderParameters( class FBaseDeviceContextRHI* InDeviceContextRHI )
{
	check( pixelShader );

	FTexture2DRef		texture2d;
	if ( material->GetTextureParameterValue( TEXT( "diffuse" ), texture2d ) )
	{
		GRHI->SetTextureParameter( InDeviceContextRHI, pixelShader->GetPixelShader(), texture2d->GetTexture2DRHI(), 0 );
		GRHI->SetSamplerState( InDeviceContextRHI, pixelShader->GetPixelShader(), GRHI->CreateSamplerState( texture2d->GetSamplerStateInitialiser() ), 0 );
	}
}

void FTestDrawPolicy::Draw( class FBaseDeviceContextRHI* InDeviceContextRHI, const class FSceneView& InSceneView )
{
	GRHI->DrawPrimitive( InDeviceContextRHI, PT_TriangleList, 0, 1 );
}