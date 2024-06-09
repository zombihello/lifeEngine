#include <stdio.h>
#include <ctime>

#include "LEBuild.h"
#include "Misc/CoreGlobals.h"
#include "Misc/EngineGlobals.h"
#include "Misc/Misc.h"
#include "Logger/LoggerMacros.h"
#include "System/BaseFileSystem.h"
#include "WindowsLogger.h"

#if WITH_EDITOR
#include "System/EditorEngine.h"
#include "Misc/WorldEdGlobals.h"
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

	std::wstring		logFile = L_Sprintf( TEXT( "%s/Logs/%s-%i.%02i.%02i-%02i.%02i.%02i.log" ), Sys_GameDir().c_str(), !g_IsEditor ? g_GameName.c_str() : TEXT( "WorldEd" ), 1900 + tmTimeNow->tm_year, 1 + tmTimeNow->tm_mon, tmTimeNow->tm_mday, tmTimeNow->tm_hour, tmTimeNow->tm_min, tmTimeNow->tm_sec );
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
	
	std::wstring			finalMessage = L_Sprintf( TEXT( "%s: %s" ), s_LogTypeNames[ ( uint32 ) InLogType ], InMessage );
	wprintf( finalMessage.c_str() );

	// Print to log widget in WorldEd
#if WITH_EDITOR
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
