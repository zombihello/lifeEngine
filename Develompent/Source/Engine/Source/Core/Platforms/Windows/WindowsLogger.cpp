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

#include <stdio.h>
#include <ctime>

#include "Core/LEBuild.h"
#include "Core/Misc/CoreGlobals.h"
#include "Core/Misc/Misc.h"
#include "Core/Logger/LoggerMacros.h"
#include "Core/Containers/String.h"
#include "Core/System/Application.h"
#include "Core/System/BaseFileSystem.h"
#include "Core/Platforms/Windows/WindowsLogger.h"

#if WITH_EDITOR
#include "Editor/System/EditorEngine.h"
#include "Editor/Misc/EditorGlobals.h"
#endif // WITH_EDITOR

static const tchar* s_LogTypeNames[] =
{
	TEXT( "Log" ),
	TEXT( "Warn" ),
	TEXT( "Error" )
};

static const uint16 s_LogColors[] =
{
	0x7,			// LC_Default
	0xC,			// LC_Red
	0xE,			// LC_Yellow
	0x2				// LC_Green
};

/*
==================
CWindowsLogger::CWindowsLogger
==================
*/
CWindowsLogger::CWindowsLogger()
	: consoleHandle( nullptr )
	, archiveLogs( nullptr )
	, textColor( LC_Default )
{}

/*
==================
CWindowsLogger::~CWindowsLogger
==================
*/
CWindowsLogger::~CWindowsLogger()
{}

/*
==================
CWindowsLogger::Show
==================
*/
void CWindowsLogger::Show( bool InShowWindow )
{
#if !NO_LOGGING
	if ( InShowWindow )
	{
		if ( consoleHandle )		return;

		AllocConsole();
		consoleHandle = GetStdHandle( STD_OUTPUT_HANDLE );

		freopen( "conin$", "r", stdin );
		freopen( "conout$", "w", stdout );
		freopen( "conout$", "w", stderr );
	}
	else
	{
		consoleHandle = nullptr;
		FreeConsole();
	}
#endif // !NO_LOGGING
}

/*
==================
CWindowsLogger::Init
==================
*/
void CWindowsLogger::Init()
{
#if !NO_LOGGING
	time_t		timeNow = time( nullptr );
	tm*			tmTimeNow = localtime( &timeNow );

	std::wstring		logFile = CString::Format( TEXT( "%s/Logs/%s-%i.%02i.%02i-%02i.%02i.%02i.log" ), Sys_GameDir().c_str(), CApplication::Get().GetShortName().c_str(), 1900 + tmTimeNow->tm_year, 1 + tmTimeNow->tm_mon, tmTimeNow->tm_mday, tmTimeNow->tm_hour, tmTimeNow->tm_min, tmTimeNow->tm_sec );
	archiveLogs = g_FileSystem->CreateFileWriter( logFile.c_str(), AW_None );
	if ( archiveLogs )
	{
		archiveLogs->SetType( AT_TextFile );
		Logf( TEXT( "Opened log file '%s'\n" ), logFile.c_str() );
	}
#endif // !NO_LOGGING
}

/*
==================
CWindowsLogger::TearDown
==================
*/
void CWindowsLogger::TearDown()
{
	Show( false );

	if ( archiveLogs )
	{
		delete archiveLogs;
		archiveLogs = nullptr;
	}
}

/*
==================
CWindowsLogger::SetTextColor
==================
*/
void CWindowsLogger::SetTextColor( ELogColor InLogColor )
{
#if !NO_LOGGING
	textColor = InLogColor;
	if ( consoleHandle )
	{
		SetConsoleTextAttribute( consoleHandle, s_LogColors[ ( uint32 )textColor ] );
	}
#endif // !NO_LOGGING
}

/*
==================
CWindowsLogger::ResetTextColor
==================
*/
void CWindowsLogger::ResetTextColor()
{
#if !NO_LOGGING
	textColor = LC_Default;
	if ( consoleHandle )
	{
		SetConsoleTextAttribute( consoleHandle, s_LogColors[ ( uint32 )textColor ] );
	}
#endif // !NO_LOGGING
}

/*
==================
CWindowsLogger::Serialize
==================
*/
void CWindowsLogger::Serialize( const tchar* InMessage, ELogType InLogType )
{
	// If console is opened - get current text color
	// and change to color by event type
	ELogColor			currentLogColor = textColor;
	bool				bIsNeedResetLogColor = true;

	if ( consoleHandle )
	{
		// Change color by event type
		switch ( InLogType )
		{
		case LT_Error:
			SetTextColor( LC_Red );
			break;

		case LT_Warning:
			SetTextColor( LC_Yellow );
			break;

		default:
			bIsNeedResetLogColor = false;
			break;
		}
	}
	
	std::wstring			finalMessage = CString::Format( TEXT( "%s: %s" ), s_LogTypeNames[ ( uint32 ) InLogType ], InMessage );
	wprintf( finalMessage.c_str() );

	// Print to log widget in WorldEd
#if WITH_EDITOR
	// TODO yehor.pohuliaka - Need implement output interface to can print logs into editor's widget, into file and other
	if ( g_EditorEngine )
	{
		g_EditorEngine->PrintLogToWidget( InLogType, finalMessage.c_str() );
	}
#endif // WITH_EDITOR

	// Serialize log to file
	if ( archiveLogs )
	{
		*archiveLogs << TCHAR_TO_ANSI( finalMessage.c_str() );
		archiveLogs->Flush();
	}

	// Print message to debug output
#if !SHIPPING_BUILD
	if ( Sys_IsDebuggerPresent() )
	{
		OutputDebugStringW( finalMessage.c_str() );
	}
#endif // !SHIPPING_BUILD

	// Change text attribute to default
	if ( consoleHandle && bIsNeedResetLogColor )
	{
		SetTextColor( currentLogColor );
	}
}
