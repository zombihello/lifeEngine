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

#include "Core/Containers/String.h"
#include "Core/Containers/StringConv.h"
#include "Core/Logger/LoggerMacros.h"
#include "Core/System/BaseFileSystem.h"
#include "Core/Misc/Class.h"
#include "Core/Misc/Misc.h"
#include "Core/Misc/CoreGlobals.h"
#include "Core/System/Package.h"
#include "Core/System/Archive.h"
#include "Core/System/Config.h"
#include "Core/System/BaseWindow.h"
#include "UI/Misc/UIGlobals.h"
#include "UI/UIEngine.h"
#include "Engine/EngineDefines.h"
#include "Engine/Misc/EngineGlobals.h"
#include "Engine/System/World.h"
#include "Engine/Render/Viewport.h"
#include "Engine/Render/RenderingThread.h"
#include "Engine/System/SplashScreen.h"
#include "Engine/System/InputSystem.h"
#include "Engine/Actors/Actor.h"
#include "Misc/WorldEdGlobals.h"
#include "System/WorldEdEngine.h"

IMPLEMENT_CLASS( CWorldEdEngine )
IMPLEMENT_DEFAULT_INITIALIZE_CLASS( CWorldEdEngine )

/**
 * @ingroup WorldEd
 * @brief Viewport client for render editor interface
 */
class CEditorInterfaceViewportClient : public CViewportClient
{
public:
	/**
	 * @brief Draw viewport
	 *
	 * @param InViewport	Viewport
	 */
	virtual void Draw( CViewport* InViewport ) override
	{
		g_UIEngine->BeginDraw();
		g_UIEngine->EndDraw();
	}
};

/*
==================
CWorldEdEngine::CWorldEdEngine
==================
*/
CWorldEdEngine::CWorldEdEngine()
	: editorInterfaceViewportClient( nullptr )
{
	g_WorldEdEngine = this;
}

/*
==================
CWorldEdEngine::~CWorldEdEngine
==================
*/
CWorldEdEngine::~CWorldEdEngine()
{
	g_WorldEdEngine = nullptr;
}

/*
==================
CWorldEdEngine::Init
==================
*/
void CWorldEdEngine::Init()
{
	Super::Init();

	// Create window and main viewport
	uint32						windowWidth		= g_Config.GetValue( CT_Engine, TEXT( "Engine.SystemSettings" ), TEXT( "WindowWidth" ) ).GetInt();
	uint32						windowHeight	= g_Config.GetValue( CT_Engine, TEXT( "Engine.SystemSettings" ), TEXT( "WindowHeight" ) ).GetInt();

	g_Window->SetTitle( GetEditorName().c_str() );
	g_Window->SetSize( windowWidth, windowHeight );

	editorInterfaceViewportClient	= new CEditorInterfaceViewportClient();
	CViewport*	viewport			= new CViewport();
	viewport->SetViewportClient( editorInterfaceViewportClient );
	viewport->Update( false, windowWidth, windowHeight, g_Window->GetHandle() );
	viewports.push_back( viewport );

	// Init all windows
	editorWindow						= MakeSharedPtr<CEditorWindow>();
	editorWindow->Init();

	actorClassesWindow					= MakeSharedPtr<CActorClassesWindow>( CString::Format( TEXT( "%s Classes" ), ANSI_TO_TCHAR( IMGUI_ICON_CODE ) ), AActor::StaticClass() );
	actorClassesWindow->Init();
	
	actorPropertiesWindow				= MakeSharedPtr<CActorPropertiesWindow>( CString::Format( TEXT( "%s Properties" ), ANSI_TO_TCHAR( IMGUI_ICON_MENU ) ) );
	actorPropertiesWindow->Init();

	contentBrowserWindow				= MakeSharedPtr<CContentBrowserWindow>( CString::Format( TEXT( "%s Content" ), ANSI_TO_TCHAR( IMGUI_ICON_DATABASE ) ) );
	contentBrowserWindow->Init();

	explorerLevelWindow					= MakeSharedPtr<CExplorerLevelWindow>( CString::Format( TEXT( "%s Explorer Level" ), ANSI_TO_TCHAR( IMGUI_ICON_GLOBE ) ) );
	explorerLevelWindow->Init();

	logsWindow							= MakeSharedPtr<CLogsWindow>( CString::Format( TEXT( "%s Logs" ), ANSI_TO_TCHAR( IMGUI_ICON_FILETEXT ) ) );
	logsWindow->Init();

	viewportWindows[LVT_OrthoXY]		= MakeSharedPtr<CLevelViewportWindow>( CString::Format( TEXT( "%s Ortho XY" ), ANSI_TO_TCHAR( IMGUI_ICON_CAMERA ) ), false, LVT_OrthoXY );
	viewportWindows[LVT_OrthoXY]->Init();

	viewportWindows[LVT_OrthoXZ]		= MakeSharedPtr<CLevelViewportWindow>( CString::Format( TEXT( "%s Ortho XZ" ), ANSI_TO_TCHAR( IMGUI_ICON_CAMERA ) ), false, LVT_OrthoXZ );
	viewportWindows[LVT_OrthoXZ]->Init();

	viewportWindows[LVT_OrthoYZ]		= MakeSharedPtr<CLevelViewportWindow>( CString::Format( TEXT( "%s Ortho YZ" ), ANSI_TO_TCHAR( IMGUI_ICON_CAMERA ) ), false, LVT_OrthoYZ );
	viewportWindows[LVT_OrthoYZ]->Init();

	viewportWindows[LVT_Perspective]	= MakeSharedPtr<CLevelViewportWindow>( CString::Format( TEXT( "%s Perspective" ), ANSI_TO_TCHAR( IMGUI_ICON_CAMERA ) ), true, LVT_Perspective );
	viewportWindows[LVT_Perspective]->Init();
}

