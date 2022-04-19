#include <wx/wx.h>
#include <string>
#include <vector>
#include <sstream>

#include "Containers/String.h"
#include "Containers/StringConv.h"
#include "System/EditorFrame.h"
#include "System/Config.h"
#include "System/EditorEngine.h"
#include "Logger/BaseLogger.h"
#include "Logger/LoggerMacros.h"
#include "Misc/CoreGlobals.h"
#include "Misc/LaunchGlobals.h"
#include "Misc/EngineGlobals.h"
#include "Misc/WorldEdGlobals.h"
#include "System/SplashScreen.h"
#include "RHI/BaseRHI.h"
#include "EngineLoop.h"
#include "WorldEd.h"

int32 appWorldEdEntry( const tchar* InCmdLine )
{
	// Parse arguments of start for wxEntry
	std::stringstream		strStream;
	std::vector< achar* >	arguments;
	strStream << TCHAR_TO_ANSI( InCmdLine );

	while ( !strStream.eof() )
	{
		std::string		tempString;
		strStream >> tempString;

		achar*			newArgument = new achar[ tempString.size() + 1 ];
		strcpy( newArgument, tempString.data() );
		arguments.push_back( newArgument );
	}

	LE_LOG( LT_Log, LC_Init, TEXT( "WxWidgets version: %i.%i.%i" ), wxMAJOR_VERSION, wxMINOR_VERSION, wxRELEASE_NUMBER );

	// Init engine
	appShowSplash( GEditorConfig.GetValue( TEXT( "Editor.Editor" ), TEXT( "Splash" ) ).GetString().c_str() );

	GEditorEngine = GEngine->Cast< LEditorEngine >();
	checkMsg( GEditorEngine, TEXT( "Class of engine for editor must be inherited from LEditorEngine" ) );

	int32		result = GEngineLoop->Init( InCmdLine );
	check( result == 0 );

	// Start WxWidgets execute
	appHideSplash();
	int32		argumentsCount = ( int32 )arguments.size();
	result = wxEntry( argumentsCount, arguments.data() );

	// Free allocated memory
	GEditorEngine = nullptr;
	for ( uint32 index = 0, count = argumentsCount; index < argumentsCount; ++index )
	{
		delete arguments[ index ];
	}
	arguments.clear();
	return result;
}

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

	return FString::Format( TEXT( "WorldEd for %s (%s-bit, %s)" ), GGameName.c_str(), platformBitsString.c_str(), GRHI->GetRHIName() );
}