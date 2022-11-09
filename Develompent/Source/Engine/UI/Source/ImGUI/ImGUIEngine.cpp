#include "LEBuild.h"
#if WITH_IMGUI

#include "ImGUI/imgui.h"

#include "Core.h"
#include "Containers/StringConv.h"
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
														immediateContext->ClearSurface( viewportRHI->GetSurface(), CColor::black );
													}
													GRHI->DrawImGUI( immediateContext, ( ImDrawData* )imGuiDrawData->GetDrawData() );
													
													GRHI->EndDrawingViewport( immediateContext, viewportRHI, true, false );												
													imGuiDrawData->MarkFree();
											  } );
	}
}

//
// IMGUI LAYER
//

CImGUILayer::CImGUILayer( const std::wstring& InName /* = TEXT( "NewLayer" ) */ )
	: bVisibility( true )
	, bFocused( false )
	, bHovered( false )
	, size( 0.f, 0.f )
	, name( InName )
{}

CImGUILayer::~CImGUILayer()
{
	GImGUIEngine->RemoveLayer( this );
}

void CImGUILayer::Init()
{
	GImGUIEngine->AddLayer( this );
}

void CImGUILayer::Tick()
{
	if ( bVisibility )
	{
		if ( ImGui::Begin( TCHAR_TO_ANSI( GetName().c_str() ), &bVisibility ) )
		{
			UpdateEvents();
			OnTick();
		}
		ImGui::End();

		// If visibility is changed, we send event to children
		if ( !bVisibility )
		{
			OnVisibilityChanged( bVisibility );
		}
	}
}

bool CImGUILayer::PollEvent( SWindowEvent& OutLayerEvent )
{
	bool	bNotEndEvent = !events.empty();
	if ( !bNotEndEvent )
	{
		return false;
	}

	OutLayerEvent = events.top();
	events.pop();
	return bNotEndEvent;
}

void CImGUILayer::UpdateEvents()
{
	// Process event of loss/gain focus
	bool		bLocalFocused = ImGui::IsWindowFocused();
	if ( bFocused != bLocalFocused )
	{
		SWindowEvent	imguiEvent;
		imguiEvent.bImGUIEvent	= true;
		imguiEvent.imGUILayer	= this;
		if ( !bLocalFocused )
		{
			imguiEvent.type = SWindowEvent::T_WindowFocusLost;
			imguiEvent.events.windowFocusLost.windowId		= 0;
		}
		else
		{
			imguiEvent.type = SWindowEvent::T_WindowFocusGained;
			imguiEvent.events.windowFocusGained.windowId	= 0;
		}
	
		bFocused = bLocalFocused;
		events.push( imguiEvent );
	}
	
	// Process event of hovered
	bool		bLocalHovered = ImGui::IsWindowHovered();
	if ( bHovered != bLocalHovered )
	{
		SWindowEvent	imguiEvent;
		imguiEvent.bImGUIEvent	= true;
		imguiEvent.imGUILayer	= this;
		if ( bLocalHovered )
		{
			imguiEvent.type = SWindowEvent::T_ImGUILayerHovered;
		}
		else
		{
			imguiEvent.type = SWindowEvent::T_ImGUILayerUnHovered;
		}

		bHovered = bLocalHovered;
		events.push( imguiEvent );
	}

	// Process event of resize
	ImVec2	imguiSize = ImGui::GetContentRegionAvail();
	if ( size.x != imguiSize.x || size.y != imguiSize.y )
	{
		SWindowEvent	imguiEvent;
		imguiEvent.bImGUIEvent					= true;
		imguiEvent.imGUILayer					= this;
		imguiEvent.type							= SWindowEvent::T_WindowResize;
		imguiEvent.events.windowResize.windowId = 0;
		imguiEvent.events.windowResize.width	= Max<float>( imguiSize.x, 1.f );
		imguiEvent.events.windowResize.height	= Max<float>( imguiSize.y, 1.f );
		
		size.x = imguiEvent.events.windowResize.width;
		size.y = imguiEvent.events.windowResize.height;
		events.push( imguiEvent );
	}
}

