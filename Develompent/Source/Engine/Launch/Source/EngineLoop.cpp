#include "Core.h"
#include "Misc/CoreGlobals.h"
#include "Misc/EngineGlobals.h"
#include "Misc/Misc.h"
#include "Logger/LoggerMacros.h"
#include "Logger/BaseLogger.h"
#include "System/BaseArchive.h"
#include "System/BaseFileSystem.h"
#include "System/BaseWindow.h"
#include "System/Config.h"
#include "System/ThreadingBase.h"
#include "Math/Color.h"
#include "Scripts/ScriptEngine.h"
#include "RHI/BaseRHI.h"
#include "RHI/BaseViewportRHI.h"
#include "RHI/BaseDeviceContextRHI.h"
#include "RHI/BaseSurfaceRHI.h"
#include "Render/Shaders/ShaderManager.h"
#include "EngineLoop.h"

#if WITH_EDITOR
	#include "Misc/WorldEdGlobals.h"	
	#include "ImGUI/ImGUIEngine.h"
	#include "ImGUI/imgui.h"
#endif // WITH_EDITOR

// --------------
// GLOBALS
// --------------

FBaseViewportRHI*			GViewportRHI = nullptr;

/**
 * Constructor
 */
FEngineLoop::FEngineLoop() :
	isInitialize( false )
{}

/**
 * Destructor
 */
FEngineLoop::~FEngineLoop()
{}

/**
 * Serialize configs
 */
void FEngineLoop::SerializeConfigs()
{
	// Loading engine config
	FBaseArchive*		arConfig = GFileSystem->CreateFileReader( TEXT( "../../Config/Engine.json" ) );
	if ( arConfig )
	{
		GEngineConfig.Serialize( *arConfig );
		delete arConfig;
	}

	// Loading game config
	arConfig = GFileSystem->CreateFileReader( TEXT( "../../Config/Game.json" ) );
	if ( arConfig )
	{
		GGameConfig.Serialize( *arConfig );
		delete arConfig;
	}

	// Loading editor config
#if WITH_EDITOR
	arConfig = GFileSystem->CreateFileReader( TEXT( "../../Config/Editor.json" ) );
	if ( arConfig )
	{
		GEditorConfig.Serialize( *arConfig );
		delete arConfig;
	}
#endif // WITH_EDITOR
}

/**
 * Pre-Initialize the main loop
 */
int32 FEngineLoop::PreInit( const tchar* InCmdLine )
{
	GGameThreadId = appGetCurrentThreadId();
	SerializeConfigs();

	GLog->Init();
	
	int32		result = appPlatformPreInit( InCmdLine );
	GScriptEngine->Init();

	return result;
}

#include "Render/Shaders/TestShader.h"
FVertexBufferRHIRef		vertexBuffer;
FBoundShaderStateRHIRef				boundShaderState;
FRasterizerStateRHIRef				rasterizerState;

#include "System/ThreadingBase.h"
#include "Render/RenderingThread.h"

/**
 * Initialize the main loop
 */
