#include "Core.h"
#include "Misc/CoreGlobals.h"
#include "Misc/EngineGlobals.h"
#include "Misc/Misc.h"
#include "Logger/LoggerMacros.h"
#include "Logger/BaseLogger.h"
#include "System/BaseArchive.h"
#include "System/BaseFileSystem.h"
#include "System/BaseWindow.h"
#include "System/Config.h"
#include "System/ThreadingBase.h"
#include "Math/Color.h"
#include "Scripts/ScriptEngine.h"
#include "RHI/BaseRHI.h"
#include "RHI/BaseViewportRHI.h"
#include "RHI/BaseDeviceContextRHI.h"
#include "RHI/BaseSurfaceRHI.h"
#include "Render/Shaders/ShaderManager.h"
#include "UIEngine.h"
#include "Misc/UIGlobals.h"
#include "EngineLoop.h"

#if WITH_EDITOR
#include "WorldEd.h"
#endif // WITH_EDITOR

// --------------
// GLOBALS
// --------------

FViewportRHIRef			GViewportRHI;

/**
 * Constructor
 */
FEngineLoop::FEngineLoop() :
	isInitialize( false )
#if WITH_EDITOR
	, worldEd( nullptr )
#endif // WITH_EDITOR
{}

/**
 * Destructor
 */
FEngineLoop::~FEngineLoop()
{}

/**
 * Serialize configs
 */
void FEngineLoop::SerializeConfigs()
{
	// Loading engine config
	FBaseArchive*		arConfig = GFileSystem->CreateFileReader( TEXT( "../../Config/Engine.json" ) );
	if ( arConfig )
	{
		GEngineConfig.Serialize( *arConfig );
		delete arConfig;
	}

	// Loading game config
	arConfig = GFileSystem->CreateFileReader( TEXT( "../../Config/Game.json" ) );
	if ( arConfig )
	{
		GGameConfig.Serialize( *arConfig );
		delete arConfig;
	}

	// Loading editor config
#if WITH_EDITOR
	arConfig = GFileSystem->CreateFileReader( TEXT( "../../Config/Editor.json" ) );
	if ( arConfig )
	{
		GEditorConfig.Serialize( *arConfig );
		delete arConfig;
	}
#endif // WITH_EDITOR
}

/**
 * Pre-Initialize the main loop
 */
int32 FEngineLoop::PreInit( const tchar* InCmdLine )
{
	GGameThreadId = appGetCurrentThreadId();
	SerializeConfigs();

	GLog->Init();
	
	int32		result = appPlatformPreInit( InCmdLine );
	GScriptEngine->Init();

	return result;
}

#include "Render/Shaders/TestShader.h"
#include "Render/Texture.h"
FVertexBufferRHIRef		vertexBuffer;
FBoundShaderStateRHIRef				boundShaderState;
FRasterizerStateRHIRef				rasterizerState;
FTexture2D							texture2D;
#include "System/ThreadingBase.h"
#include "Render/RenderingThread.h"

/**
 * Initialize the main loop
 */
int32 FEngineLoop::Init( const tchar* InCmdLine )
{
	LE_LOG( LT_Log, LC_Init, TEXT( "Engine version: %i" ), ENGINE_VERSION );
	LE_LOG( LT_Log, LC_Init, TEXT( "Started with arguments: %s" ), InCmdLine );

	// Create window
	std::wstring				windowTitle = GGameConfig.GetValue( TEXT( "Game.GameInfo" ), TEXT( "Name" ) ).GetString();
	uint32						windowWidth = GEngineConfig.GetValue( TEXT( "Engine.SystemSettings" ), TEXT( "WindowWidth" ) ).GetInt();
	uint32						windowHeight = GEngineConfig.GetValue( TEXT( "Engine.SystemSettings" ), TEXT( "WindowHeight" ) ).GetInt();
	GWindow->Create( windowTitle.c_str(), windowWidth, windowHeight, SW_Default | SW_Hidden );
	
	// Create viewport for render
	uint32			width = 0;
	uint32			height = 0;
	GWindow->GetSize( width, height );

	GRHI->Init( false );
	GViewportRHI = GRHI->CreateViewport( GWindow->GetHandle(), width, height );

	int32		result = appPlatformInit( InCmdLine );
	GShaderManager->Init();
	GUIEngine->Init();

	texture2D.SetFilename( TEXT( "../../Content/Textures/TestTexture2D.jpg" ) );

	struct FInitializeTriangleCommandHelper
	{
		static void Execute()
		{
			FVertexDeclarationElementList		vertexDeclElementList;
			uint32								strideVertex = (3 * sizeof(float)) + (2 * sizeof(float));
			vertexDeclElementList.push_back(FVertexElement(0, strideVertex, 0, VET_Float3, VEU_Position, 0));
			vertexDeclElementList.push_back(FVertexElement(0, strideVertex, 3 * sizeof(float), VET_Float2, VEU_TextureCoordinate, 0));
			FVertexDeclarationRHIRef			vertexDeclRHI = GRHI->CreateVertexDeclaration(vertexDeclElementList);
			
			FShaderRef		testVertexShader = GShaderManager->FindInstance< FTestVertexShader >();
			FShaderRef		testPixelShader = GShaderManager->FindInstance< FTestPixelShader >();
			check(testVertexShader && testPixelShader);
			
			vertexBuffer = GRHI->CreateVertexBuffer(TEXT("TestVertexBuffer"), strideVertex * 3, nullptr, RUF_Dynamic);
			FLockedData				lockedData;
			GRHI->LockVertexBuffer(GRHI->GetImmediateContext(), vertexBuffer, strideVertex * 3, 0, lockedData);
			
			float tempData[] =
			{
				-0.5f, -0.5f, 0.0f,				0.0f, 0.0f,
				 0.5f, -0.5f, 0.0f,				1.0f, 0.0f,
				 0.0f,  0.5f, 0.0f,				0.5f, 1.0f
			};
			memcpy(lockedData.data, &tempData, strideVertex * 3);
			
			GRHI->UnlockVertexBuffer(GRHI->GetImmediateContext(), vertexBuffer, lockedData);
			
			boundShaderState = GRHI->CreateBoundShaderState(TEXT("TestBoundShaderState"), vertexDeclRHI, testVertexShader->GetVertexShader(), testPixelShader->GetPixelShader());
			
			FRasterizerStateInitializerRHI		rasterizerStateInitializerRHI;
			appMemzero(&rasterizerStateInitializerRHI, sizeof(FRasterizerStateInitializerRHI));
			rasterizerStateInitializerRHI.cullMode = CM_CW;
			rasterizerStateInitializerRHI.fillMode = FM_Solid;
			rasterizerState = GRHI->CreateRasterizerState(rasterizerStateInitializerRHI);
		}
	};

	UNIQUE_RENDER_COMMAND( FInitializeTriangleCommand, 
		{
			FInitializeTriangleCommandHelper::Execute();
		} );

#if WITH_EDITOR
	worldEd = new FWorldEd();
	worldEd->Init();
#endif // WITH_EDITOR

	StartRenderingThread();
	GWindow->Show();
	return result;
}

