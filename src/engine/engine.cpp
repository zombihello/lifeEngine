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

#include "pch_engine.h"
#include "appframework/iwindowmgr.h"
#include "gameinfo/gameinfo.h"
#include "core/icommandline.h"
#include "inputsystem/iinputsystem.h"
#include "filesystem/ifilesystem.h"
#include "game/igame.h"
#include "engine/sys_dll.h"
#include "engine/engine.h"

// The game engine
CEngine		g_Engine;

/**
 * @ingroup engine
 * @brief Cvar window width
 */
CConVar		r_windowwidth( "r_windowwidth", "1280", "Window width", FCVAR_ARCHIVE );

/**
 * @ingroup engine
 * @brief Cvar window height
 */
CConVar		r_windowheight( "r_windowheight", "720", "Window height", FCVAR_ARCHIVE );

/**
 * @ingroup engine
 * @brief Cvar is need open window in fullscreen mode
 */
CConVar		r_fullscreen( "r_fullscreen", "0", "Is need open window in fullscreen mode", FCVAR_ARCHIVE );

/*
==================
CEngine::CEngine
==================
*/
CEngine::CEngine()
	: bInitialized( false )
{}

/*
==================
CEngine::Init
==================
*/
bool CEngine::Init()
{
	// gameinfo.txt must be loaded
	if ( !gameinfo.IsLoaded() )
	{
		Warning( "CEngine: gameinfo.txt must be loaded!" );
		return false;
	}
	
	// Attach the input system to window
	g_pInputSystem->AttachToWindow( g_pWindowMgr );

	// Initialize the game
	if ( !g_pGame->Init( g_pAppSystemFactory ) )
	{
		return false;
	}

	// Read file configuration and override from command line
	ReadConfiguration( "//GAME/cfg/config.cfg" );
	OverrideConfigurationFromCommandLine();

	// Update window size and show the one
	g_pWindowMgr->SetSize( r_windowwidth.GetInt(), r_windowheight.GetInt() );
	g_pWindowMgr->SetTitle( gameinfo.GetGame().c_str() );
	g_pWindowMgr->SetFullscreen( r_fullscreen.GetBool() );
	g_pWindowMgr->ShowWindow();

	// We are done!
	bInitialized = true;
	return true;
}

/*
==================
CEngine::Shutdown
==================
*/
void CEngine::Shutdown()
{
	if ( bInitialized )
	{
		// Shutdown the game
		g_pGame->Shutdown();

		// Detach the input system from window
		g_pInputSystem->DetachFromWindow();
		bInitialized = false;
	}
}

/*
==================
CEngine::FrameUpdate
==================
*/
void CEngine::FrameUpdate()
{
	g_pGame->FrameUpdate();
}

/*
==================
CEngine::ReadConfiguration
==================
*/
void CEngine::ReadConfiguration( const achar* pPath, bool bWriteConfigIfNoExist /* = true */ )
{
	// If pPath is exist execute it
	if ( g_pFileSystem->IsFileExists( pPath ) )
	{
		g_pCvar->Exec( L_Sprintf( "exec %s", pPath ).c_str() );
		return;
	}

	// Otherwise try execute <filename>_default if the one is exist
	std::string		defaultPath;
	std::string		baseFileName;
	L_GetFilePath( pPath, defaultPath, false );
	L_GetFileBaseName( pPath, baseFileName, false );
	defaultPath		+= L_Sprintf( "%s_default", baseFileName.c_str() );
	defaultPath		+= L_GetFileExtension( pPath, true );
	if ( g_pFileSystem->IsFileExists( defaultPath.c_str() ) )
	{
		g_pCvar->Exec( L_Sprintf( "exec %s", defaultPath.c_str() ).c_str() );
	}

	// Save a new config if it need
	if ( bWriteConfigIfNoExist )
	{
		WriteConfiguration( pPath );
	}
}

/*
==================
CEngine::WriteConfiguration
==================
*/
void CEngine::WriteConfiguration( const achar* pPath )
{
	// Open file to write
	TRefPtr<IFileWriter>	file = g_pFileSystem->CreateFileWriter( pPath );
	if ( !file )
	{
		Warning( "CEngine::WriteConfiguration: Failed to create file configuration \"%s\"", pPath );
		return;
	}
	std::string		buffer;

	// Write bindings
	buffer += "unbindall\n";
	for ( uint32 index = 0; index < BUTTON_CODE_COUNT; ++index )
	{
		const achar*	pCommand = g_pInputSystem->GetBindingCommand( ( EButtonCode )index );
		if ( !pCommand || !pCommand[0] )
		{
			continue;
		}

		buffer += L_Sprintf( "bind \"%s\" \"%s\"\n", g_pInputSystem->GetButtonName( ( EButtonCode )index ), pCommand );
	}

	// Write cvars
	for ( IConCmdBase* pVar = g_pCvar->GetCommands(); pVar; pVar = pVar->GetNext() )
	{
		// Skip commands and cvars that not have FCVAR_ARCHIVE
		if ( pVar->IsCommand() || !pVar->IsFlagSet( FCVAR_ARCHIVE ) )
		{
			continue;;
		}

		buffer += L_Sprintf( "%s \"%s\"\n", pVar->GetName(), ( ( IConVar* )pVar )->GetString() );
	}

	// Write buffer into file
	file->Write( buffer.data(), buffer.size() * sizeof( achar ) );
}

/*
==================
CEngine::OverrideConfigurationFromCommandLine
==================
*/
void CEngine::OverrideConfigurationFromCommandLine()
{
	ICommandLine*	pCommandLine = CommandLine();
	
	// Check for windowed mode command line override
	if ( pCommandLine->HasParam( "windowed" ) || pCommandLine->HasParam( "window" ) )
	{
		r_fullscreen.SetInt( 0 );
	}
	// Check for fullscreen override
	else if ( pCommandLine->HasParam( "full" ) || pCommandLine->HasParam( "fullscreen" ) )
	{
		r_fullscreen.SetInt( 1 );
	}

	// Get width
	const achar*	pWidthParam = nullptr;
	if ( pCommandLine->HasParam( "width" ) )
	{
		pWidthParam = "width";
	}
	else if ( pCommandLine->HasParam( "w" ) )
	{
		pWidthParam = "w";
	}

	// Override width
	if ( pWidthParam )
	{
		r_windowwidth.SetString( pCommandLine->GetFirstValue( pWidthParam ) );
	}

	// Get height
	const achar*	pHeightParam = nullptr;
	if ( pCommandLine->HasParam( "height" ) )
	{
		pHeightParam = "height";
	}
	else if ( pCommandLine->HasParam( "h" ) )
	{
		pHeightParam = "h";
	}

	// Override height
	if ( pHeightParam )
	{
		r_windowheight.SetString( pCommandLine->GetFirstValue( pHeightParam ) );
	}
}