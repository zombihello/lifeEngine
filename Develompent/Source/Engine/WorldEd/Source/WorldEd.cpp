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

/**
 * Globals
 */
SEditorDelegates::COnAssetsCanDelete		SEditorDelegates::onAssetsCanDelete;
SEditorDelegates::COnAssetsDeleted			SEditorDelegates::onAssetsDeleted;
SEditorDelegates::COnAssetsReloaded			SEditorDelegates::onAssetsReloaded;
SEditorDelegates::COnActorsSpawned			SEditorDelegates::onActorsSpawned;
SEditorDelegates::COnActorsDestroyed		SEditorDelegates::onActorsDestroyed;
SEditorDelegates::COnEditorModeChanged		SEditorDelegates::onEditorModeChanged;
SEditorDelegates::COnEditorCreatedNewMap	SEditorDelegates::onEditorCreatedNewMap;
SEditorDelegates::COnEditorLoadedMap		SEditorDelegates::onEditorLoadedMap;
SEditorDelegates::COnEditorSavedMap			SEditorDelegates::onEditorSavedMap;
SEditorDelegates::COnActorsSelected			SEditorDelegates::onActorsSelected;
SEditorDelegates::COnActorsUnselected		SEditorDelegates::onActorsUnselected;
SEditorDelegates::COnEditorMapMarkedDirty	SEditorDelegates::onEditorMapMarkedDirty;

std::wstring appGetWorldEdName()
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

	return CString::Format( TEXT( "WorldEd for %s (%s-bit, %s)" ), GGameName.c_str(), platformBitsString.c_str(), GRHI->GetRHIName() );
}