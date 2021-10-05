#include "System/LaunchApp.h"
#include "Misc/LaunchGlobals.h"
#include "Misc/CoreGlobals.h"
#include "Misc/EngineGlobals.h"
#include "Misc/WorldEdGlobals.h"
#include "Containers/String.h"
#include "System/Config.h"
#include "EngineLoop.h"
#include "System/SplashScreen.h"
#include "System/EditorEngine.h"
#include "WorldEdApp.h"

WxLaunchApp::~WxLaunchApp()
{}

bool WxLaunchApp::OnInit()
{
	appShowSplash( GEditorConfig.GetValue( TEXT( "Editor.Editor" ), TEXT( "Splash" ) ).GetString().c_str() );	
	
	int32		result = GEngineLoop->Init();
	GEditorEngine = GEngine->Cast< LEditorEngine >();
	checkMsg( GEditorEngine, TEXT( "Class of engine for editor must be inherited from LEditorEngine" ) );

	appHideSplash();
	return result == 0;
}

int WxLaunchApp::OnExit()
{
	return wxApp::OnExit();
}

void WxLaunchApp::TickLE()
{
	if ( !GIsRequestingExit )
	{
		GEngineLoop->Tick();
	}
}

void WxLaunchApp::ShutdownAfterError()
{}

/**
 * The below is a manual expansion of wxWindows's IMPLEMENT_APP to allow multiple wxApps.
 *
 * @warning: when upgrading wxWindows, make sure that the below is how IMPLEMENT_APP looks like
 */
wxAppConsole* wxCreateApp()
{
	wxApp::CheckBuildOptions( WX_BUILD_OPTIONS_SIGNATURE, "lifeEngine" );
	return new WxWorldEdApp();
}
wxAppInitializer					wxTheAppInitializer( ( wxAppInitializerFunction )wxCreateApp );
WxLaunchApp& wxGetApp()				{ return *( WxLaunchApp* )wxTheApp; }