int32 FEngineLoop::Init( const tchar* InCmdLine )
{
	LE_LOG( LT_Log, LC_Init, TEXT( "Engine version: %i" ), ENGINE_VERSION );
	LE_LOG( LT_Log, LC_Init, TEXT( "Started with arguments: %s" ), InCmdLine );

	// Create window
	std::wstring				windowTitle = GGameConfig.GetValue( TEXT( "Game.GameInfo" ), TEXT( "Name" ) ).GetString();
	uint32						windowWidth = GEngineConfig.GetValue( TEXT( "Engine.SystemSettings" ), TEXT( "WindowWidth" ) ).GetInt();
	uint32						windowHeight = GEngineConfig.GetValue( TEXT( "Engine.SystemSettings" ), TEXT( "WindowHeight" ) ).GetInt();
	GWindow->Create( windowTitle.c_str(), windowWidth, windowHeight, SW_Default | SW_Hidden );
	
	// Create viewport for render
	uint32			width = 0;
	uint32			height = 0;
	GWindow->GetSize( width, height );

	GRHI->Init( false );
	GViewportRHI = GRHI->CreateViewport( GWindow->GetHandle(), width, height );

	int32		result = appPlatformInit( InCmdLine );
	StartRenderingThread();

	// -- Test section --
	FVertexDeclarationElementList		vertexDeclElementList;
	uint32								strideVertex = ( 3 * sizeof( float ) ) + ( 2 * sizeof( float ) );
	vertexDeclElementList.push_back( FVertexElement( 0, strideVertex, 0, VET_Float3, VEU_Position, 0 ) );
	vertexDeclElementList.push_back( FVertexElement( 0, strideVertex, 3 * sizeof(float), VET_Float2, VEU_TextureCoordinate, 0 ) );
	FVertexDeclarationRHIRef			vertexDeclRHI = GRHI->CreateVertexDeclaration( vertexDeclElementList );

	GShaderManager->Init();
	FShaderRef		testVertexShader = GShaderManager->FindInstance< FTestVertexShader >();
	FShaderRef		testPixelShader = GShaderManager->FindInstance< FTestPixelShader >();
	check( testVertexShader && testPixelShader );

	vertexBuffer = GRHI->CreateVertexBuffer( TEXT( "TestVertexBuffer" ), strideVertex * 3, nullptr, RUF_Dynamic );
	FLockedData				lockedData;
	GRHI->LockVertexBuffer( GRHI->GetImmediateContext(), vertexBuffer, strideVertex * 3, 0, lockedData );

	float tempData[] =
	{
		-0.5f, -0.5f, 0.0f,				0.0f, 0.0f,
		 0.5f, -0.5f, 0.0f,				1.0f, 0.0f,
		 0.0f,  0.5f, 0.0f,				0.5f, 1.0f
	};
	memcpy( lockedData.data, &tempData, strideVertex * 3 );

	GRHI->UnlockVertexBuffer( GRHI->GetImmediateContext(), vertexBuffer, lockedData );

	boundShaderState = GRHI->CreateBoundShaderState( TEXT( "TestBoundShaderState" ), vertexDeclRHI, testVertexShader->GetVertexShader(), testPixelShader->GetPixelShader() );

	FRasterizerStateInitializerRHI		rasterizerStateInitializerRHI;
	appMemzero( &rasterizerStateInitializerRHI, sizeof( FRasterizerStateInitializerRHI ) );

	rasterizerStateInitializerRHI.cullMode = CM_CW;
	rasterizerStateInitializerRHI.fillMode = FM_Solid;
	rasterizerState = GRHI->CreateRasterizerState( rasterizerStateInitializerRHI );

#if WITH_EDITOR
	GImGUIEngine->Init( GRHI->GetImmediateContext() );
#endif // WITH_EDITOR

	GWindow->Show();
	return result;
}

/**
 * Advances main loop
 */
void FEngineLoop::Tick()
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

#if WITH_EDITOR
			GImGUIEngine->ProcessEvent( windowEvent );
#endif // WITH_EDITOR
		}
	}

	FBaseDeviceContextRHI*		immediateContext = GRHI->GetImmediateContext();
	GRHI->BeginDrawingViewport( immediateContext, GViewportRHI );
	immediateContext->ClearSurface( GViewportRHI->GetSurface(), FColor::black );

	GRHI->SetStreamSource(immediateContext, 0, vertexBuffer, (3 * sizeof(float)) + (2 * sizeof(float)), 0);
	GRHI->SetBoundShaderState(immediateContext, boundShaderState);
	GRHI->SetRasterizerState(immediateContext, rasterizerState);
	GRHI->DrawPrimitive(immediateContext, PT_TriangleList, 0, 1);

#if WITH_EDITOR
	// This test drawing of ImGUI
	GImGUIEngine->BeginDrawing( immediateContext );
	ImGui::ShowDemoWindow();
	GImGUIEngine->EndDrawing( immediateContext );
#endif // WITH_EDITOR

	GRHI->EndDrawingViewport( immediateContext, GViewportRHI, true, false );
}

/**
 * Performs shut down
 */
void FEngineLoop::Exit()
{
#if WITH_EDITOR
	GImGUIEngine->Shutdown( GRHI->GetImmediateContext() );
#endif // WITH_EDITOR

	StopRenderingThread();

	delete GViewportRHI;
	GRHI->Destroy();

	GWindow->Close();
	GLog->TearDown();
}