#include "LEBuild.h"
#if WITH_IMGUI

#include "ImGUI/imgui.h"
#include "ImGUI/imgui_internal.h"

#include "Core.h"
#include "Containers/StringConv.h"
#include "Misc/EngineGlobals.h"
#include "Logger/BaseLogger.h"
#include "Logger/LoggerMacros.h"
#include "RHI/BaseRHI.h"
#include "RHI/BaseDeviceContextRHI.h"
#include "Render/RenderingThread.h"
#include "ImGUI/ImGUIEngine.h"
#include "ImGUI/ImGuizmo.h"
#include "Misc/UIGlobals.h"
#include "System/ConsoleSystem.h"

#if !SHIPPING_BUILD
/**
 * @ingroup UI
 * @brief CVar enable/disable debug mode of the ImGUI
 * @note This console variable is exist when SHIPPING_BUILD is disabled
 */
CConVar		CVarImGUIDebug( TEXT( "imgui.debug" ), TEXT( "0" ), CVT_Bool, TEXT( "Enable/Disable debug mode of the ImGUI" ) );
#endif // !SHIPPING_BUILD

// Colors of ImGUI theme
namespace ImGui::Colors
{
	constexpr uint32 highlight            = IM_COL32( 39, 185, 242, 255 );
	constexpr uint32 background           = IM_COL32( 36, 36, 36, 255 );
	constexpr uint32 backgroundDark       = IM_COL32( 24, 24, 24, 255 );
	constexpr uint32 titlebar             = IM_COL32( 21, 21, 21, 255 );
	constexpr uint32 propertyField        = IM_COL32( 15, 15, 15, 255 );
	constexpr uint32 text                 = IM_COL32( 192, 192, 192, 255 );
	constexpr uint32 muted                = IM_COL32( 77, 77, 77, 255 );
	constexpr uint32 groupHeader          = IM_COL32( 47, 47, 47, 255 );
	constexpr uint32 selection            = IM_COL32( 237, 192, 119, 255 );
	constexpr uint32 selectionMuted       = IM_COL32( 237, 201, 142, 23 );
	constexpr uint32 backgroundPopup      = IM_COL32( 70, 70, 70, 255 );
}

//
// IMGUI DRAW DATA
//

/*
==================
CImGUIDrawData::CImGUIDrawData
==================
*/
CImGUIDrawData::CImGUIDrawData() :
	isFree( true )
{
	Sys_Memzero( &drawData, sizeof( ImDrawData ) );
}

/*
==================
CImGUIDrawData::CImGUIDrawData
==================
*/
CImGUIDrawData::~CImGUIDrawData()
{
	Clear();
}

/*
==================
CImGUIDrawData::CImGUIDrawData
==================
*/
void CImGUIDrawData::Clear()
{
	Assert( isFree );
	if ( drawData.CmdLists )
	{
		for ( uint32 index = 0; index < ( uint32 )drawData.CmdListsCount; index++ )
		{
			// Unlock textures
			ImDrawList*					drawList = drawData.CmdLists[index];
			for ( uint32 bufferIndex = 0; bufferIndex < drawList->CmdBuffer.Size; ++bufferIndex )
			{
				const ImDrawCmd&		drawCmd = drawList->CmdBuffer[bufferIndex];
				if ( drawCmd.TextureId.handle )
				{
					drawCmd.TextureId.handle->ReleaseRef();
				}
			}

			delete drawList;
		}

		delete[] drawData.CmdLists;
		drawData.CmdLists = nullptr;
	}

	drawData.Clear();
}

/*
==================
CImGUIDrawData::CImGUIDrawData
==================
*/
void CImGUIDrawData::SetDrawData( ImDrawData* InDrawData )
{
	Assert( isFree && InDrawData && InDrawData->Valid );
	Clear();

	isFree = false;
	drawData = *InDrawData;

	drawData.CmdLists = new ImDrawList*[ InDrawData->CmdListsCount ];
	for ( uint32 index = 0; index < ( uint32 )InDrawData->CmdListsCount; index++ )
	{
		ImDrawList* drawList		= InDrawData->CmdLists[index]->CloneOutput();
		drawData.CmdLists[ index ]	= drawList;
		
		// Lock textures 
		for ( uint32 bufferIndex = 0; bufferIndex < drawList->CmdBuffer.Size; ++bufferIndex )
		{
			const ImDrawCmd& drawCmd = drawList->CmdBuffer[bufferIndex];
			if ( drawCmd.TextureId.handle )
			{
				drawCmd.TextureId.handle->AddRef();
			}
		}
	}
}