void CImGUILayer::ProcessEvent( struct SWindowEvent& InWindowEvent )
{}

void CImGUILayer::OnVisibilityChanged( bool InNewVisibility )
{}

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

	// Init theme of ImGUI
	InitTheme();

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

void ÑImGUIEngine::InitTheme()
{
	// Init dark solors	
	ImGui::StyleColorsDark();

	float	fontSize			= 15.0f;
	ImGui::GetIO().Fonts->AddFontFromFileTTF( TCHAR_TO_ANSI( ( appBaseDir() + TEXT( "Engine/Editor/Fonts/Roboto/Roboto-Bold.ttf ") ).c_str() ), fontSize );
	ImGui::GetIO().FontDefault	= ImGui::GetIO().Fonts->AddFontFromFileTTF( TCHAR_TO_ANSI( ( appBaseDir() + TEXT( "Engine/Editor/Fonts/Roboto/Roboto-Regular.ttf " ) ).c_str() ), fontSize );

	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones
	ImGuiStyle&		style	= ImGui::GetStyle();
	if ( ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable )
	{
		style.WindowRounding				= 0.0f;
		style.Colors[ImGuiCol_WindowBg].w	= 1.0f;
	}

	// Set colors
	{
		auto&		colors					= style.Colors;
		colors[ImGuiCol_WindowBg]			= ImVec4{ 0.1f, 0.105f, 0.11f, 1.0f };

		// Headers
		colors[ImGuiCol_Header]				= ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colors[ImGuiCol_HeaderHovered]		= ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colors[ImGuiCol_HeaderActive]		= ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		// Buttons
		colors[ImGuiCol_Button]				= ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colors[ImGuiCol_ButtonHovered]		= ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colors[ImGuiCol_ButtonActive]		= ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		// Frame BG
		colors[ImGuiCol_FrameBg]			= ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colors[ImGuiCol_FrameBgHovered]		= ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colors[ImGuiCol_FrameBgActive]		= ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		// Tabs
		colors[ImGuiCol_Tab]				= ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TabHovered]			= ImVec4{ 0.38f, 0.3805f, 0.381f, 1.0f };
		colors[ImGuiCol_TabActive]			= ImVec4{ 0.28f, 0.2805f, 0.281f, 1.0f };
		colors[ImGuiCol_TabUnfocused]		= ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };

		// Title
		colors[ImGuiCol_TitleBg]			= ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TitleBgActive]		= ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TitleBgCollapsed]	= ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
	}
}

void ÑImGUIEngine::Tick( float InDeltaSeconds )
{
	for ( uint32 index = 0, count = layers.size(); index < count; ++index )
	{
		CImGUILayer*	layer = layers[index];
		SWindowEvent	layerEvent;
		while ( layer->PollEvent( layerEvent ) )
		{
			layer->ProcessEvent( layerEvent );
		}
	}
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
	// Process event by ImGUI
	appImGUIProcessEvent( InWindowEvent );

	SWindowEvent		imguiEvent;
	if ( !layers.empty() )
	{
		imguiEvent = InWindowEvent;
	}

	// Process event in all layers
	for ( uint32 index = 0, count = layers.size(); index < count; ++index )
	{
		if ( layers[index]->IsHovered() || layers[index]->IsFocused() )
		{
			imguiEvent.imGUILayer = layers[index];
			layers[index]->ProcessEvent( imguiEvent );
		}
	}
}

/**
 * Begin draw commands for render ImGUI
 */
void ÑImGUIEngine::BeginDraw()
{
	appImGUIBeginDrawing();
	ImGui::NewFrame();

	// Hide cursor if need
	if ( !bShowCursor )
	{
		ImGui::SetMouseCursor( ImGuiMouseCursor_None );
	}
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
	// Draw all layers
	for ( uint32 index = 0, count = layers.size(); index < count; ++index )
	{
		layers[index]->Tick();
	}

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