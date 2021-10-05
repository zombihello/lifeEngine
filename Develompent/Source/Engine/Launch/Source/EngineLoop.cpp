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
#include "Misc/Class.h"
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
#include "System/BaseEngine.h"

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

	// Creating engine from config
	{
		std::wstring		classEngineName = TEXT( "LBaseEngine" );
		if ( !GIsEditor )
		{
			classEngineName = GEngineConfig.GetValue( TEXT( "Engine.Engine" ), TEXT( "Class" ) ).GetString().c_str();
		}
#if WITH_EDITOR
		else
		{
			classEngineName = GEditorConfig.GetValue( TEXT( "Editor.Editor" ), TEXT( "Class" ) ).GetString().c_str();
		}
#endif // WITH_EDITOR

		const LClass*		lclass = LClass::StaticFindClass( classEngineName.c_str() );
		checkMsg( lclass, TEXT( "Class engine %s not found" ), classEngineName.c_str() );
		
		GEngine = lclass->CreateObject< LBaseEngine >();
		check( GEngine );
	}

	LE_LOG( LT_Log, LC_Init, TEXT( "Started with arguments: %s" ), InCmdLine );
	return result;
}

/**
 * Initialize the main loop
 */
int32 FEngineLoop::Init()
{
	LE_LOG( LT_Log, LC_Init, TEXT( "Engine version: " ENGINE_VERSION_STRING ) );

	appSetSplashText( STT_StartupProgress, TEXT( "Init platform" ) );
	int32		result = appPlatformInit();
	
	appSetSplashText( STT_StartupProgress, TEXT( "Init render system" ) );
	GShaderManager->Init();
	//GUIEngine->Init();

	appSetSplashText( STT_StartupProgress, TEXT( "Init engine" ) );
	GEngine->Init();

	StartRenderingThread();
	return result;
}

/**
 * Process event
 */
void FEngineLoop::ProcessEvent( struct SWindowEvent& InWindowEvent )
{
	// Handling system events
	GEngine->ProcessEvent( InWindowEvent );
}

/**
 * Advances main loop
 */
void FEngineLoop::Tick()
{
	// Update engine
	GEngine->Tick( 0.f );
}

/**
 * Performs shut down
 */
void FEngineLoop::Exit()
{
	StopRenderingThread();

	GEngine->Shutdown();
	delete GEngine;
	GEngine = nullptr;

	//GUIEngine->Shutdown();
	GShaderManager->Shutdown();
	GRHI->Destroy();

	GWindow->Close();
	GLog->TearDown();
}