//
// IMGUI WINDOW
//

/*
==================
CImGUIWindow::CImGUIWindow
==================
*/
CImGUIWindow::CImGUIWindow( ImGuiViewport* InViewport ) :
	imguiViewport( InViewport ),
	indexCurrentBuffer( 0 )
{
	for ( uint32 index = 0; index < IMGUI_DRAWBUFFERS_COUNT; ++index )
	{
		drawDataBuffers[ index ] = new CImGUIDrawData();
	}
}

/*
==================
CImGUIWindow::CImGUIWindow
==================
*/
void CImGUIWindow::Tick()
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
		UNIQUE_RENDER_COMMAND_ONEPARAMETER( CMainWindow_DrawImGUICommand,
											TRefCountPtr< CImGUIDrawData >, imGuiDrawData, currentBuffer,
											{
												g_RHI->DrawImGUI( g_RHI->GetImmediateContext(), ( ImDrawData* )imGuiDrawData->GetDrawData() );
												imGuiDrawData->MarkFree();
											} );
	}
	else
	{
		UNIQUE_RENDER_COMMAND_THREEPARAMETER( CChildWindow_DrawImGUICommand,
											  TRefCountPtr< CImGUIDrawData >, imGuiDrawData, currentBuffer,
											  ViewportRHIRef_t, viewportRHI, imguiViewport->ViewportRHI,
											  bool, isNeedClear, !( imguiViewport->Flags & ImGuiViewportFlags_NoRendererClear ),
											  {
													CBaseDeviceContextRHI*		immediateContext = g_RHI->GetImmediateContext();
													g_RHI->BeginDrawingViewport( immediateContext, viewportRHI );
													
													if ( isNeedClear )
													{
														immediateContext->ClearSurface( viewportRHI->GetSurface(), CColor::black );
													}
													g_RHI->DrawImGUI( immediateContext, ( ImDrawData* )imGuiDrawData->GetDrawData() );
													
													g_RHI->EndDrawingViewport( immediateContext, viewportRHI, true, false );												
													imGuiDrawData->MarkFree();
											  } );
	}
}

//
// IMGUI POPUP
//

/*
==================
CImGUIPopup::CImGUIPopup
==================
*/
CImGUIPopup::CImGUIPopup( const std::wstring& InName /* = TEXT( "NewPopup" ) */ )
	: bOpen( false )
	, bNeedClose( false )
	, bNeedOpen( false )
	, bPendingChangeSize( false )
	, name( InName )
{}

/*
==================
CImGUIPopup::CImGUIPopup
==================
*/
void CImGUIPopup::Tick()
{
	// Open popup is need
	if ( bNeedOpen )
	{
		ImGui::OpenPopup( TCHAR_TO_ANSI( name.c_str() ) );
		bNeedOpen	= false;
		bOpen		= true;
	}

	if ( bOpen )
	{
		// Always center this window when appearing
		ImGui::SetNextWindowPos( ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2( 0.5f, 0.5f ) );

		// Update window size if need
		if ( bPendingChangeSize )
		{
			ImGui::SetNextWindowSize( ImVec2{ pendingSize.x, pendingSize.y } );
			bPendingChangeSize = false;
		}

		// Draw popup window
		if ( ImGui::BeginPopupModal( TCHAR_TO_ANSI( name.c_str() ), nullptr, ImGuiWindowFlags_AlwaysAutoResize ) )
		{
			OnTick();

			if ( bNeedClose )
			{
				ImGui::CloseCurrentPopup();
				bNeedClose = false;
				bOpen = false;
			}
			ImGui::EndPopup();
		}
	}
}

//
// IMGUI LAYER
//

