#include "Misc/Class.h"
#include "Misc/Misc.h"
#include "Logger/LoggerMacros.h"
#include "Containers/String.h"
#include "System/GameEngine.h"
#include "System/Config.h"
#include "System/BaseWindow.h"
#include "System/World.h"
#include "System/BaseFileSystem.h"
#include "Render/RenderingThread.h"
#include "Misc/EngineGlobals.h"
#include "Misc/CoreGlobals.h"
#include "RHI/BaseRHI.h"
#include "RHI/BaseViewportRHI.h"
#include "RHI/BaseDeviceContextRHI.h"
#include "Actors/PlayerStart.h"

IMPLEMENT_CLASS( LGameEngine )

LGameEngine::LGameEngine()
{}

LGameEngine::~LGameEngine()
{}

void LGameEngine::Init()
{
	Super::Init();

	// Create window and viewport
	std::wstring				gameName = GGameConfig.GetValue( TEXT( "Game.GameInfo" ), TEXT( "Name" ) ).GetString();;
	uint32						windowWidth = GEngineConfig.GetValue( TEXT( "Engine.SystemSettings" ), TEXT( "WindowWidth" ) ).GetInt();
	uint32						windowHeight = GEngineConfig.GetValue( TEXT( "Engine.SystemSettings" ), TEXT( "WindowHeight" ) ).GetInt();
	
	GWindow->Create( gameName.c_str(), windowWidth, windowHeight, SW_Default );
	viewport.Update( false, windowWidth, windowHeight, GWindow->GetHandle() );
}

void LGameEngine::Tick( float InDeltaSeconds )
{
	GWorld->Tick( InDeltaSeconds );
	viewport.Draw();
}

void LGameEngine::Shutdown()
{
	Super::Shutdown();

	// Destroy viewport
	viewport.Update( true, 0, 0, nullptr );

	// Wait while viewport RHI is not deleted
	while ( viewport.IsValid() )
	{
		appSleep( 0.1f );
	}
}

void LGameEngine::ProcessEvent( struct SWindowEvent& InWindowEvent )
{
	Super::ProcessEvent( InWindowEvent );

	switch ( InWindowEvent.type )
	{
	case SWindowEvent::T_WindowClose:
		if ( InWindowEvent.events.windowClose.windowId == GWindow->GetID() )
		{
			GIsRequestingExit = true;
		}
		break;

	case SWindowEvent::T_WindowResize:
		if ( InWindowEvent.events.windowResize.windowId == GWindow->GetID() )
		{
			viewport.Update( false, InWindowEvent.events.windowResize.width, InWindowEvent.events.windowResize.height, GWindow->GetHandle() );
		}
		break;
	}
}