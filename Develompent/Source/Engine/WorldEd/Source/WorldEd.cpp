#include <string>
#include <vector>
#include <sstream>

#include "Misc/StringConv.h"
#include "Logger/LoggerMacros.h"
#include "System/Config.h"
#include "System/SplashScreen.h"
#include "System/EditorEngine.h"
#include "Misc/EngineGlobals.h"
#include "Misc/LaunchGlobals.h"
#include "Misc/WorldEdGlobals.h"
#include "Misc/Misc.h"
#include "Commandlets/CookPackagesCommandlet.h"
#include "Commandlets/FixupRedirectsCommandlet.h"
#include "Commandlets/ScriptCompilerCommandlet.h"
#include "RHI/BaseRHI.h"
#include "EngineLoop.h"
#include "WorldEd.h"

// Native classes
#define WITH_REGISTRANTS
#include "Classes/WorldEdClasses.h"
#undef WITH_REGISTRANTS

EditorDelegates::COnAssetsCanDelete			EditorDelegates::onAssetsCanDelete;
EditorDelegates::COnAssetsDeleted			EditorDelegates::onAssetsDeleted;
EditorDelegates::COnAssetsReloaded			EditorDelegates::onAssetsReloaded;
EditorDelegates::COnActorsSpawned			EditorDelegates::onActorsSpawned;
EditorDelegates::COnActorsDestroyed			EditorDelegates::onActorsDestroyed;
EditorDelegates::COnEditorModeChanged		EditorDelegates::onEditorModeChanged;
EditorDelegates::COnEditorCreatedNewMap		EditorDelegates::onEditorCreatedNewMap;
EditorDelegates::COnEditorLoadedMap			EditorDelegates::onEditorLoadedMap;
EditorDelegates::COnEditorSavedMap			EditorDelegates::onEditorSavedMap;
EditorDelegates::COnActorsSelected			EditorDelegates::onActorsSelected;
EditorDelegates::COnActorsUnselected		EditorDelegates::onActorsUnselected;
EditorDelegates::COnEditorMapMarkedDirty	EditorDelegates::onEditorMapMarkedDirty;

/*
==================
Sys_GetWorldEdName
==================
*/
std::wstring Sys_GetWorldEdName()
{
#if PLATFORM_64BIT
	const tchar*		platformBitsString = TEXT( "64" );
#elif PLATFORM_32BIT
	const tchar*		platformBitsString = TEXT( "32" );
#else
#error Insert court bitness of your platform
#endif // PLATFORM_64BIT || PLATFORM_32BIT

	return L_Sprintf( TEXT( "WorldEd for %s (%s-bit, %s)" ), g_GameName.c_str(), platformBitsString, g_RHI->GetRHIName() );
}

/*
==================
InitRegistrantsModule_WorldEd
Register all native classes in the system
==================
*/
void InitRegistrantsModule_WorldEd()
{
	InitRegistrants_WorldEd();
}