/*
==================
CImGUILayer::CImGUILayer
==================
*/
CImGUILayer::CImGUILayer( const std::wstring& InName /* = TEXT( "NewLayer" ) */ )
	: flags( 0 )
	, bInit( false )
	, bVisibility( true )
	, bFocused( false )
	, bHovered( false )
	, bPendingChangeSize( false )
	, bPendingChangePos( false )
	, bPendingChangeViewportID( false )
	, rounding( ImGui::GetStyle().WindowRounding )
	, borderSize( ImGui::GetStyle().WindowBorderSize )
	, size( 0.f, 0.f )
	, position( 0.f, 0.f )
	, viewportID( -1 )
	, padding( 5.f, 5.f )
	, name( InName )
{}

/*
==================
CImGUILayer::~CImGUILayer
==================
*/
CImGUILayer::~CImGUILayer()
{
	Destroy();
}

/*
==================
CImGUILayer::Init
==================
*/
void CImGUILayer::Init()
{
	if ( !bInit )
	{
		g_ImGUIEngine->AddLayer( AsShared() );
		bInit = true;
	}
}

/*
==================
CImGUILayer::Destroy
==================
*/
void CImGUILayer::Destroy()
{
	if ( bInit )
	{
		g_ImGUIEngine->RemoveLayer( AsShared() );
		bInit = false;
	}
}

/*
==================
CImGUILayer::Tick
==================
*/
void CImGUILayer::Tick()
{
	if ( bVisibility )
	{
		ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2{ padding.x, padding.y } );
		uint32		numStyleVarsToPop = 1;

		// Update window position if need
		if ( bPendingChangePos )
		{
			ImGui::SetNextWindowPos( ImVec2{ position.x, position.y } );
			bPendingChangePos = false;
		}

		// Update window size if need
		if ( bPendingChangeSize )
		{
			ImGui::SetNextWindowSize( ImVec2{ size.x, size.y } );
			bPendingChangeSize = false;
		}

		// Update viewport ID if need
		if ( bPendingChangeViewportID )
		{
			ImGui::SetNextWindowViewport( viewportID );
			bPendingChangeViewportID = false;
		}

		// Push style variables		
		ImGui::PushStyleVar( ImGuiStyleVar_WindowRounding, rounding );
		ImGui::PushStyleVar( ImGuiStyleVar_WindowBorderSize, borderSize );
		numStyleVarsToPop += 2;

		if ( ImGui::Begin( TCHAR_TO_ANSI( GetName().c_str() ), &bVisibility, flags ) )
		{
			ImGui::PopStyleVar( 2 );
			numStyleVarsToPop -= 2;

			// Update popup if him is existing
			if ( popup )
			{
				TSharedPtr<CImGUIPopup>		currentPopup = popup;		// It need for remember current popup, because in him Tick will be change to other
				currentPopup->Tick();

				// If popup is closed and him is not changed in Tick, we reset pointer
				if ( !currentPopup->IsOpen() && popup == currentPopup )
				{
					popup = nullptr;
				}
			}

			UpdateEvents();
			OnTick();
		}
		ImGui::End();
		ImGui::PopStyleVar( Max( ( uint32 )0, numStyleVarsToPop ) );

		// If visibility is changed, we send event to children
		if ( !bVisibility )
		{
			OnVisibilityChanged( bVisibility );
			if ( flags & LF_DestroyOnHide )
			{
				Destroy();
			}
		}
	}
}

/*
==================
CImGUILayer::PollEvent
==================
*/
bool CImGUILayer::PollEvent( WindowEvent& OutLayerEvent )
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

