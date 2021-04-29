#include "Core.h"
#include "Misc/CoreGlobals.h"
#include "Misc/EngineGlobals.h"
#include "Misc/Misc.h"
#include "Logger/BaseLogger.h"
#include "System/BaseArchive.h"
#include "System/BaseFileSystem.h"
#include "System/BaseWindow.h"
#include "System/Config.h"
#include "Math/Color.h"
#include "RHI/BaseRHI.h"
#include "RHI/BaseViewportRHI.h"
#include "RHI/BaseDeviceContextRHI.h"
#include "RHI/BaseSurfaceRHI.h"
#include "Scripts/ScriptEngine.h"
#include "EngineLoop.h"

// --------------
// GLOBALS
// --------------

BaseViewportRHI*			GViewportRHI = nullptr;

/**
 * Constructor
 */
EngineLoop::EngineLoop() :
	isInitialize( false )
{}

/**
 * Destructor
 */
EngineLoop::~EngineLoop()
{}

/**
 * Serialize configs
 */
void EngineLoop::SerializeConfigs()
{
	// Loading engine config
	BaseArchive*		arConfig = GFileSystem->CreateFileReader( TEXT( "Config/Engine.json" ) );
	if ( arConfig )
	{
		GEngineConfig.Serialize( *arConfig );
		delete arConfig;
	}

	// Loading game config
	arConfig = GFileSystem->CreateFileReader( TEXT( "Config/Game.json" ) );
	if ( arConfig )
	{
		GGameConfig.Serialize( *arConfig );
		delete arConfig;
	}
}

/**
 * Pre-Initialize the main loop
 */
int32 EngineLoop::PreInit( const tchar* InCmdLine )
{
	GFileSystem->SetCurrentDirectory( TEXT( "../../" ) );
	GLog->Init();
	GScriptEngine->Init();

	int32		errorCode = appPlatformPreInit( InCmdLine );
	
	SerializeConfigs();
	return errorCode;
}

/**
 * Initialize the main loop
 */
int32 EngineLoop::Init( const tchar* InCmdLine )
{
	LE_LOG( LT_Log, LC_Init, TEXT( "Started with arguments: %s" ), InCmdLine );

#if WITH_EDITOR
	// Start 'MAKE' tool for compilation scripts
	if ( appParseParam( InCmdLine, TEXT( "make" ) ) )
	{
		GScriptEngine->Make( InCmdLine );
		GIsRequestingExit = true;
		
		return 0;
	}
#endif // WITH_EDITOR
	
	// Create window
	std::wstring				windowTitle = GGameConfig.GetString( TEXT( "GameInfo" ), TEXT( "Name" ), TEXT( "lifeEngine" ) );
	uint32						windowWidth = GEngineConfig.GetInt( TEXT( "SystemSettings" ), TEXT( "WindowWidth" ), 1280 );
	uint32						windowHeight = GEngineConfig.GetInt( TEXT( "SystemSettings" ), TEXT( "WindowHeight" ), 720 );
	GWindow->Create( windowTitle.c_str(), windowWidth, windowHeight );
	
	// Create viewport for render
	uint32			width = 0;
	uint32			height = 0;
	GWindow->GetSize( width, height );

	GRHI->Init( false );
	GViewportRHI = GRHI->CreateViewport( GWindow->GetHandle(), width, height );

	return appPlatformInit( InCmdLine );
}

/**
 * Advances main loop
 */
void EngineLoop::Tick()
{
	// Handling system events
	{
		SWindowEvent			windowEvent;
		while ( GWindow->PollEvent( windowEvent ) )
		{
			if ( windowEvent.type == SWindowEvent::T_WindowClose )
			{
				GIsRequestingExit = true;
				break;
			}
		}
	}

	BaseDeviceContextRHI*		immediateContext = GRHI->GetImmediateContext();
	GRHI->BeginDrawingViewport( immediateContext, GViewportRHI );
	immediateContext->ClearSurface( GViewportRHI->GetSurface(), Color::black );
	GRHI->EndDrawingViewport( immediateContext, GViewportRHI, true, false );
}

/**
 * Performs shut down
 */
void EngineLoop::Exit()
{
	delete GViewportRHI;
	GRHI->Destroy();

	GWindow->Close();
	GLog->TearDown();
}