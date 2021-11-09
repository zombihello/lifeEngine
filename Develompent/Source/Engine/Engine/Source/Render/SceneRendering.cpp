#include "Render/SceneRendering.h"
#include "Render/Scene.h"
#include "Misc/EngineGlobals.h"
#include "RHI/BaseRHI.h"
#include "RHI/BaseBufferRHI.h"
#include "RHI/TypesRHI.h"
#include "Render/Texture.h"

FStaticMeshDrawPolicy::FStaticMeshDrawPolicy( class FVertexFactory* InVertexFactory, class FMaterial* InMaterial, float InDepthBias /* = 0.f */ ) :
	FMeshDrawingPolicy( InVertexFactory, InMaterial, InDepthBias )
{}

void FStaticMeshDrawPolicy::SetShaderParameters( class FBaseDeviceContextRHI* InDeviceContextRHI )
{
	FMeshDrawingPolicy::SetShaderParameters( InDeviceContextRHI );

	FTexture2DRef		texture2d;
	if ( material->GetTextureParameterValue( TEXT( "diffuse" ), texture2d ) )
	{
		GRHI->SetTextureParameter( InDeviceContextRHI, pixelShader->GetPixelShader(), texture2d->GetTexture2DRHI(), 0 );
		GRHI->SetSamplerState( InDeviceContextRHI, pixelShader->GetPixelShader(), GRHI->CreateSamplerState( texture2d->GetSamplerStateInitialiser() ), 0 );
	}
}

void FStaticMeshDrawPolicy::Draw( class FBaseDeviceContextRHI* InDeviceContextRHI, const struct FMeshBatch& InMeshBatch, const class FSceneView& InSceneView )
{
	for ( uint32 indexBatch = 0, numBatches = ( uint32 )InMeshBatch.elements.size(); indexBatch < numBatches; ++indexBatch )
	{
		const FMeshBatchElement&		batchElement = InMeshBatch.elements[ indexBatch ];
		check( batchElement.indexBufferRHI );

		GRHI->DrawIndexedPrimitive( InDeviceContextRHI, batchElement.indexBufferRHI, InMeshBatch.primitiveType, batchElement.baseVertexIndex, batchElement.firstIndex, batchElement.numPrimitives );
	}
}