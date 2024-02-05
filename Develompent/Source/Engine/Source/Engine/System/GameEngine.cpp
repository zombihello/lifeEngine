/**
 * ************************************************************
 *                  This file is part of:
 *                      LIFEENGINE
 *          https://github.com/zombihello/lifeEngine
 * ************************************************************
 * Copyright (C) 2024 Yehor Pohuliaka.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "Core/Misc/CoreGlobals.h"
#include "Core/Logger/LoggerMacros.h"
#include "Core/Containers/String.h"
#include "Core/Misc/Class.h"
#include "Core/Misc/Misc.h"
#include "Core/System/Config.h"
#include "Core/System/BaseWindow.h"
#include "Core/System/BaseFileSystem.h"
#include "Core/System/Application.h"
#include "Engine/System/GameEngine.h"
#include "Engine/System/World.h"
#include "Engine/Render/RenderingThread.h"
#include "Engine/Misc/EngineGlobals.h"
#include "Engine/Actors/PlayerStart.h"
#include "RHI/BaseRHI.h"
#include "RHI/BaseViewportRHI.h"
#include "RHI/BaseDeviceContextRHI.h"

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
CGameEngine::~CGameEngine
==================
*/
CGameEngine::~CGameEngine()
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
	uint32	windowWidth		= g_Config.GetValue( CT_Engine, TEXT( "Engine.SystemSettings" ), TEXT( "WindowWidth" ) ).GetInt();
	uint32	windowHeight	= g_Config.GetValue( CT_Engine, TEXT( "Engine.SystemSettings" ), TEXT( "WindowHeight" ) ).GetInt();
	
	g_Window->SetTitle( CApplication::Get().GetName().c_str() );
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