/*
==================
CImGUILayer::UpdateEvents
==================
*/
void CImGUILayer::UpdateEvents()
{
	// Process event of loss/gain focus
	bool		bLocalFocused = ImGui::IsWindowFocused();
	if ( bFocused != bLocalFocused )
	{
		WindowEvent	imguiEvent;
		imguiEvent.bImGUIEvent	= true;
		imguiEvent.imGUILayer	= AsWeak();
		if ( !bLocalFocused )
		{
			imguiEvent.type = WindowEvent::T_WindowFocusLost;
			imguiEvent.events.windowFocusLost.windowId		= 0;
		}
		else
		{
			imguiEvent.type = WindowEvent::T_WindowFocusGained;
			imguiEvent.events.windowFocusGained.windowId	= 0;
		}
	
		bFocused = bLocalFocused;
		events.push( imguiEvent );
	}
	
	// Process event of hovered
	bool		bLocalHovered = ImGui::IsWindowHovered();
	if ( bHovered != bLocalHovered )
	{
		WindowEvent	imguiEvent;
		imguiEvent.bImGUIEvent	= true;
		imguiEvent.imGUILayer	= AsWeak();
		if ( bLocalHovered )
		{
			imguiEvent.type = WindowEvent::T_ImGUILayerHovered;
		}
		else
		{
			imguiEvent.type = WindowEvent::T_ImGUILayerUnHovered;
		}

		bHovered = bLocalHovered;
		events.push( imguiEvent );
	}

	// Process event of resize
	ImVec2	imguiSize = ImGui::GetContentRegionAvail();
	if ( size.x != imguiSize.x || size.y != imguiSize.y )
	{
		WindowEvent	imguiEvent;
		imguiEvent.bImGUIEvent					= true;
		imguiEvent.imGUILayer					= AsWeak();
		imguiEvent.type							= WindowEvent::T_WindowResize;
		imguiEvent.events.windowResize.windowId = 0;
		imguiEvent.events.windowResize.width	= Max<float>( imguiSize.x, 1.f );
		imguiEvent.events.windowResize.height	= Max<float>( imguiSize.y, 1.f );
		
		size.x = imguiEvent.events.windowResize.width;
		size.y = imguiEvent.events.windowResize.height;
		events.push( imguiEvent );
	}
}

/*
==================
CImGUILayer::ProcessEvent
==================
*/
void CImGUILayer::ProcessEvent( struct WindowEvent& InWindowEvent )
{}

/*
==================
CImGUILayer::OnVisibilityChanged
==================
*/
void CImGUILayer::OnVisibilityChanged( bool InNewVisibility )
{}

//
// IMGUI ENGINE
//

/*
==================
CImGUIEngine::CImGUIEngine
==================
*/
CImGUIEngine::CImGUIEngine() 
	: imguiContext( nullptr )
{
	Sys_Memzero( &styleColors, sizeof( styleColors ) );
}

/*
==================
CImGUIEngine::~CImGUIEngine
==================
*/
CImGUIEngine::~CImGUIEngine()
{}

/*
==================
CImGUIEngine::Init
==================
*/
void CImGUIEngine::Init()
{
	// Create ImGUI context
	imguiContext = ImGui::CreateContext();
	Assert( imguiContext );

	ImGuiIO&		imguiIO = ImGui::GetIO();
	imguiIO.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	imguiIO.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows

	// Init theme of ImGUI
	InitTheme();

	// Initialize platform for ImGUI
	bool			result = Sys_ImGUIInit();
	Assert( result );

	// Initialize RHI for ImGUI
	UNIQUE_RENDER_COMMAND( ÑInitImGUICommand,
		{
			g_RHI->InitImGUI( g_RHI->GetImmediateContext() );
		} );

	Logf( TEXT( "ImGui version: " IMGUI_VERSION "\n" ) );

	// Open main window
	ImGuiPlatformIO&	imguiPlatformIO = ImGui::GetPlatformIO();
	OpenWindow( imguiPlatformIO.Viewports[ 0 ] );
}

