#include "ImGUI/imgui.h"

#include "Core.h"
#include "Misc/EngineGlobals.h"
#include "RHI/BaseRHI.h"
#include "RHI/BaseDeviceContextRHI.h"
#include "ImGUI/ImGUIEngine.h"

/**
 * Constructor
 */
FImGUIEngine::FImGUIEngine() :
	imguiContext( nullptr )
{}

/**
 * Destructor
 */
FImGUIEngine::~FImGUIEngine()
{}

/**
 * Initialize ImGUI
 */
bool FImGUIEngine::Init( class FBaseDeviceContextRHI* InDeviceContext )
{
	// Create ImGUI context
	imguiContext = ImGui::CreateContext();
	if ( !imguiContext )
	{
		return false;
	}

	// Initialize platform for ImGUI
	bool		result = appImGUIInit();
	if ( !result )
	{
		return false;
	}

	// Initialize RHI for ImGUI
	result = GRHI->InitImGUI( InDeviceContext );
	if ( !result )
	{
		return false;
	}

	return true;
}

/**
 * Shutdown ImGUI on platform
 */
void FImGUIEngine::Shutdown( class FBaseDeviceContextRHI* InDeviceContext )
{
	check( imguiContext );

	GRHI->ShutdownImGUI( InDeviceContext );
	appImGUIShutdown();
	ImGui::DestroyContext( imguiContext );

	imguiContext = nullptr;
}

/**
 * Process event for ImGUI
 */
void FImGUIEngine::ProcessEvent( struct SWindowEvent& InWindowEvent )
{
	appImGUIProcessEvent( InWindowEvent );
}

/**
 * Begin drawing ImGUI
 */
void FImGUIEngine::BeginDrawing( class FBaseDeviceContextRHI* InDeviceContext )
{
	GRHI->BeginDrawingImGUI( InDeviceContext );
	appImGUIBeginDrawing();
	ImGui::NewFrame();
}

/**
 * End drawing ImGUI
 */
void FImGUIEngine::EndDrawing( class FBaseDeviceContextRHI* InDeviceContext )
{
	ImGui::Render();
	appImGUIEndDrawing();
	GRHI->EndDrawingImGUI( InDeviceContext );
}