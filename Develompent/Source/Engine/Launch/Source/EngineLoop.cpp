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
#include "Render/RenderingThread.h"
#include "System/SplashScreen.h"

// --------------
// GLOBALS
// --------------

FViewportRHIRef			GViewportRHI;

/**
 * Constructor
 */
FEngineLoop::FEngineLoop() :
	isInitialize( false )
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

	GGameName = GGameConfig.GetValue( TEXT( "Game.GameInfo" ), TEXT( "Name" ) ).GetString();

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

#if WITH_EDITOR
	GIsEditor = appParseParam( InCmdLine, TEXT( "editor" ) );
#endif // WITH_EDITOR

	GLog->Init();

	int32		result = appPlatformPreInit( InCmdLine );
	GScriptEngine->Init();
	GRHI->Init( GIsEditor );

	LE_LOG( LT_Log, LC_Init, TEXT( "Started with arguments: %s" ), InCmdLine );
	return result;
}

/**
 * Initialize the main loop
 */
int32 FEngineLoop::Init()
{
	LE_LOG( LT_Log, LC_Init, TEXT( "Engine version: " ENGINE_VERSION_STRING ) );

	appSetSplashText( STT_StartupProgress, TEXT( "Init platform..." ) );
	int32		result = appPlatformInit();
	
	appSetSplashText( STT_StartupProgress, TEXT( "Init render system..." ) );
	GShaderManager->Init();
//	GUIEngine->Init();

	// If started game - create window
	if ( !GIsEditor )
	{
		uint32						windowWidth = GEngineConfig.GetValue( TEXT( "Engine.SystemSettings" ), TEXT( "WindowWidth" ) ).GetInt();
		uint32						windowHeight = GEngineConfig.GetValue( TEXT( "Engine.SystemSettings" ), TEXT( "WindowHeight" ) ).GetInt();
		
		GWindow->Create( GGameName.c_str(), windowWidth, windowHeight, SW_Default );
		GViewportRHI = GRHI->CreateViewport( GWindow->GetHandle(), windowWidth, windowHeight );
	}

	StartRenderingThread();
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
		}
	}

	UNIQUE_RENDER_COMMAND( FBeginRenderCommand,
		{
			FBaseDeviceContextRHI*		immediateContext = GRHI->GetImmediateContext();
			GRHI->BeginDrawingViewport( immediateContext, GViewportRHI );
			immediateContext->ClearSurface( GViewportRHI->GetSurface(), FColor::black );
		} );

	//GUIEngine->BeginDraw();
	//GUIEngine->EndDraw();

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

	//GUIEngine->Shutdown();

	GViewportRHI.SafeRelease();
	GShaderManager->Shutdown();
	GRHI->Destroy();

	GWindow->Close();
	GLog->TearDown();
}