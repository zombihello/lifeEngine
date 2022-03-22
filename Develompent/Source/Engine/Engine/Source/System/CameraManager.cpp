#include "System/CameraManager.h"
#include "System/WindowEvent.h"

void FCameraManager::ProcessEvent( struct SWindowEvent &InWindowEvent )
{
	if ( !activeCamera )
	{
		return;
	}

	switch ( InWindowEvent.type )
	{
	case SWindowEvent::T_WindowResize:
		activeCamera->SetAspectRatio( ( float )InWindowEvent.events.windowResize.width / InWindowEvent.events.windowResize.height );
		activeCamera->SetOrthoWidth( InWindowEvent.events.windowResize.width );
		activeCamera->SetOrthoHeight( InWindowEvent.events.windowResize.height );
		break;
	}
}
