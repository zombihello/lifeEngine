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
#include "System/BaseArchive.h"
#include "System/BaseFileSystem.h"
#include "Misc/CoreGlobals.h"
#include "Misc/Misc.h"
#include "Components/CameraComponent.h"
#include "Render/Scene.h"

FSceneView			sceneView;
LCameraComponent	cameraComponent;

struct FPSConstantBuffer
{
	float		r;
	float		g;
	float		b;
	float		a;
} GPSConstantBuffer;

FVertexBufferRHIRef		vertexBuffer;
FBoundShaderStateRHIRef				boundShaderState;
FRasterizerStateRHIRef				rasterizerState;
FSamplerStateRHIRef					samplerState;
FTexture2D							texture2D;
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
		FVertexDeclarationRHIRef			vertexDeclRHI = GRHI->CreateVertexDeclaration( vertexDeclElementList );

		FShaderRef		testVertexShader = GShaderManager->FindInstance< FTestVertexShader >();
		FShaderRef		testPixelShader = GShaderManager->FindInstance< FTestPixelShader >();
		check( testVertexShader && testPixelShader );

		vertexBuffer = GRHI->CreateVertexBuffer( TEXT( "TestVertexBuffer" ), strideVertex * 3, nullptr, RUF_Dynamic );
		FLockedData				lockedData;
		GRHI->LockVertexBuffer( GRHI->GetImmediateContext(), vertexBuffer, strideVertex * 3, 0, lockedData );

		float tempData[] =
		{
			-50.f, -50.f, 0.0f,				0.0f, 0.0f,
			 50.f, -50.f, 0.0f,				1.0f, 0.0f,
			 0.0f,  50.f, 0.0f,				0.5f, 1.0f
		};
		memcpy( lockedData.data, &tempData, strideVertex * 3 );

		GRHI->UnlockVertexBuffer( GRHI->GetImmediateContext(), vertexBuffer, lockedData );

		boundShaderState = GRHI->CreateBoundShaderState( TEXT( "TestBoundShaderState" ), vertexDeclRHI, testVertexShader->GetVertexShader(), testPixelShader->GetPixelShader() );

		FRasterizerStateInitializerRHI		rasterizerStateInitializerRHI;
		appMemzero( &rasterizerStateInitializerRHI, sizeof( FRasterizerStateInitializerRHI ) );
		rasterizerStateInitializerRHI.cullMode = CM_CW;
		rasterizerStateInitializerRHI.fillMode = FM_Solid;
		rasterizerState = GRHI->CreateRasterizerState( rasterizerStateInitializerRHI );

		FSamplerStateInitializerRHI			samplerStateInitializerRHI;
		appMemzero( &samplerStateInitializerRHI, sizeof( FSamplerStateInitializerRHI ) );
		samplerStateInitializerRHI.filter = SF_Bilinear;
		samplerStateInitializerRHI.addressU = SAM_Wrap;
		samplerStateInitializerRHI.addressV = SAM_Wrap;
		samplerState = GRHI->CreateSamplerState( samplerStateInitializerRHI );

		FBaseArchive*	ar = GFileSystem->CreateFileReader( appBaseDir() + TEXT( "/Engine/Content/EngineTextures.tfc" ) );
		if ( ar )
		{
			FTextureFileCache		textureFileCache;
			textureFileCache.Serialize( *ar );
			
			FTextureCacheItem		textureCacheItem;
			if ( textureFileCache.Find( FTextureCacheItem::CalcHash( TEXT( "DefaultTexture" ) ), &textureCacheItem ) )
			{
				texture2D.SetData( textureCacheItem );
			}

			delete ar;
		}

		appMemzero( &GPSConstantBuffer, sizeof( FPSConstantBuffer ) );
		GPSConstantBuffer.r = 1;
		GPSConstantBuffer.a = 1;

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

	cameraComponent.SetAspectRatio( ( float )InNewSizeX / InNewSizeY );
	cameraComponent.SetProjectionMode( CPM_Perspective );
	cameraComponent.MoveComponent( FVector( 0.f, 0.f, 50.f ) );

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

	sceneView.SetCameraView( &cameraComponent );

	UNIQUE_RENDER_COMMAND_TWOPARAMETER( FBeginRenderCommand,
										FViewportRHIRef, viewportRHI, viewportRHI,
										FSceneView, sceneView, sceneView,
										{
											FBaseDeviceContextRHI* immediateContext = GRHI->GetImmediateContext();
											GRHI->BeginDrawingViewport( immediateContext, viewportRHI );
											immediateContext->ClearSurface( viewportRHI->GetSurface(), FColor::black );
											GRHI->SetViewParameters( immediateContext, sceneView );
										} );
	
	UNIQUE_RENDER_COMMAND( FTriangleRenderCommand,
						   {
							   FBaseDeviceContextRHI * immediateContext = GRHI->GetImmediateContext();
								GRHI->SetShaderParameter( immediateContext, boundShaderState->GetPixelShader(), 0, 0, sizeof( float ) * 4, &GPSConstantBuffer );
								GPSConstantBuffer.r += a ? -0.0005f : 0.0005f;
								if ( GPSConstantBuffer.r >= 1 || GPSConstantBuffer.r <= 0 )
								{
									a = !a;
								}
							   GRHI->SetStreamSource( immediateContext, 0, vertexBuffer, ( 3 * sizeof( float ) ) + ( 2 * sizeof( float ) ), 0 );
							   GRHI->SetBoundShaderState( immediateContext, boundShaderState );
							   GRHI->SetRasterizerState( immediateContext, rasterizerState );
							   GRHI->SetTextureParameter( immediateContext, boundShaderState->GetPixelShader(), texture2D.GetTexture2DRHI(), 0 );
							   GRHI->SetSamplerState( immediateContext, boundShaderState->GetPixelShader(), samplerState, 0 );
							   GRHI->DrawPrimitive( immediateContext, PT_TriangleList, 0, 1 );
						   } );

	UNIQUE_RENDER_COMMAND_TWOPARAMETER( FEndRenderCommand,
										bool, isShouldPresent, InIsShouldPresent,
										FViewportRHIRef, viewportRHI, viewportRHI,
										{
											FBaseDeviceContextRHI* immediateContext = GRHI->GetImmediateContext();
											GRHI->EndDrawingViewport( immediateContext, viewportRHI, isShouldPresent, false );
										} );
}