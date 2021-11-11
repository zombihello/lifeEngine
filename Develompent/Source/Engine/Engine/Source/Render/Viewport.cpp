#include "Misc/EngineGlobals.h"
#include "RHI/BaseRHI.h"
#include "RHI/BaseDeviceContextRHI.h"
#include "RHI/BaseViewportRHI.h"
#include "Render/RenderingThread.h"
#include "Render/Viewport.h"


FViewport::FViewport() :
	windowHandle( nullptr ),
	sizeX( 0 ),
	sizeY( 0 )
{}

FViewport::~FViewport()
{}

// BS yehor.pohuliaka - This code for test, need delete it
#include "Render/Shaders/BasePassShader.h"
#include "Render/Texture.h"
#include "System/Archive.h"
#include "System/BaseFileSystem.h"
#include "Misc/CoreGlobals.h"
#include "Misc/Misc.h"
#include "Components/CameraComponent.h"
#include "Render/Scene.h"
#include "System/InputSystem.h"
#include "Render/Material.h"
#include "Render/SceneRendering.h"
#include "System/Package.h"
#include "Render/StaticMesh.h"

FSceneView								sceneView;
extern LCameraComponent*				cameraComponent;
FStaticMeshRef							staticMesh;
FTexture2DRHIRef						depthBufferTexture2D;
FSurfaceRHIRef							depthBufferSurface;
bool									q = false;

struct FTees
{
	float X;
	FVector z;
};

void FViewport::InitRHI()
{
	// If viewport already created - we need resize
	if ( viewportRHI )
	{
		viewportRHI->Resize( sizeX, sizeY );
	}
	// Else we need create it
	else
	{
		viewportRHI = GRHI->CreateViewport( windowHandle, sizeX, sizeY );
	}

	if ( !q )
	{
		depthBufferTexture2D = GRHI->CreateTexture2D( TEXT( "DepthBuffer" ), sizeX, sizeY, PF_DepthStencil, 1, TCF_ResolveTargetable | TCF_DepthStencil );
		depthBufferSurface = GRHI->CreateTargetableSurface( TEXT( "DepthBuffer" ), sizeX, sizeY, PF_DepthStencil, depthBufferTexture2D, TCF_ResolveTargetable | TCF_DepthStencil );
		staticMesh = GPackageManager->FindAsset( TEXT( "Content/Tiger.lpak" ), appCalcHash( TEXT( "Tiger" ) ) );

		/*FPackage		pak;
		pak.Open( TEXT( "Content/Tiger.lpak" ), true );

		{
			FTexture2DRef		texture2D = GPackageManager->FindAsset( TEXT( "Content/Tiger.lpak" ), appCalcHash( TEXT( "TankBody_C" ) ) );
			FMaterialRef		material = new FMaterial();
			material->SetShader( FBasePassVertexShader::staticType );
			material->SetShader( FBasePassPixelShader::staticType );
			material->UsageOnStaticMesh( true );
			material->SetTextureParameterValue( TEXT( "diffuse" ), texture2D );
			material->SetAssetName( TEXT( "TankBody_Mat" ) );
			material->SetAssetHash( appCalcHash( TEXT( "TankBody_Mat" ) ) );
			staticMesh->SetMaterial( 1, material );
			pak.Add( texture2D );
			pak.Add( material );
		}

		{
			FTexture2DRef		texture2D = GPackageManager->FindAsset( TEXT( "Content/Tiger.lpak" ), appCalcHash( TEXT( "TigerMG_C" ) ) );
			FMaterialRef		material = new FMaterial();
			material->SetShader( FBasePassVertexShader::staticType );
			material->SetShader( FBasePassPixelShader::staticType );
			material->UsageOnStaticMesh( true );
			material->SetTextureParameterValue( TEXT( "diffuse" ), texture2D );
			material->SetAssetName( TEXT( "TigerMG_Mat" ) );
			material->SetAssetHash( appCalcHash( TEXT( "TigerMG_Mat" ) ) );
			staticMesh->SetMaterial( 0, material );
			pak.Add( texture2D );
			pak.Add( material );
		}

		{
			FTexture2DRef		texture2D = GPackageManager->FindAsset( TEXT( "Content/Tiger.lpak" ), appCalcHash( TEXT( "TigerTrack_C" ) ) );
			FMaterialRef		material = new FMaterial();
			material->SetShader( FBasePassVertexShader::staticType );
			material->SetShader( FBasePassPixelShader::staticType );
			material->UsageOnStaticMesh( true );
			material->SetTextureParameterValue( TEXT( "diffuse" ), texture2D );
			material->SetAssetName( TEXT( "TigerTrack_Mat" ) );
			material->SetAssetHash( appCalcHash( TEXT( "TigerTrack_Mat" ) ) );
			staticMesh->SetMaterial( 2, material );
			pak.Add( texture2D );
			pak.Add( material );
		}

		pak.Add( staticMesh );
		pak.Serialize();*/
		
		q = true;
	}
}

