#include "Containers/String.h"
#include "Render/SceneRendering.h"
#include "Render/Scene.h"
#include "Misc/EngineGlobals.h"
#include "System/World.h"
#include "RHI/BaseRHI.h"
#include "RHI/BaseBufferRHI.h"
#include "RHI/BaseSurfaceRHI.h"
#include "RHI/BaseDeviceContextRHI.h"
#include "RHI/TypesRHI.h"
#include "Render/RenderingThread.h"
#include "Render/SceneUtils.h"
#include "Render/Scene.h"
#include "Render/Texture.h"
#include "Render/SceneRenderTargets.h"

#if WITH_EDITOR
#include "Render/WorldGrid.h"
#include "Misc/WorldEdGlobals.h"
#include "System/EditorEngine.h"
#include "Render/Shaders/WorldGridShader.h"
#include "Render/VertexFactory/WorldGridVertexFactory.h"
#endif // WITH_EDITOR

FStaticMeshDrawPolicy::FStaticMeshDrawPolicy()
{}

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
	SCOPED_DRAW_EVENT( EventDraw, DEC_MATERIAL, FString::Format( TEXT( "Material %s" ), material->GetAssetName().c_str() ).c_str() );
	check( InMeshBatch.indexBufferRHI );

	// If vertex factory not support instancig - draw without it
	if ( !vertexFactory->SupportsInstancing() )
	{
		for ( uint32 instanceId = 0; instanceId < InMeshBatch.numInstances; ++instanceId )
		{
			vertexShader->SetMesh( InDeviceContextRHI, InMeshBatch, vertexFactory, &InSceneView, 1, instanceId );
			GRHI->CommitConstants( InDeviceContextRHI );
			GRHI->DrawIndexedPrimitive( InDeviceContextRHI, InMeshBatch.indexBufferRHI, InMeshBatch.primitiveType, InMeshBatch.baseVertexIndex, InMeshBatch.firstIndex, InMeshBatch.numPrimitives );
		}
	}
	// Else we draw geometry with help instancing
	else
	{
		vertexShader->SetMesh( InDeviceContextRHI, InMeshBatch, vertexFactory, &InSceneView, InMeshBatch.numInstances );
		GRHI->CommitConstants( InDeviceContextRHI );
		GRHI->DrawIndexedPrimitive( InDeviceContextRHI, InMeshBatch.indexBufferRHI, InMeshBatch.primitiveType, InMeshBatch.baseVertexIndex, InMeshBatch.firstIndex, InMeshBatch.numPrimitives, InMeshBatch.numInstances );
	}
}


FORCEINLINE const tchar* GetSceneSDGName( ESceneDepthGroup SDG )
{
	switch ( SDG )
	{
	case SDG_World:		return TEXT( "World" );
	default:			return TEXT( "Unknown" );
	}
}

FSceneRenderer::FSceneRenderer( FSceneView* InSceneView )
	: sceneView( InSceneView )
{}

void FSceneRenderer::Render( FViewportRHIParamRef InViewportRHI )
{
	FBaseDeviceContextRHI*		immediateContext = GRHI->GetImmediateContext();
	GRHI->SetRenderTarget( immediateContext, InViewportRHI->GetSurface(), GSceneRenderTargets.GetSceneDepthZSurface() );
	immediateContext->ClearSurface( InViewportRHI->GetSurface(), sceneView->GetBackgroundColor() );
	immediateContext->ClearDepthStencil( GSceneRenderTargets.GetSceneDepthZSurface() );
	GRHI->SetViewParameters( immediateContext, *sceneView );

	EShowFlags		showFlags	= sceneView->GetShowFlags();
	FScene*			scene		= ( FScene* )GWorld->GetScene();
    scene->BuildSDGs( *sceneView );

	// Render scene layers
	{
		SCOPED_DRAW_EVENT( EventSDGs, DEC_SCENE_ITEMS, TEXT( "SDGs" ) );
		for ( uint32 SDGIndex = 0; SDGIndex < SDG_Max; ++SDGIndex )
		{
			SCOPED_DRAW_EVENT( EventSDG, DEC_SCENE_ITEMS, FString::Format( TEXT( "SDG %s" ), GetSceneSDGName( ( ESceneDepthGroup ) SDGIndex ) ).c_str() );
			FSceneDepthGroup&		SDG = scene->GetSDG( ( ESceneDepthGroup ) SDGIndex );

			// Draw static meshes
			if ( showFlags & SHOW_StaticMesh && SDG.staticMeshDrawList.GetNum() > 0 )
			{
				SCOPED_DRAW_EVENT( EventStaticMeshes, DEC_STATIC_MESH, TEXT( "Static meshes" ) );
				SDG.staticMeshDrawList.Draw( immediateContext, *sceneView );
			}

			// Draw sprites
			if ( showFlags & SHOW_Sprite && SDG.spriteDrawList.GetNum() > 0 )
			{
				SCOPED_DRAW_EVENT( EventSprites, DEC_SPRITE, TEXT( "Sprites" ) );
				SDG.spriteDrawList.Draw( immediateContext, *sceneView );
			}
		}
	}
}
