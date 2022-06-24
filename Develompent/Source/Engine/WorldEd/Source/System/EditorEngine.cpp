#include "Windows/MainWindow.h"

#include "Containers/String.h"
#include "Logger/LoggerMacros.h"
#include "System/BaseFileSystem.h"
#include "EngineDefines.h"
#include "Misc/Class.h"
#include "Misc/Misc.h"
#include "Misc/EngineGlobals.h"
#include "Misc/WorldEdGlobals.h"
#include "System/World.h"
#include "Render/Viewport.h"
#include "Render/RenderingThread.h"
#include "System/EditorEngine.h"
#include "System/AssetDataBase.h"
#include "System/ActorFactory.h"
#include "System/Package.h"
#include "Widgets/LogWidget.h"

// Actors
#include "Actors/PlayerStart.h"
#include "Actors/StaticMesh.h"
#include "Actors/Audio.h"

IMPLEMENT_CLASS( CEditorEngine )

CEditorEngine::CEditorEngine()
	: mainWindow( nullptr )
{}

CEditorEngine::~CEditorEngine()
{}

void CEditorEngine::Init()
{
	Super::Init();
	GAssetDataBase.Init();

	// Register actor factory for assets
	GActorFactory.Register( AT_StaticMesh,	&AStaticMesh::SpawnActorAsset );
	GActorFactory.Register( AT_AudioBank,	&AAudio::SpawnActorAsset );

	// Create main window of editor
	mainWindow = new WeMainWindow();
	mainWindow->showMaximized();
}

void CEditorEngine::Tick( float InDeltaSeconds )
{
	Super::Tick( InDeltaSeconds );
	GWorld->Tick( InDeltaSeconds );

	// Update viewports
	for ( uint32 index = 0, count = ( uint32 )viewports.size(); index < count; ++index )
	{
		viewports[ index ]->Tick( InDeltaSeconds );
	}

	// Wait while render thread is rendering of the frame
	FlushRenderingCommands();

	// Draw frame to viewports
	for ( uint32 index = 0, count = ( uint32 )viewports.size(); index < count; ++index )
	{
		viewports[ index ]->Draw();
	}
}

void CEditorEngine::Shutdown()
{
	Super::Shutdown();
	GAssetDataBase.Shutdown();

	viewports.clear();
	if ( mainWindow )
	{
		delete mainWindow;
		mainWindow = nullptr;
	}
}

void CEditorEngine::PrintLogToWidget( ELogType InLogType, const tchar* InMessage )
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

bool CEditorEngine::LoadMap( const std::wstring& InMap, std::wstring& OutError )
{
	if ( !Super::LoadMap( InMap, OutError ) )
	{
		LE_LOG( LT_Warning, LC_General, TEXT( "Failed loading map '%s'. Error: %s" ), InMap.c_str(), OutError.c_str() );
		GWorld->SpawnActor< APlayerStart >( Vector( 0.f, 0.f, 0.f ) );
	}

	return true;
}

std::wstring CEditorEngine::GetEditorName() const
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
	
	return ÑString::Format( TEXT( "WorldEd for %s (%s-bit, %s)" ), GGameName.c_str(), platformBitsString.c_str(), GRHI->GetRHIName() );
}