/*
==================
CImGUIEngine::InitTheme
==================
*/
void CImGUIEngine::InitTheme()
{
	// Init dark solors	
	ImGui::StyleColorsDark();

	float	fontSize			= 15.0f;
	ImGui::GetIO().Fonts->AddFontFromFileTTF( TCHAR_TO_ANSI( ( Sys_BaseDir() + TEXT( "Engine/Editor/Fonts/Roboto/Roboto-Bold.ttf ") ).c_str() ), fontSize );
	ImGui::GetIO().FontDefault	= ImGui::GetIO().Fonts->AddFontFromFileTTF( TCHAR_TO_ANSI( ( Sys_BaseDir() + TEXT( "Engine/Editor/Fonts/Roboto/Roboto-Regular.ttf " ) ).c_str() ), fontSize );
	
	ImFontConfig	imguiIconsConfig;
	imguiIconsConfig.OversampleH			= 2;
	imguiIconsConfig.MergeMode				= true;
	imguiIconsConfig.GlyphMinAdvanceX		= 14.0f;									// Use if you want to make the icon monospaced
	static const ImWchar	iconRanges[]	= { IMGUI_ICON_MIN, IMGUI_ICON_MAX, 0 };
	ImGui::GetIO().Fonts->AddFontFromFileTTF( TCHAR_TO_ANSI( ( Sys_BaseDir() + TEXT( "Engine/Editor/Fonts/OpenFontIcons/OpenFontIcons.ttf " ) ).c_str() ), 12, &imguiIconsConfig, iconRanges );

	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones
	ImGuiStyle&		style	= ImGui::GetStyle();
	if ( ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable )
	{
		style.WindowRounding				= 0.0f;
		style.Colors[ImGuiCol_WindowBg].w	= 1.0f;
	}

	// Set colors
	{
		auto& colors = style.Colors;
		
		// *************************
		//			COLORS
		// *************************	
		// Headers
		colors[ImGuiCol_Header]					= ImGui::ColorConvertU32ToFloat4( ImGui::Colors::groupHeader );
		colors[ImGuiCol_HeaderHovered]			= ImGui::ColorConvertU32ToFloat4( ImGui::Colors::groupHeader );
		colors[ImGuiCol_HeaderActive]			= ImGui::ColorConvertU32ToFloat4( ImGui::Colors::groupHeader );

		// Buttons
		colors[ImGuiCol_Button]					= ImColor( 56, 56, 56, 200 );
		colors[ImGuiCol_ButtonHovered]			= ImColor( 70, 70, 70, 255 );
		colors[ImGuiCol_ButtonActive]			= ImColor( 56, 56, 56, 150 );

		// Frame BG
		colors[ImGuiCol_FrameBg]				= ImGui::ColorConvertU32ToFloat4( ImGui::Colors::propertyField );
		colors[ImGuiCol_FrameBgHovered]			= ImGui::ColorConvertU32ToFloat4( ImGui::Colors::propertyField );
		colors[ImGuiCol_FrameBgActive]			= ImGui::ColorConvertU32ToFloat4( ImGui::Colors::propertyField );

		// Tabs
		colors[ImGuiCol_Tab]					= ImGui::ColorConvertU32ToFloat4( ImGui::Colors::titlebar );
		colors[ImGuiCol_TabHovered]				= ImColor( 135, 135, 135, 30 );
		colors[ImGuiCol_TabActive]				= ImColor( 135, 135, 135, 60 );
		colors[ImGuiCol_TabUnfocused]			= ImGui::ColorConvertU32ToFloat4( ImGui::Colors::titlebar );
		colors[ImGuiCol_TabUnfocusedActive]		= colors[ImGuiCol_TabHovered];

		// Title
		colors[ImGuiCol_TitleBg]				= ImGui::ColorConvertU32ToFloat4( ImGui::Colors::titlebar );
		colors[ImGuiCol_TitleBgActive]			= ImGui::ColorConvertU32ToFloat4( ImGui::Colors::titlebar );
		colors[ImGuiCol_TitleBgCollapsed]		= ImVec4( 0.15f, 0.1505f, 0.151f, 1.0f );

		// Resize Grip
		colors[ImGuiCol_ResizeGrip]				= ImVec4( 0.91f, 0.91f, 0.91f, 0.25f );
		colors[ImGuiCol_ResizeGripHovered]		= ImVec4( 0.81f, 0.81f, 0.81f, 0.67f );
		colors[ImGuiCol_ResizeGripActive]		= ImVec4( 0.46f, 0.46f, 0.46f, 0.95f );

		// Scrollbar
		colors[ImGuiCol_ScrollbarBg]			= ImVec4( 0.02f, 0.02f, 0.02f, 0.53f );
		colors[ImGuiCol_ScrollbarGrab]			= ImVec4( 0.31f, 0.31f, 0.31f, 1.0f );
		colors[ImGuiCol_ScrollbarGrabHovered]	= ImVec4( 0.41f, 0.41f, 0.41f, 1.0f );
		colors[ImGuiCol_ScrollbarGrabActive]	= ImVec4( 0.51f, 0.51f, 0.51f, 1.0f );

		// Slider
		colors[ImGuiCol_SliderGrab]				= ImVec4( 0.51f, 0.51f, 0.51f, 0.7f	);
		colors[ImGuiCol_SliderGrabActive]		= ImVec4( 0.66f, 0.66f, 0.66f, 1.0f	);

		// Text
		colors[ImGuiCol_Text]					= ImGui::ColorConvertU32ToFloat4( ImGui::Colors::text );

		// Checkbox
		colors[ImGuiCol_CheckMark]				= ImGui::ColorConvertU32ToFloat4( ImGui::Colors::text );

		// Separator
		colors[ImGuiCol_Separator]				= ImGui::ColorConvertU32ToFloat4( ImGui::Colors::backgroundDark );
		colors[ImGuiCol_SeparatorActive]		= ImGui::ColorConvertU32ToFloat4( ImGui::Colors::highlight );
		colors[ImGuiCol_SeparatorHovered]		= ImColor( 39, 185, 242, 150 );

		// Window Background
		colors[ImGuiCol_WindowBg]				= ImGui::ColorConvertU32ToFloat4( ImGui::Colors::background );
		colors[ImGuiCol_ChildBg]				= ImGui::ColorConvertU32ToFloat4( ImGui::Colors::background );
		colors[ImGuiCol_PopupBg]				= ImGui::ColorConvertU32ToFloat4( ImGui::Colors::backgroundPopup );
		colors[ImGuiCol_Border]					= ImGui::ColorConvertU32ToFloat4( ImGui::Colors::backgroundDark );

		// Tables
		colors[ImGuiCol_TableHeaderBg]			= ImGui::ColorConvertU32ToFloat4( ImGui::Colors::groupHeader );
		colors[ImGuiCol_TableBorderLight]		= ImGui::ColorConvertU32ToFloat4( ImGui::Colors::backgroundDark );

		// *************************
		//			STYLE
		// *************************
		style.FrameRounding		= 2.5f;
		style.FrameBorderSize	= 1.0f;
		style.IndentSpacing		= 11.0f;
	}

	styleColors[IGC_Selection]				= ImVec4{ 0.f, 0.43f, 0.87f, 1.f };
	styleColors[IGC_Asset_Unknown]			= ImVec4( 1.f, 1.f, 1.f, 1.f );
	styleColors[IGC_Asset_Texture2D]		= ImVec4( 0.75f, 0.25f, 0.25f, 1.f );
	styleColors[IGC_Asset_Material]			= ImVec4( 0.25f, 0.75f, 0.25f, 1.f );
	styleColors[IGC_Asset_Script]			= ImVec4( 0.f, 0.f, 0.f, 0.f );
	styleColors[IGC_Asset_StaticMesh]		= ImVec4( 0.f, 1.f, 1.f, 1.f );
	styleColors[IGC_Asset_AudioBank]		= ImVec4( 0.38f, 0.33f, 0.83f, 1.f );
	styleColors[IGC_Asset_PhysicsMaterial]	= ImVec4( 0.78f, 0.75f, 0.5f, 1.f );
	styleColors[IGC_TableBgColor0]			= ImGui::ColorConvertU32ToFloat4( CColor( 26.f, 27.f, 28.f ).GetUInt32Color() );
	styleColors[IGC_TableBgColor1]			= ImGui::ColorConvertU32ToFloat4( CColor( 32.f, 35.f, 36.f ).GetUInt32Color() );
	styleColors[IGC_WarningColor]			= ImVec4( 1.0f, 0.8f, 0.6f, 1.0f );
	styleColors[IGC_ErrorColor]				= ImVec4( 1.0f, 0.4f, 0.4f, 1.0f );
	styleColors[IGC_AxisXColor]				= ImVec4( 0.8f, 0.1f, 0.15f, 1.f );
	styleColors[IGC_AxisYColor]				= ImVec4( 0.1f, 0.25f, 0.8f, 1.f );
	styleColors[IGC_AxisZColor]				= ImVec4( 0.2f, 0.7f, 0.2f, 1.f );
	styleColors[IGC_AxisXColor_Hovered]		= ImVec4( 0.9f, 0.2f, 0.25f, 1.f );
	styleColors[IGC_AxisYColor_Hovered]		= ImVec4( 0.2f, 0.35f, 0.9f, 1.f );
	styleColors[IGC_AxisZColor_Hovered]		= ImVec4( 0.3f, 0.8f, 0.3f, 1.f );
	styleColors[IGC_AxisXColor_Active]		= styleColors[IGC_AxisXColor];
	styleColors[IGC_AxisYColor_Active]		= styleColors[IGC_AxisYColor];
	styleColors[IGC_AxisZColor_Active]		= styleColors[IGC_AxisZColor];
}

