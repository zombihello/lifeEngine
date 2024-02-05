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

#include "Core/System/Config.h"
#include "Core/Logger/LoggerMacros.h"
#include "Core/System/WindowEvent.h"
#include "Engine/Misc/EngineGlobals.h"
#include "Engine/System/InputSystem.h"

/*
==================
CInputSystem::CInputSystem
==================
*/
CInputSystem::CInputSystem() :
	mouseLocation( Math::vectorZero ),
	mouseOffset( Math::vectorZero ),
	mouseSensitivity( 0.5f )
{
	Memory::Memset( &buttonEvents, BE_None, BC_Count * sizeof( EButtonEvent ) );
}

/*
==================
CInputSystem::Init
==================
*/
void CInputSystem::Init()
{
	// Get mouse sensitivity
	CConfigValue		configSensitivity = g_Config.GetValue( CT_Input, TEXT( "InputSystem.InputSettings" ), TEXT( "Sensitivity" ) );
	if ( configSensitivity.IsValid() )
	{
		mouseSensitivity = configSensitivity.GetNumber();
	}
}

/*
==================
CInputSystem::ProcessEvent
==================
*/
void CInputSystem::ProcessEvent( struct WindowEvent& InWindowEvent )
{
	switch ( InWindowEvent.type )
	{
	case WindowEvent::T_KeyPressed:		buttonEvents[ InWindowEvent.events.key.code ] = BE_Pressed;				break;
	case WindowEvent::T_KeyReleased:		buttonEvents[ InWindowEvent.events.key.code ] = BE_Released;			break;

	case WindowEvent::T_MousePressed:		buttonEvents[ InWindowEvent.events.mouseButton.code ] = BE_Pressed;		break;
	case WindowEvent::T_MouseReleased:		buttonEvents[ InWindowEvent.events.mouseButton.code ] = BE_Released;	break;

	case WindowEvent::T_MouseMove:
		mouseOffset.x += InWindowEvent.events.mouseMove.xDirection;
		mouseOffset.y += InWindowEvent.events.mouseMove.yDirection;
		mouseLocation.x = InWindowEvent.events.mouseMove.x;
		mouseLocation.y = InWindowEvent.events.mouseMove.y;

		if ( mouseOffset.x != 0.f )
		{
			buttonEvents[ BC_MouseX ] = BE_Moved;
		}

		if ( mouseOffset.y != 0.f )
		{
			buttonEvents[ BC_MouseY ] = BE_Moved;
		}
		break;

	case WindowEvent::T_MouseWheel:
		buttonEvents[ InWindowEvent.events.mouseWheel.y > 0 ? BC_MouseWheelUp : BC_MouseWheelDown ] = BE_Scrolled;
		break;

	case WindowEvent::T_TextInput:
		break;
	}
}

/*
==================
CInputSystem::ResetEvents
==================
*/
void CInputSystem::ResetEvents()
{
	for ( uint32 index = 0; index < BC_Count; ++index )
	{
		if ( buttonEvents[ index ] == BE_Released || buttonEvents[ index ] == BE_Scrolled || buttonEvents[ index ] == BE_Moved )
		{
			buttonEvents[ index ] = BE_None;
		}
	}

	mouseOffset = Math::vectorZero;
}