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

#include <exception>
#include <SDL.h>

#include "Core/Misc/CoreGlobals.h"
#include "Core/Misc/CommandLine.h"
#include "Core/Logger/LoggerMacros.h"
#include "Core/System/Config.h"
#include "Core/System/Application.h"
#include "Core/Platforms/Windows/WindowsGlobals.h"
#include "Core/Platforms/Windows/WindowsLogger.h"
#include "Core/Platforms/Windows/WindowsWindow.h"
#include "Core/Core.h"
#include "Engine/System/SplashScreen.h"
#include "Launch/LaunchMain.h"
#include "Launch/EngineLoop.h"

/*
==================
Sys_PlatformPreInit
==================
*/
int32 Sys_PlatformPreInit()
{
#if WITH_EDITOR
	EApplicationType	appType = CApplication::Get().GetType();
	const bool			bShowConsole = appType == AT_Tool || appType == AT_Cooker;
#else
	const bool			bShowConsole = false;
#endif // WITH_EDITOR


	if ( g_CommandLine.HasParam( TEXT( "console" ) )  || bShowConsole )
	{
		static_cast< CWindowsLogger* >( g_Log )->Show( true );
	}

	// Print version SDL to logs
	{
		SDL_version		sdlVersion;
		SDL_GetVersion( &sdlVersion );
		Logf( TEXT( "SDL version: %i.%i.%i\n" ), sdlVersion.major, sdlVersion.minor, sdlVersion.patch );
	}

	return 0;
}

/*
==================
Sys_PlatformInit
==================
*/
int32 Sys_PlatformInit()
{
	g_Window->ShowCursor();
	return 0;
}

/*
==================
Sys_GetCommandLine
==================
*/
std::wstring Sys_GetCommandLine()
{
	int32			argc = 0;
	LPWSTR*			argv = CommandLineToArgvW( GetCommandLineW(), &argc );
	std::wstring	commandLine;
	
	for ( int32 index = 0; index < argc; ++index )
	{
		commandLine += argv[ index ];
		commandLine += TEXT( " " );
	}

	LocalFree( argv );
	return commandLine;
}

/*
==================
WinMain
==================
*/
int WINAPI WinMain( HINSTANCE hInst, HINSTANCE hPreInst, LPSTR lpCmdLine, int nCmdShow )
{
	try
	{
		g_WinHInstance	= hInst;
		std::wstring	commandLine = Sys_GetCommandLine();
		return LEMain( commandLine.c_str() );
	}
	catch ( std::exception InException )
	{
		Sys_Errorf( ANSI_TO_TCHAR( InException.what() ) );
		return 1;
	}
	catch ( ... )
	{
		Sys_Errorf( TEXT( "Unknown exception" ) );
		return 1;
	}

    return 0;
}