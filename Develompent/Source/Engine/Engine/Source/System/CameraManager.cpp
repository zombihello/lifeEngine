#include "System/CameraManager.h"
#include "Misc/CoreGlobals.h"
#include "System/BaseWindow.h"
#include "System/WindowEvent.h"

/*
==================
CCameraManager::BeginPlay
==================
*/
void CCameraManager::BeginPlay()
{
	// If camera is active - we update aspect ratio and width/height
	if ( activeCamera && activeCamera->IsAutoViewData() )
	{
		uint32		windowWidth;
		uint32		windowHeight;
		g_Window->GetSize( windowWidth, windowHeight );

		activeCamera->SetAspectRatio( ( float )windowWidth / windowHeight );
		activeCamera->SetOrthoWidth( windowWidth );
		activeCamera->SetOrthoHeight( windowHeight );
	}
}

/*
==================
CCameraManager::EndPlay
==================
*/
void CCameraManager::EndPlay()
{
	activeCamera = nullptr;
}

/*
==================
CCameraManager::ProcessEvent
==================
*/
void CCameraManager::ProcessEvent( struct WindowEvent &InWindowEvent )
{
	if ( !activeCamera )
	{
		return;
	}

	switch ( InWindowEvent.type )
	{
	case WindowEvent::T_WindowResize:
		if ( activeCamera && activeCamera->IsAutoViewData() )
		{
			activeCamera->SetAspectRatio( ( float )InWindowEvent.events.windowResize.width / InWindowEvent.events.windowResize.height );
			activeCamera->SetOrthoWidth( InWindowEvent.events.windowResize.width );
			activeCamera->SetOrthoHeight( InWindowEvent.events.windowResize.height );
		}
		break;
	}
}