/*
==================
CImGUIEngine::Tick
==================
*/
void CImGUIEngine::Tick( float InDeltaSeconds )
{
	for ( uint32 index = 0, count = layers.size(); index < count; ++index )
	{
		TSharedPtr<CImGUILayer>	layer = layers[index];
		WindowEvent			layerEvent;
		while ( layer->PollEvent( layerEvent ) )
		{
			layer->ProcessEvent( layerEvent );
		}
	}
}

/*
==================
CImGUIEngine::Shutdown
==================
*/
void CImGUIEngine::Shutdown()
{
	if ( !imguiContext )
	{
		return;
	}

	UNIQUE_RENDER_COMMAND( ÑShutdownImGUICommand,
		{
			g_RHI->ShutdownImGUI( g_RHI->GetImmediateContext() );
		} );

	Sys_ImGUIShutdown();
	ImGui::DestroyContext( imguiContext );

	imguiContext = nullptr;
}

/*
==================
CImGUIEngine::ProcessEvent
==================
*/
void CImGUIEngine::ProcessEvent( struct WindowEvent& InWindowEvent )
{
	// Process event by ImGUI
	Sys_ImGUIProcessEvent( InWindowEvent );

	WindowEvent		imguiEvent;
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

/*
==================
CImGUIEngine::BeginDraw
==================
*/
void CImGUIEngine::BeginDraw()
{
	Sys_ImGUIBeginDrawing();
	ImGui::NewFrame();
	ImGuizmo::BeginFrame();

	// Hide cursor if need
	if ( !bShowCursor )
	{
		ImGui::SetMouseCursor( ImGuiMouseCursor_None );
	}
}

/*
==================
CImGUIEngine::OpenWindow
==================
*/
void CImGUIEngine::OpenWindow( ImGuiViewport* InViewport )
{
	windows.push_back( new CImGUIWindow( InViewport ) );
}

/*
==================
CImGUIEngine::CloseWindow
==================
*/
void CImGUIEngine::CloseWindow( ImGuiViewport* InViewport )
{
	for ( uint32 index = 0, count = ( uint32 )windows.size(); index < count; ++index )
	{
		CImGUIWindow*		window = windows[ index ];
		if ( window->GetViewport() == InViewport )
		{			
			delete window;
			windows.erase( windows.begin() + index );
			return;
		}
	}
}

/*
==================
CImGUIEngine::EndDraw
==================
*/
void CImGUIEngine::EndDraw()
{
	// Draw all layers
	std::vector<TSharedPtr<CImGUILayer>>		layersOnTick = layers;
	for ( uint32 index = 0, count = layersOnTick.size(); index < count; ++index )
	{
		layersOnTick[index]->Tick();
	}

	// Draw debug window of the ImGUI if it need
#if !SHIPPING_BUILD
	if ( CVarImGUIDebug.GetValueBool() )
	{
		ImGui::ShowMetricsWindow();
	}
#endif // !SHIPPING_BUILD

	ImGui::Render();
	Sys_ImGUIEndDrawing();

	if ( ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable )
	{
		ImGui::UpdatePlatformWindows();
	}
	
	for ( uint32 index = 0, count = ( uint32 )windows.size(); index < count; ++index )
	{
		windows[ index ]->Tick();
	}

	// Unlock textures
	for ( auto it = lockedTextures.begin(), itEnd = lockedTextures.end(); it != itEnd; ++it )
	{
		( *it )->ReleaseRef();
	}
	lockedTextures.clear();
}

#endif // WITH_IMGUI