void FViewport::ReleaseRHI()
{
	viewportRHI.SafeRelease();
}

void FViewport::Update( bool InIsDestroyed, uint32 InNewSizeX, uint32 InNewSizeY, void* InNewWindowHandle )
{
	bool	isNewWindowHandle = windowHandle != InNewWindowHandle;

	// Update the viewport attributes
	windowHandle = InNewWindowHandle;
	sizeX = InNewSizeX;
	sizeY = InNewSizeY;

#if DO_CHECK
	if ( !InIsDestroyed )
	{
		check( windowHandle );
	}
#endif // DO_CHECK

	// Don't reinitialize the viewport RHI if the viewport has been destroyed
	if ( InIsDestroyed || isNewWindowHandle )
	{	
		BeginReleaseResource( this );

		if ( InIsDestroyed )
		{
			return;
		}
	}

	// Initialize the viewport's resources if viewport RHI not created
	if ( !viewportRHI )
	{
		BeginInitResource( this );
	}
	// Else update resource
	else
	{
		BeginUpdateResource( this );
	}
}

bool a = false;
void FViewport::Draw( bool InIsShouldPresent /* = true */ )
{
	// BS yehor.pohuliaka - Place court render scene
	if ( !IsInitialized() )
	{
		return;
	}

	if ( cameraComponent )
	{
		sceneView.SetCameraView( cameraComponent );
	}

	struct Helper
	{
		static void Execute( FViewportRHIRef viewportRHI, FSceneView sceneView, bool isShouldPresent )
		{
			FBaseDeviceContextRHI* immediateContext = GRHI->GetImmediateContext();
			GRHI->BeginDrawingViewport( immediateContext, viewportRHI );
			GRHI->SetRenderTarget( immediateContext, viewportRHI->GetSurface(), depthBufferSurface );
			immediateContext->ClearSurface( viewportRHI->GetSurface(), FColor::black );
			immediateContext->ClearDepthStencil( depthBufferSurface );
			GRHI->SetViewParameters( immediateContext, sceneView );

			// Render test static mesh
			const std::vector< FStaticMeshSurface >&		staticMeshSurfaces = staticMesh->GetSurfaces();
			const std::vector< FMaterialRef >&				staticMeshMaterials = staticMesh->GetMaterials();

			for ( uint32 indexSurface = 0, numSurfaces = ( uint32 )staticMeshSurfaces.size(); indexSurface < numSurfaces; ++indexSurface )
			{
				const FStaticMeshSurface&		surface = staticMeshSurfaces[ indexSurface ];
				FMeshBatch						meshBatch;
				meshBatch.primitiveType = PT_TriangleList;
				meshBatch.elements.push_back( FMeshBatchElement{ staticMesh->GetIndexBufferRHI(), surface.baseVertexIndex, surface.firstIndex, surface.numPrimitives } );

				FStaticMeshDrawPolicy		drawPolicy( staticMesh->GetVertexFactory(), staticMeshMaterials[ surface.materialID ] );
				drawPolicy.SetRenderState( immediateContext );
				drawPolicy.SetShaderParameters( immediateContext );
				drawPolicy.Draw( immediateContext, meshBatch, sceneView );
			}

			GRHI->EndDrawingViewport( immediateContext, viewportRHI, isShouldPresent, false );
		}
	};

	UNIQUE_RENDER_COMMAND_THREEPARAMETER( FQRenderCommand,
										  FViewportRHIRef, viewportRHI, viewportRHI,
										  FSceneView, sceneView, sceneView,
										  bool, isShouldPresent, InIsShouldPresent,
										  {
											  Helper::Execute( viewportRHI,sceneView,isShouldPresent );
										  } );
}