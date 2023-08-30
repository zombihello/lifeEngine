#include "System/Config.h"
#include "Logger/LoggerMacros.h"
#include "Misc/EngineGlobals.h"
#include "System/InputSystem.h"
#include "System/WindowEvent.h"

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
	memset( &buttonEvents, BE_None, BC_Count * sizeof( EButtonEvent ) );
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