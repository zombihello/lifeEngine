#include "System/Config.h"
#include "Logger/LoggerMacros.h"
#include "Misc/EngineGlobals.h"
#include "System/InputSystem.h"
#include "System/WindowEvent.h"

FInputSystem::FInputSystem() :
	mouseLocation( FMath::vectorZero ),
	mouseOffset( FMath::vectorZero ),
	mouseSensitivity( 0.5f )
{
	memset( &buttonEvents, BE_None, BC_Count * sizeof( EButtonEvent ) );
}

void FInputSystem::Init()
{
	// Get mouse sensitivity
	FConfigValue		configSensitivity = GInputConfig.GetValue( TEXT( "InputSystem.InputSettings" ), TEXT( "Sensitivity" ) );
	if ( configSensitivity.IsValid() )
	{
		mouseSensitivity = configSensitivity.GetNumber();
	}
}

void FInputSystem::ProcessEvent( struct SWindowEvent& InWindowEvent )
{
	switch ( InWindowEvent.type )
	{
	case SWindowEvent::T_KeyPressed:		buttonEvents[ InWindowEvent.events.key.code ] = BE_Pressed;				break;
	case SWindowEvent::T_KeyReleased:		buttonEvents[ InWindowEvent.events.key.code ] = BE_Released;			break;

	case SWindowEvent::T_MousePressed:		buttonEvents[ InWindowEvent.events.mouseButton.code ] = BE_Pressed;		break;
	case SWindowEvent::T_MouseReleased:		buttonEvents[ InWindowEvent.events.mouseButton.code ] = BE_Released;	break;

	case SWindowEvent::T_MouseMove:
		mouseOffset.x += InWindowEvent.events.mouseMove.xDirection;
		mouseOffset.y += InWindowEvent.events.mouseMove.yDirection;
		mouseLocation.x = InWindowEvent.events.mouseMove.x;
		mouseLocation.y = InWindowEvent.events.mouseMove.y;
		break;

	case SWindowEvent::T_MouseWheel:
		buttonEvents[ InWindowEvent.events.mouseWheel.y > 0 ? BC_MouseWheelUp : BC_MouseWheelDown ] = BE_Scrolled;
		break;

	case SWindowEvent::T_TextInput:
		break;
	}
}

void FInputSystem::ResetEvents()
{
	for ( uint32 index = 0; index < BC_Count; ++index )
	{
		if ( buttonEvents[ index ] == BE_Released || buttonEvents[ index ] == BE_Scrolled )
		{
			buttonEvents[ index ] = BE_None;
		}
	}

	mouseOffset = FMath::vectorZero;
}