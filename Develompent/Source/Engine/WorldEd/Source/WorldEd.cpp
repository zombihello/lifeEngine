#include <string>
#include <vector>
#include <sstream>

#include "Containers/String.h"
#include "Containers/StringConv.h"
#include "Logger/LoggerMacros.h"
#include "System/Config.h"
#include "System/SplashScreen.h"
#include "System/EditorEngine.h"
#include "Misc/EngineGlobals.h"
#include "Misc/LaunchGlobals.h"
#include "Misc/WorldEdGlobals.h"
#include "Misc/Misc.h"
#include "RHI/BaseRHI.h"
#include "EngineLoop.h"
#include "WorldEd.h"

EditorDelegates::COnAssetsCanDelete		EditorDelegates::onAssetsCanDelete;
EditorDelegates::COnAssetsDeleted			EditorDelegates::onAssetsDeleted;
EditorDelegates::COnAssetsReloaded			EditorDelegates::onAssetsReloaded;
EditorDelegates::COnActorsSpawned			EditorDelegates::onActorsSpawned;
EditorDelegates::COnActorsDestroyed		EditorDelegates::onActorsDestroyed;
EditorDelegates::COnEditorModeChanged		EditorDelegates::onEditorModeChanged;
EditorDelegates::COnEditorCreatedNewMap	EditorDelegates::onEditorCreatedNewMap;
EditorDelegates::COnEditorLoadedMap		EditorDelegates::onEditorLoadedMap;
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
#if PLATFORM_WINDOWS
#if _WIN64
	const std::wstring				platformBitsString( TEXT( "64" ) );
#else
	const std::wstring				platformBitsString( TEXT( "32" ) );
#endif // _WIN64
#else
#error Insert court bitness of your platform
#endif // PLATFORM_WINDOWS

	return CString::Format( TEXT( "WorldEd for %s (%s-bit, %s)" ), g_GameName.c_str(), platformBitsString.c_str(), g_RHI->GetRHIName() );
}