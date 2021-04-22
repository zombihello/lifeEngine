#include "Core.h"
#include "Misc/CoreGlobals.h"
#include "Misc/EngineGlobals.h"
#include "Logger/BaseLogger.h"
#include "System/BaseFileSystem.h"
#include "System/BaseWindow.h"
#include "Math/Color.h"
#include "RHI/BaseRHI.h"
#include "RHI/BaseViewportRHI.h"
#include "RHI/BaseDeviceContextRHI.h"
#include "RHI/BaseSurfaceRHI.h"
#include "EngineLoop.h"

// --------------
// GLOBALS
// --------------

BaseViewportRHI*			GViewportRHI = nullptr;

/**
 * Constructor
 */
EngineLoop::EngineLoop() :
	isInitialize( false )
{}

/**
 * Destructor
 */
EngineLoop::~EngineLoop()
{}

/**
 * Pre-Initialize the main loop
 */
int32 EngineLoop::PreInit( const tchar* InCmdLine )
{
	GFileSystem->SetCurrentDirectory( TEXT( "../../" ) );
	GLog->Init();
	
	return appPlatformPreInit( InCmdLine );
}

/**
 * Initialize the main loop
 */
int32 EngineLoop::Init( const tchar* InCmdLine )
{
	// Create window
	GWindow->Create( TEXT( "lifeEngine" ), 1280, 720 );
	
	// Create viewport for render
	uint32			width = 0;
	uint32			height = 0;
	GWindow->GetSize( width, height );

	GRHI->Init( false );
	GViewportRHI = GRHI->CreateViewport( GWindow->GetHandle(), width, height );

	return appPlatformInit( InCmdLine );
}

/**
 * Advances main loop
 */
void EngineLoop::Tick()
{
	// Handling system events
	{
		SWindowEvent			windowEvent;
		while ( GWindow->PollEvent( windowEvent ) )
		{
			if ( windowEvent.type == SWindowEvent::T_WindowClose )
			{
				GIsRequestingExit = true;
				break;
			}
		}
	}

	BaseDeviceContextRHI*		immediateContext = GRHI->GetImmediateContext();
	GRHI->BeginDrawingViewport( immediateContext, GViewportRHI );
	immediateContext->ClearSurface( GViewportRHI->GetSurface(), Color::black );
	GRHI->EndDrawingViewport( immediateContext, GViewportRHI, true, false );
}

/**
 * Performs shut down
 */
void EngineLoop::Exit()
{
	delete GViewportRHI;
	GRHI->Destroy();

	GWindow->Close();
	GLog->TearDown();
}