/**
 * Advances main loop
 */
void FEngineLoop::Tick()
{
	// Handling system events
	{
		SWindowEvent			windowEvent;
		while ( GWindow->PollEvent( windowEvent ) )
		{
			switch ( windowEvent.type )
			{
			case SWindowEvent::T_WindowClose:
				if ( windowEvent.events.windowClose.windowId == GWindow->GetID() )
				{
					GIsRequestingExit = true;
				}
				break;

			case SWindowEvent::T_WindowResize:
				if ( windowEvent.events.windowResize.windowId == GWindow->GetID() )
				{
					UNIQUE_RENDER_COMMAND_TWOPARAMETER( FResizeViewportCommand,
														uint32, newWidth, windowEvent.events.windowResize.width,
														uint32, newHeight, windowEvent.events.windowResize.height,
														{
															GViewportRHI->Resize( newWidth, newHeight );
														} );		
				}
				break;
			}

#if WITH_EDITOR
			worldEd->ProcessEvent( windowEvent );
#endif // WITH_EDITOR
		}
	}

	UNIQUE_RENDER_COMMAND( FBeginRenderCommand,
		{
			FBaseDeviceContextRHI*		immediateContext = GRHI->GetImmediateContext();
			GRHI->BeginDrawingViewport( immediateContext, GViewportRHI );
			immediateContext->ClearSurface( GViewportRHI->GetSurface(), FColor::black );
		} );

	UNIQUE_RENDER_COMMAND( FTriangleRenderCommand,
		{
			FBaseDeviceContextRHI*		immediateContext = GRHI->GetImmediateContext();
			GRHI->SetStreamSource( immediateContext, 0, vertexBuffer, (3 * sizeof(float)) + (2 * sizeof(float)), 0 );
			GRHI->SetBoundShaderState( immediateContext, boundShaderState );
			GRHI->SetRasterizerState( immediateContext, rasterizerState );
			GRHI->SetTextureParameter( immediateContext, boundShaderState->GetPixelShader(), texture2D.GetTexture2DRHI(), 0 );
			GRHI->SetSamplerState( immediateContext, boundShaderState->GetPixelShader(), texture2D.GetSamplerStateRHI(), 0 );
			GRHI->DrawPrimitive( immediateContext, PT_TriangleList, 0, 1 );
		} );

#if WITH_EDITOR
	worldEd->Tick();
#endif // WITH_EDITOR

	GUIEngine->BeginDraw();
	GUIEngine->EndDraw();

	UNIQUE_RENDER_COMMAND( FEndRenderCommand,
		{
			FBaseDeviceContextRHI*		immediateContext = GRHI->GetImmediateContext();
			GRHI->EndDrawingViewport( immediateContext, GViewportRHI, true, false );
		} );
}

/**
 * Performs shut down
 */
void FEngineLoop::Exit()
{
	StopRenderingThread();

#if WITH_EDITOR
	delete worldEd;
	worldEd = nullptr;
#endif // WITH_EDITOR

	GUIEngine->Shutdown();

	GViewportRHI.SafeRelease();
	GRHI->Destroy();

	GWindow->Close();
	GLog->TearDown();
}