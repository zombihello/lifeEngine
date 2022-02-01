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
#include "Actors/PlayerStart.h"
#include "Widgets/LogWidget.h"

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

void LEditorEngine::PrintLogToWidget( ELogType InLogType, const tchar* InMessage )
{
	if ( !mainWindow )
	{
		return;
	}

	WeLogWidget*		logWidget = mainWindow->GetLogWidget();
	if ( logWidget )
	{
		logWidget->Print( InLogType, InMessage );
	}
}

bool LEditorEngine::LoadMap( const std::wstring& InMap, std::wstring& OutError )
{
	if ( !Super::LoadMap( InMap, OutError ) )
	{
		LE_LOG( LT_Warning, LC_General, TEXT( "Failed loading map '%s'. Error: %s" ), InMap.c_str(), OutError.c_str() );
		GWorld->SpawnActor< APlayerStart >( FVector( 0.f, 0.f, 0.f ) );
	}

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