#include "ImGUI/imgui.h"

#include "Core.h"
#include "Misc/EngineGlobals.h"
#include "RHI/BaseRHI.h"
#include "RHI/BaseDeviceContextRHI.h"
#include "Render/RenderingThread.h"
#include "ImGUI/ImGUIEngine.h"
#include "Misc/WorldEdGlobals.h"

FImGUIDrawData::FImGUIDrawData() :
	isFree( true )
{
	appMemzero( &drawData, sizeof( sizeof( ImDrawData ) ) );
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

		delete drawData.CmdLists;
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

/**
 * Constructor
 */
FImGUIEngine::FImGUIEngine() :
	imguiContext( nullptr ),
	indexCurrentBuffer( 0 )
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

	// Initialize platform for ImGUI
	check( appImGUIInit() );

	// Initialize RHI for ImGUI
	UNIQUE_RENDER_COMMAND( FInitImGUICommand,
		{
			GRHI->InitImGUI( GRHI->GetImmediateContext() );
		} );
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

/**
 * End draw commands for render ImGUI
 */
void FImGUIEngine::EndDraw()
{
	ImGui::Render();
	appImGUIEndDrawing();
	
	FImGUIDrawData*			currentBuffer = &drawDataBuffers[ indexCurrentBuffer ];
	while ( !currentBuffer->IsFree() )
	{
		for ( uint32 index = 0; index < IMGUI_DRAWBUFFERS_COUNT; ++index, indexCurrentBuffer = ++indexCurrentBuffer % IMGUI_DRAWBUFFERS_COUNT )
		{
			currentBuffer = &drawDataBuffers[indexCurrentBuffer];
			if ( currentBuffer->IsFree() )
			{
				break;
			}
		}
	}

	currentBuffer->SetDrawData( ImGui::GetDrawData() );
	indexCurrentBuffer = ++indexCurrentBuffer % IMGUI_DRAWBUFFERS_COUNT;

	UNIQUE_RENDER_COMMAND_ONEPARAMETER( FDrawImGUICommand, FImGUIDrawData*, imGuiDrawData, currentBuffer,
		{
			GRHI->DrawImGUI( GRHI->GetImmediateContext(), ( ImDrawData* )imGuiDrawData->GetDrawData() );
			imGuiDrawData->MarkFree();
		} );
}