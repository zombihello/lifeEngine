#include "LEBuild.h"
#if WITH_IMGUI

#include "ImGUI/imgui.h"

#include "Core.h"
#include "Misc/EngineGlobals.h"
#include "Logger/BaseLogger.h"
#include "Logger/LoggerMacros.h"
#include "RHI/BaseRHI.h"
#include "RHI/BaseDeviceContextRHI.h"
#include "Render/RenderingThread.h"
#include "ImGUI/ImGUIEngine.h"
#include "Misc/UIGlobals.h"

//
// IMGUI DRAW DATA
//

CImGUIDrawData::CImGUIDrawData() :
	isFree( true )
{
	appMemzero( &drawData, sizeof( ImDrawData ) );
}

CImGUIDrawData::~CImGUIDrawData()
{
	Clear();
}

void CImGUIDrawData::Clear()
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

void CImGUIDrawData::SetDrawData( ImDrawData* InDrawData )
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

ÑImGUIWindow::ÑImGUIWindow( ImGuiViewport* InViewport ) :
	imguiViewport( InViewport ),
	indexCurrentBuffer( 0 )
{
	for ( uint32 index = 0; index < IMGUI_DRAWBUFFERS_COUNT; ++index )
	{
		drawDataBuffers[ index ] = new CImGUIDrawData();
	}
}

void ÑImGUIWindow::Tick()
{
	CImGUIDrawData*				currentBuffer = drawDataBuffers[ indexCurrentBuffer ];
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
		UNIQUE_RENDER_COMMAND_ONEPARAMETER( ÑMainWindow_DrawImGUICommand,
											TRefCountPtr< CImGUIDrawData >, imGuiDrawData, currentBuffer,
											{
												GRHI->DrawImGUI( GRHI->GetImmediateContext(), ( ImDrawData* )imGuiDrawData->GetDrawData() );
												imGuiDrawData->MarkFree();
											} );
	}
	else
	{
		UNIQUE_RENDER_COMMAND_THREEPARAMETER( ÑChildWindow_DrawImGUICommand,
											  TRefCountPtr< CImGUIDrawData >, imGuiDrawData, currentBuffer,
											  ViewportRHIRef_t, viewportRHI, imguiViewport->ViewportRHI,
											  bool, isNeedClear, !( imguiViewport->Flags & ImGuiViewportFlags_NoRendererClear ),
											  {
													CBaseDeviceContextRHI*		immediateContext = GRHI->GetImmediateContext();
													GRHI->BeginDrawingViewport( immediateContext, viewportRHI );
													
													if ( isNeedClear )
													{
														immediateContext->ClearSurface( viewportRHI->GetSurface(), ÑColor::black );
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
ÑImGUIEngine::ÑImGUIEngine() :
	imguiContext( nullptr )
{}

/**
 * Destructor
 */
ÑImGUIEngine::~ÑImGUIEngine()
{}

/**
 * Initialize ImGUI
 */
void ÑImGUIEngine::Init()
{
	// Create ImGUI context
	imguiContext = ImGui::CreateContext();
	check( imguiContext );

	ImGuiIO&		imguiIO = ImGui::GetIO();
	imguiIO.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	imguiIO.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows

	// Initialize platform for ImGUI
	bool			result = appImGUIInit();
	check( result );

	// Initialize RHI for ImGUI
	UNIQUE_RENDER_COMMAND( ÑInitImGUICommand,
		{
			GRHI->InitImGUI( GRHI->GetImmediateContext() );
		} );

	LE_LOG( LT_Log, LC_Init, TEXT( "ImGui version: " IMGUI_VERSION ) );

	// Open main window
	ImGuiPlatformIO&	imguiPlatformIO = ImGui::GetPlatformIO();
	OpenWindow( imguiPlatformIO.Viewports[ 0 ] );
}

/**
 * Shutdown ImGUI on platform
 */
void ÑImGUIEngine::Shutdown()
{
	if ( !imguiContext )
	{
		return;
	}

	UNIQUE_RENDER_COMMAND( ÑShutdownImGUICommand,
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
void ÑImGUIEngine::ProcessEvent( struct SWindowEvent& InWindowEvent )
{
	appImGUIProcessEvent( InWindowEvent );
}

/**
 * Begin draw commands for render ImGUI
 */
void ÑImGUIEngine::BeginDraw()
{
	appImGUIBeginDrawing();
	ImGui::NewFrame();
}

void ÑImGUIEngine::OpenWindow( ImGuiViewport* InViewport )
{
	windows.push_back( new ÑImGUIWindow( InViewport ) );
}

void ÑImGUIEngine::CloseWindow( ImGuiViewport* InViewport )
{
	for ( uint32 index = 0, count = ( uint32 )windows.size(); index < count; ++index )
	{
		ÑImGUIWindow*		window = windows[ index ];
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
void ÑImGUIEngine::EndDraw()
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
#endif // WITH_IMGUI