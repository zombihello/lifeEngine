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

FSceneView			sceneView;
extern LCameraComponent* cameraComponent;

FVertexBufferRHIRef		vertexBuffer;
FTexture2DRef						texture2D = new FTexture2D();
FMaterialRef						material = new FMaterial();
FVertexDeclarationRHIRef			vertexDeclRHI;
bool			q = false;

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
		FVertexDeclarationElementList		vertexDeclElementList;
		uint32								strideVertex = ( 3 * sizeof( float ) ) + ( 2 * sizeof( float ) );
		vertexDeclElementList.push_back( FVertexElement( 0, strideVertex, 0, VET_Float3, VEU_Position, 0 ) );
		vertexDeclElementList.push_back( FVertexElement( 0, strideVertex, 3 * sizeof( float ), VET_Float2, VEU_TextureCoordinate, 0 ) );
		vertexDeclRHI = GRHI->CreateVertexDeclaration( vertexDeclElementList );

		vertexBuffer = GRHI->CreateVertexBuffer( TEXT( "TestVertexBuffer" ), strideVertex * 3, nullptr, RUF_Dynamic );
		FLockedData				lockedData;
		GRHI->LockVertexBuffer( GRHI->GetImmediateContext(), vertexBuffer, strideVertex * 3, 0, lockedData );

		float tempData[] =
		{
			-50.f, -50.f, 0.0f,				0.0f, 0.0f,
			 50.f, -50.f, 0.0f,				1.0f, 0.0f,
			 0.0f,  50.f, 0.0f,				0.5f, -1.0f
		};
		memcpy( lockedData.data, &tempData, strideVertex * 3 );
		GRHI->UnlockVertexBuffer( GRHI->GetImmediateContext(), vertexBuffer, lockedData );

		FArchive*	ar = GFileSystem->CreateFileReader( appBaseDir() + TEXT( "/Engine/Content/EngineTextures.lpak" ) );
		if ( ar )
		{
			ar->SerializePackageHeader();

			FTextureFileCache		textureFileCache;
			textureFileCache.Serialize( *ar );
			
			FTextureCacheItem		textureCacheItem;
			if ( textureFileCache.Find( FTextureCacheItem::CalcHash( TEXT( "DefaultDiffuse" ) ), &textureCacheItem ) )
			{
				texture2D->SetData( textureCacheItem );
			}

			delete ar;
		}

		material->SetShader( FTestVertexShader::staticType, SF_Vertex );
		material->SetShader( FTestPixelShader::staticType, SF_Pixel );
		material->SetTextureParameterValue( TEXT( "diffuse" ), texture2D );

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
	if ( !isInitialized )
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

			GRHI->SetStreamSource( immediateContext, 0, vertexBuffer, ( 3 * sizeof( float ) ) + ( 2 * sizeof( float ) ), 0 );
			
			FTestDrawPolicy		drawPolicy( material );
			drawPolicy.SetRenderState( immediateContext, vertexDeclRHI );
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