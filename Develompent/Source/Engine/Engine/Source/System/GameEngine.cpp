#include "Containers/String.h"
#include "Misc/Misc.h"
#include "Logger/LoggerMacros.h"
#include "Reflection/Class.h"
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

IMPLEMENT_CLASS( CGameEngine )
IMPLEMENT_DEFAULT_INITIALIZE_CLASS( CGameEngine )

/*
==================
CGameEngine::CGameEngine
==================
*/
CGameEngine::CGameEngine()
{}

/*
==================
CGameEngine::Init
==================
*/
void CGameEngine::Init()
{
	Super::Init();

	// Create window and viewport
	std::wstring				gameName		= g_Config.GetValue( CT_Game, TEXT( "Game.GameInfo" ), TEXT( "Name" ) ).GetString();;
	uint32						windowWidth		= g_Config.GetValue( CT_Engine, TEXT( "Engine.SystemSettings" ), TEXT( "WindowWidth" ) ).GetInt();
	uint32						windowHeight	= g_Config.GetValue( CT_Engine, TEXT( "Engine.SystemSettings" ), TEXT( "WindowHeight" ) ).GetInt();
	
	g_Window->SetTitle( gameName.c_str() );
	g_Window->SetSize( windowWidth, windowHeight );
	viewport.SetViewportClient( &viewportClient );
	viewport.Update( false, windowWidth, windowHeight, g_Window->GetHandle() );
}

/*
==================
CGameEngine::Tick
==================
*/
void CGameEngine::Tick( float InDeltaSeconds )
{
	Super::Tick( InDeltaSeconds );
	viewport.Tick( InDeltaSeconds );

	// Wait while render thread is rendering of the frame
	FlushRenderingCommands();

	// Draw frame
	viewport.Draw();
}

/*
==================
CGameEngine::Shutdown
==================
*/
void CGameEngine::Shutdown()
{
	Super::Shutdown();

	// Destroy viewport
	viewport.Update( true, 0, 0, ( void* )nullptr );
	viewport.SetViewportClient( nullptr );

	// Wait while viewport RHI is not deleted
	while ( viewport.IsValid() )
	{
		Sys_Sleep( 0.1f );
	}
}

/*
==================
CGameEngine::LoadMap
==================
*/
bool CGameEngine::LoadMap( const std::wstring& InMap, std::wstring& OutError )
{
	if ( Super::LoadMap( InMap, OutError ) )
	{
		g_World->BeginPlay();
		return true;
	}

	return false;
}

/*
==================
CGameEngine::ProcessEvent
==================
*/
void CGameEngine::ProcessEvent( struct WindowEvent& InWindowEvent )
{
	Super::ProcessEvent( InWindowEvent );

	switch ( InWindowEvent.type )
	{
	case WindowEvent::T_WindowClose:
		if ( InWindowEvent.events.windowClose.windowId == g_Window->GetID() )
		{
			g_IsRequestingExit = true;
		}
		break;

	case WindowEvent::T_WindowResize:
		if ( InWindowEvent.events.windowResize.windowId == g_Window->GetID() )
		{
			viewport.Update( false, InWindowEvent.events.windowResize.width, InWindowEvent.events.windowResize.height, g_Window->GetHandle() );
		}
		break;
	}

	// Process event in viewport client
	CViewportClient*		viewportClient = viewport.GetViewportClient();
	if ( viewportClient )
	{
		viewportClient->ProcessEvent( InWindowEvent );
	}
}