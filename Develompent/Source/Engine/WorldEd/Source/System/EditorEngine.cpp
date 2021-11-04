#include "Windows/MainWindow.h"

#include "Containers/String.h"
#include "Logger/LoggerMacros.h"
#include "System/BaseFileSystem.h"
#include "Misc/Class.h"
#include "Misc/Misc.h"
#include "Misc/EngineGlobals.h"
#include "System/World.h"
#include "Render/Viewport.h"
#include "System/EditorEngine.h"

IMPLEMENT_CLASS( LEditorEngine )

LEditorEngine::LEditorEngine() :
	mainWindow( nullptr )
{}

LEditorEngine::~LEditorEngine()
{}

void LEditorEngine::Init()
{
	Super::Init();

	mainWindow = new WeMainWindow();
	mainWindow->showMaximized();
}

void LEditorEngine::Tick( float InDeltaSeconds )
{
	// Update world
	GWorld->Tick( InDeltaSeconds );

	// Draw to viewports
	for ( uint32 index = 0, count = ( uint32 )viewports.size(); index < count; ++index )
	{
		viewports[ index ]->Draw();
	}
}

void LEditorEngine::Shutdown()
{
	if ( mainWindow )
	{
		delete mainWindow;
		mainWindow = nullptr;
	}
}

bool LEditorEngine::LoadMap( const std::wstring& InMap, std::wstring& OutError )
{
	LE_LOG( LT_Log, LC_General, TEXT( "Load map: %s" ), InMap.c_str() );

	FArchive*		archive = GFileSystem->CreateFileReader( appBaseDir() + FString::Format( TEXT( "Content/Maps/%s" ), InMap.c_str() ) );
	if ( !archive )
	{
		OutError = TEXT( "Map not found" );
		return false;
	}

	archive->SerializePackageHeader();
	GWorld->Serialize( *archive );

	return true;
}

std::wstring LEditorEngine::GetEditorName() const
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