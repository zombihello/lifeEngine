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
#include "appframework/iwindowmgr.h"
#include "inputsystem/iinputsystem.h"
#include "launcher/launcher.h"

/**
 * @ingroup launcher
 * @brief Macro for set full name module
 */
#define LAUNCHER_APPSYSTEM( Name )		Name DLL_EXT_STRING

/*
==================
CLifeEngineApp::CLifeEngineApp
==================
*/
CLifeEngineApp::CLifeEngineApp( appInstanceHandle_t hInstance /* = nullptr */ )
	: hInstance( hInstance )
	, pWindowMgr( nullptr )
{}

/*
==================
CLifeEngineApp::Create
==================
*/
bool CLifeEngineApp::Create()
{
	// Load application systems
	AppSystemInfo		appSystemInfos[] =
	{
		{ LAUNCHER_APPSYSTEM( "inputsystem" ),		INPUTSYSTEM_INTERFACE_VERSION		},
		{ "", "" }																		// Required to terminate the list
	};

	// Add window manager to the app systems list
	pWindowMgr = CreateWindowMgr();
	AddSystem( pWindowMgr, WINDOWMGR_INTERFACE_VERSION );

	// Add all systems from array
	if ( !AddSystems( appSystemInfos ) )
	{
		return false;
	}

	// Create the window
	if ( !pWindowMgr->Create( "LifeEngine", 1280, 720 ) )
	{
		return false;
	}

	// Get the input system and attach it to the window manager
	IInputSystem*	pInputSystem = ( IInputSystem* )FindSystem( INPUTSYSTEM_INTERFACE_VERSION );
	pInputSystem->AttachToWindow( pWindowMgr );
	
	// Subscribe on close window event
	pWindowMgr->OnProcessWindowEvent()->AddFunc( []( void* pUserData, const WindowEvent& windowEvent ) 
												 {
													 if ( windowEvent.type == WindowEvent::EVENT_WINDOW_CLOSE )
													 {
														 Sys_RequestExit( false );
													 }
												 }, nullptr );
	return true;
}

/*
==================
CLifeEngineApp::Main
==================
*/
int32 CLifeEngineApp::Main()
{
	while ( !Sys_IsRequestingExit() )
	{
		pWindowMgr->ProcessEvents();
	}
	return 0;
}