#include "ImGUI/imgui.h"

#include "Core.h"
#include "Misc/EngineGlobals.h"
#include "RHI/BaseRHI.h"
#include "RHI/BaseDeviceContextRHI.h"
#include "Render/RenderingThread.h"
#include "ImGUI/ImGUIEngine.h"
#include "Misc/WorldEdGlobals.h"

//
// IMGUI DRAW DATA
//

FImGUIDrawData::FImGUIDrawData() :
	isFree( true )
{
	appMemzero( &drawData, sizeof( ImDrawData ) );
}

FImGUIDrawData::~FImGUIDrawData()
{
	Clear();
}

void FImGUIDrawData::Clear()
{
	check( isFree );
	if ( drawData.CmdLists )
	{
		for ( uint32 index = 0; index < ( uint32 )drawData.CmdListsCount; index++ )
		{
			delete drawData.CmdLists[ index ];
		}

		delete[] drawData.CmdLists;
		drawData.CmdLists = nullptr;
	}

	drawData.Clear();
}

void FImGUIDrawData::SetDrawData( ImDrawData* InDrawData )
{
	check( isFree && InDrawData && InDrawData->Valid );
	Clear();

	isFree = false;
	drawData = *InDrawData;

	drawData.CmdLists = new ImDrawList*[ InDrawData->CmdListsCount ];
	for ( uint32 index = 0; index < ( uint32 )InDrawData->CmdListsCount; index++ )
	{
		drawData.CmdLists[ index ] = InDrawData->CmdLists[ index ]->CloneOutput();
	}
}

//
// IMGUI WINDOW
//

FImGUIWindow::FImGUIWindow( ImGuiViewport* InViewport ) :
	imguiViewport( InViewport ),
	indexCurrentBuffer( 0 )
{
	for ( uint32 index = 0; index < IMGUI_DRAWBUFFERS_COUNT; ++index )
	{
		drawDataBuffers[ index ] = new FImGUIDrawData();
	}
}

void FImGUIWindow::Tick()
{
	FImGUIDrawData*				currentBuffer = drawDataBuffers[ indexCurrentBuffer ];
	while ( !currentBuffer->IsFree() )
	{
		for ( uint32 index = 0; index < IMGUI_DRAWBUFFERS_COUNT; ++index, indexCurrentBuffer = ++indexCurrentBuffer % IMGUI_DRAWBUFFERS_COUNT )
		{
			currentBuffer = drawDataBuffers[ indexCurrentBuffer ];
			if ( currentBuffer->IsFree() )
			{
				break;
			}
		}
	}

	currentBuffer->SetDrawData( imguiViewport->DrawData );
	indexCurrentBuffer = ++indexCurrentBuffer % IMGUI_DRAWBUFFERS_COUNT;

	// If main window - ViewportRHI is nullptr; If child window - ViewportRHI is valid
	if ( !imguiViewport->ViewportRHI )
	{
		UNIQUE_RENDER_COMMAND_ONEPARAMETER( FMainWindow_DrawImGUICommand,
											TRefCountPtr< FImGUIDrawData >, imGuiDrawData, currentBuffer,
											{
												GRHI->DrawImGUI( GRHI->GetImmediateContext(), ( ImDrawData* )imGuiDrawData->GetDrawData() );
												imGuiDrawData->MarkFree();
											} );
	}
	else
	{
		UNIQUE_RENDER_COMMAND_THREEPARAMETER( FChildWindow_DrawImGUICommand,
											  TRefCountPtr< FImGUIDrawData >, imGuiDrawData, currentBuffer,
											  FViewportRHIRef, viewportRHI, imguiViewport->ViewportRHI,
											  bool, isNeedClear, !( imguiViewport->Flags & ImGuiViewportFlags_NoRendererClear ),
											  {
													FBaseDeviceContextRHI * immediateContext = GRHI->GetImmediateContext();

													GRHI->BeginDrawingViewport( immediateContext, viewportRHI );
													if ( isNeedClear )
													{
														immediateContext->ClearSurface( viewportRHI->GetSurface(), FColor::black );
													}

													GRHI->DrawImGUI( immediateContext, ( ImDrawData* )imGuiDrawData->GetDrawData() );
													GRHI->EndDrawingViewport( immediateContext, viewportRHI, true, false );
													imGuiDrawData->MarkFree();
											  } );
	}
}

//
// IMGUI ENGINE
//

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
void FImGUIEngine::Init()
{
	// Create ImGUI context
	imguiContext = ImGui::CreateContext();
	check( imguiContext );

	ImGuiIO&		imguiIO = ImGui::GetIO();
	imguiIO.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	imguiIO.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows

	// Initialize platform for ImGUI
	check( appImGUIInit() );

	// Initialize RHI for ImGUI
	UNIQUE_RENDER_COMMAND( FInitImGUICommand,
		{
			GRHI->InitImGUI( GRHI->GetImmediateContext() );
		} );

	// Open main window
	ImGuiPlatformIO&	imguiPlatformIO = ImGui::GetPlatformIO();
	OpenWindow( imguiPlatformIO.Viewports[ 0 ] );
}

/**
 * Shutdown ImGUI on platform
 */
void FImGUIEngine::Shutdown()
{
	check( imguiContext );

	UNIQUE_RENDER_COMMAND( FShutdownImGUICommand,
		{
			GRHI->ShutdownImGUI( GRHI->GetImmediateContext() );
		} );

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
 * Begin draw commands for render ImGUI
 */
void FImGUIEngine::BeginDraw()
{
	appImGUIBeginDrawing();
	ImGui::NewFrame();
}

void FImGUIEngine::OpenWindow( ImGuiViewport* InViewport )
{
	windows.push_back( new FImGUIWindow( InViewport ) );
}

void FImGUIEngine::CloseWindow( ImGuiViewport* InViewport )
{
	for ( uint32 index = 0, count = ( uint32 )windows.size(); index < count; ++index )
	{
		FImGUIWindow*		window = windows[ index ];
		if ( window->GetViewport() == InViewport )
		{			
			delete window;
			windows.erase( windows.begin() + index );
			return;
		}
	}
}

/**
 * End draw commands for render ImGUI
 */
void FImGUIEngine::EndDraw()
{
	ImGui::Render();
	appImGUIEndDrawing();

	if ( ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable )
	{
		ImGui::UpdatePlatformWindows();
	}

	for ( uint32 index = 0, count = ( uint32 )windows.size(); index < count; ++index )
	{
		windows[ index ]->Tick();
	}
}