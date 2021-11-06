#include "Misc/EngineGlobals.h"
#include "RHI/BaseRHI.h"
#include "RHI/BaseDeviceContextRHI.h"
#include "RHI/BaseViewportRHI.h"
#include "Render/RenderingThread.h"
#include "Render/Viewport.h"
#include "Render/VertexFactory/TestVertexFactory.h"


FViewport::FViewport() :
	windowHandle( nullptr ),
	sizeX( 0 ),
	sizeY( 0 )
{}

FViewport::~FViewport()
{}

// BS yehor.pohuliaka - This code for test, need delete it
#include "Render/Shaders/TestShader.h"
#include "Render/Texture.h"
#include "Render/TextureFileCache.h"
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

FSceneView								sceneView;
extern LCameraComponent*				cameraComponent;
FVertexBufferRHIRef						vertexBuffer;
FMaterialRef							material;
TRefCountPtr< FTestVertexFactory >		vertexFactory;
bool									q = false;

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
		vertexBuffer = GRHI->CreateVertexBuffer( TEXT( "TestVertexBuffer" ), sizeof( FTestVertexType ) * 3, nullptr, RUF_Dynamic );
		FLockedData				lockedData;
		GRHI->LockVertexBuffer( GRHI->GetImmediateContext(), vertexBuffer, sizeof( FTestVertexType ) * 3, 0, lockedData );

		FTestVertexType tempData[] =
		{
			 FTestVertexType{ FVector( -50.f, -50.f, 0.0f ),	FVector2D( 0.0f, 0.0f ) },
			 FTestVertexType{ FVector( 50.f, -50.f, 0.0f ),		FVector2D( 1.0f, 0.0f ) },
			 FTestVertexType{ FVector( 0.f, 50.f, 0.0f ),		FVector2D( 0.5f, -1.0f ) }
		};

		memcpy( lockedData.data, &tempData, sizeof( FTestVertexType ) * 3 );
		GRHI->UnlockVertexBuffer( GRHI->GetImmediateContext(), vertexBuffer, lockedData );
		
		vertexFactory = new FTestVertexFactory();
		vertexFactory->AddVertexStream( FVertexStream{ vertexBuffer, sizeof( FTestVertexType ) } );		// 0 stream slot
		vertexFactory->Init();

		/*FTexture2DRef texture2D = new FTexture2D();
		FArchive*	ar = GFileSystem->CreateFileReader( appBaseDir() + TEXT( "/Engine/Content/EngineTextures.tfc" ) );
		if ( ar )
		{
			ar->SerializeHeader();

			FTextureFileCache		textureFileCache;
			textureFileCache.Serialize( *ar );

			FTextureCacheItem		textureCacheItem;
			if ( textureFileCache.Find( appCalcHash( TEXT( "DefaultDiffuse" ) ), &textureCacheItem ) )
			{
				texture2D->SetHash( appCalcHash( TEXT( "T_Test" ) ) );
				texture2D->SetTextureCache( textureCacheItem, TEXT( "/Engine/Content/EngineTextures.tfc" ) );
			}

			delete ar;
		}
		material = new FMaterial();
		material->SetShader( FTestPixelShader::staticType );
		material->SetTextureParameterValue( TEXT( "diffuse" ), texture2D );
		material->SetHash( appCalcHash( TEXT( "DefaultMat" ) ) );

		FPackage	pak;
		pak.Open( TEXT( "Content/PackageTest.lpak" ), true );
		pak.Add( material );
		pak.Add( texture2D );
		pak.Serialize();*/

		material = GPackageManager->FindAsset( TEXT( "Content/PackageTest.lpak" ), appCalcHash( TEXT( "DefaultMat" ) ) );
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
			immediateContext->ClearSurface( viewportRHI->GetSurface(), FColor::black );
			GRHI->SetViewParameters( immediateContext, sceneView );

			FTestDrawPolicy		drawPolicy( material, vertexFactory );
			drawPolicy.SetRenderState( immediateContext );
			drawPolicy.SetShaderParameters( immediateContext );
			drawPolicy.Draw( immediateContext, sceneView );

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