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


#include "pch_launcher.h"
#include <filesystem>

#include "appframework/iwindowmgr.h"
#include "core/icommandline.h"
#include "inputsystem/iinputsystem.h"
#include "filesystem/ifilesystem.h"
#include "studiorender/istudiorender.h"
#include "launcher/launcher.h"

/**
 * @ingroup launcher
 * @brief Macros for set full name module
 */
#define LAUNCHER_APPSYSTEM( Name )		Name DLL_EXT_STRING

/**
 * @ingroup launcher
 * @brief Macros default game directory
 */
#define DEFAULT_GAMEDIR					"eleot"

#if ENABLE_LOGGING
static FILE*		s_pLogFile = nullptr;		// Launcher's log file

/*
==================
Launcher_LogOutput
==================
*/
static void Launcher_LogOutput( const achar* pMsg )
{
	// Print message to OS console
	if ( Sys_IsInitedConsoleIO() )
	{
		printf( pMsg );
	}

	// Print message to the log file
	if ( s_pLogFile )
	{
		fprintf( s_pLogFile, pMsg );
		fflush( s_pLogFile );
	}

	// Print message to debug output
	if ( Sys_IsDebuggerPresent() )
	{
		Sys_DebugMessage( pMsg );
	}

	// Print message to the engine's console
	if ( g_pCvar )
	{
		g_pCvar->ConsolePrintf( pMsg );
	}
}
#endif // ENABLE_LOGGING

/*
==================
Launcher_InitLogOutput
==================
*/
void Launcher_InitLogOutput()
{
#if ENABLE_LOGGING
	// Create directory for logs
	std::filesystem::create_directory( "../../logs" );
	
	// Open a log file
	s_pLogFile = fopen( "../../logs/launcher.log", "w" );
	
	// Set our log output function
	Sys_SetLogOutputFunc( Launcher_LogOutput );
#endif // ENABLE_LOGGING
}

/*
==================
CLifeEngineApp::CLifeEngineApp
==================
*/
CLifeEngineApp::CLifeEngineApp( appInstanceHandle_t hInstance /* = nullptr */ )
	: hInstance( hInstance )
	, pWindowMgr( nullptr )
	, pEngineLauncher( nullptr )
{}

/*
==================
CLifeEngineApp::Create
==================
*/
bool CLifeEngineApp::Create()
{
	// Print some info about this engine build and user
	Msg( "Engine build: %i (" __DATE__ " " __TIME__ ")", Sys_BuildNumber() );
	Msg( "User %s//%s", Sys_GetComputerName(), Sys_GetUserName() );

	// Load application systems
	AppSystemInfo		appSystemInfos[] =
	{
		{ LAUNCHER_APPSYSTEM( "engine" ),			CVAR_QUERY_INTERFACE_VERSION		},	// This one must be first
		{ LAUNCHER_APPSYSTEM( "filesystem" ),		FILESYSTEM_INTERFACE_VERSION		},
		{ LAUNCHER_APPSYSTEM( "engine" ),			CVAR_INTERFACE_VERSION				},
		{ LAUNCHER_APPSYSTEM( "inputsystem" ),		INPUTSYSTEM_INTERFACE_VERSION		},
		{ LAUNCHER_APPSYSTEM( "studiorender" ),		STUDIORENDER_INTERFACE_VERSION		},
		{ LAUNCHER_APPSYSTEM( "engine" ),			ENGINE_LAUNCHER_INTERFACE_VERSION	},
		{ "", "" }																			// Required to terminate the list
	};

	// Add the window manager to app systems list
	pWindowMgr = CreateWindowMgr();
	AddSystem( pWindowMgr, WINDOWMGR_INTERFACE_VERSION );

	// Add all systems from array
	if ( !AddSystems( appSystemInfos ) )
	{
		return false;
	}

	// Load up the appropriate Studio API DLL
	// This has to be done before connection 
	const achar*	pStudioAPIDLL = nullptr;
#if PLATFORM_WINDOWS
	pStudioAPIDLL = "studioapi_dx11" DLL_EXT_STRING;
#else
	#error Unknown platform
#endif // PLATFORM_WINDOWS

	IStudioRender*		pStudioRender = ( IStudioRender* )FindSystem( STUDIORENDER_INTERFACE_VERSION );
	pStudioRender->SetStudioAPI( pStudioAPIDLL );

	// Get the engine launcher
	pEngineLauncher = ( IEngineLauncher* )FindSystem( ENGINE_LAUNCHER_INTERFACE_VERSION );
	return true;
}

/*
==================
CLifeEngineApp::PreInit
==================
*/
bool CLifeEngineApp::PreInit()
{
	// Connect StdLib and register cvars
	if ( !ConnectStdLib( GetFactory() ) )
	{
		return false;
	}
	ConVar_Register();

	// Create a hidden window for we can init render context and other things links with the one
	if ( !pWindowMgr->Create( "LifeEngine", 1, 1, WINDOW_STYLE_DEFAULT | WINDOW_STYLE_HIDDEN ) )
	{
		Sys_Error( "Launcher: Failed to create window" );
		return false;
	}

	// Set startup info
	StartupInfo							startupInfo;
	startupInfo.pAppInstance			= hInstance;
	startupInfo.pGame					= CommandLine()->HasParam( "game" ) ? CommandLine()->GetFirstValue( "game" ) : DEFAULT_GAMEDIR;
	startupInfo.pParentAppSystemGroup	= this;
	pEngineLauncher->SetStartupInfo( startupInfo );
	return true;
}

/*
==================
CLifeEngineApp::Main
==================
*/
int32 CLifeEngineApp::Main()
{
	return pEngineLauncher->Run();
}

/*
==================
CLifeEngineApp::PostShutdown
==================
*/
void CLifeEngineApp::PostShutdown()
{
	ConVar_Unregister();
	DisconnectStdLib();
	pWindowMgr->Close();

	pWindowMgr = nullptr;
	pEngineLauncher = nullptr;
}