/*
==================
CWorldEdEngine::Tick
==================
*/
void CWorldEdEngine::Tick( float InDeltaSeconds )
{
	Super::Tick( InDeltaSeconds );

	// Update viewports
	for ( uint32 index = 0, count = ( uint32 )viewports.size(); index < count; ++index )
	{
		viewports[index]->Tick( InDeltaSeconds );
	}
	
	// Reset input events after engine tick
	g_InputSystem->ResetEvents();

	// Wait while render thread is rendering of the frame
	FlushRenderingCommands();

	// Draw frame to viewports
	for ( int32 index = viewports.size()-1; index >= 0; --index )
	{
		viewports[index]->Draw();
	}
}

/*
==================
CWorldEdEngine::Shutdown
==================
*/
void CWorldEdEngine::Shutdown()
{
	Super::Shutdown();

	// Destroy viewports
	for ( uint32 index = 0, count = ( uint32 )viewports.size(); index < count; ++index )
	{
		viewports[index]->Update( true, 0, 0, ( void* )nullptr );
	}
	FlushRenderingCommands();
	viewports.clear();

	if ( editorInterfaceViewportClient )
	{
		delete editorInterfaceViewportClient;
		editorInterfaceViewportClient = nullptr;
	}
}

/*
==================
CWorldEdEngine::ProcessEvent
==================
*/
void CWorldEdEngine::ProcessEvent( struct WindowEvent& InWindowEvent )
{
	Super::ProcessEvent( InWindowEvent );

	switch ( InWindowEvent.type )
	{
	case WindowEvent::T_WindowResize:
		if ( InWindowEvent.events.windowResize.windowId == g_Window->GetID() )
		{
			viewports[0]->Update( false, InWindowEvent.events.windowResize.width, InWindowEvent.events.windowResize.height, g_Window->GetHandle() );
		}
		break;
	}
}

/*
==================
CWorldEdEngine::PrintLogToWidget
==================
*/
void CWorldEdEngine::PrintLogToWidget( ELogType InLogType, const tchar* InMessage )
{
	if ( logsWindow && logsWindow->IsInit() )
	{
		logsWindow->PrintLog( InLogType, InMessage );
	}
}

/*
==================
CWorldEdEngine::GetSelectedAsset
==================
*/
std::wstring CWorldEdEngine::GetSelectedAsset() const
{
	return contentBrowserWindow->GetSelectedAssetReference();
}

/*
==================
CWorldEdEngine::SetSelectedAsset
==================
*/
void CWorldEdEngine::SetSelectedAsset( const std::wstring& InAssetReference )
{
	contentBrowserWindow->ShowAsset( InAssetReference );
}

/*
==================
CWorldEdEngine::GetEditorName
==================
*/
std::wstring CWorldEdEngine::GetEditorName() const
{
#if PLATFORM_64BIT
	const std::wstring				platformBitsString( TEXT( "64" ) );
#elif PLATFORM_32BIT
	const std::wstring				platformBitsString( TEXT( "32" ) );
#else
	#error Insert court bitness of your platform
#endif // PLATFORM_64BIT || PLATFORM_32BIT
	
	return CString::Format( TEXT( "WorldEd for %s (%s-bit, %s)" ), ANSI_TO_TCHAR( GAMENAME ), platformBitsString.c_str(), g_RHI->GetRHIName() );
}