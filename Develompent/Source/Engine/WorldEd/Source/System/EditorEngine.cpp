#include "Containers/String.h"
#include "Logger/LoggerMacros.h"
#include "System/BaseFileSystem.h"
#include "EngineDefines.h"
#include "Misc/Class.h"
#include "Misc/Misc.h"
#include "Misc/EngineGlobals.h"
#include "System/World.h"
#include "Render/Viewport.h"
#include "Render/RenderingThread.h"
#include "System/EditorEngine.h"
#include "Actors/PlayerStart.h"

IMPLEMENT_CLASS( LEditorEngine )

LEditorEngine::LEditorEngine()
{}

LEditorEngine::~LEditorEngine()
{}

void LEditorEngine::Init()
{
	Super::Init();

	// Init world grid
	worldGrid = new FWorldGrid();
	worldGrid->Update( -HALF_WORLD_MAX, HALF_WORLD_MAX, 16.f );
}

void LEditorEngine::Tick( float InDeltaSeconds )
{
	Super::Tick( InDeltaSeconds );
	GWorld->Tick( InDeltaSeconds );

	// Wait while render thread is rendering of the frame
	FlushRenderingCommands();

	// Draw frame to viewports
	for ( uint32 index = 0, count = ( uint32 )viewports.size(); index < count; ++index )
	{
		viewports[ index ]->Draw();
	}
}

void LEditorEngine::Shutdown()
{
	Super::Shutdown();
	worldGrid.SafeRelease();
}

void LEditorEngine::PrintLogToWidget( ELogType InLogType, const tchar* InMessage )
{}

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