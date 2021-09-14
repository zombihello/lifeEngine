#include "Misc/WorldEdGlobals.h"
#include "ImGUI/ImGUIEngine.h"
#include "Render/RenderingThread.h"
#include "Windows/WindowsManager.h"
#include "Core.h"
#include "WorldEd.h"

// Widgets
#include "Widgets/BaseWidget.h"
#include "Widgets/MainMenuBar.h"

// Windows
#include "Windows/WindowImportTexture.h"

FWorldEd::FWorldEd()
{}

FWorldEd::~FWorldEd()
{}

void FWorldEd::Init()
{
	GImGUIEngine->Init();
	InitUI();
}

void FWorldEd::InitUI()
{
	mainMenuBar.Init();
	
	// Add menu 'File'
	{
		WMainMenuBar::FMenu*		menu = new WMainMenuBar::FMenu( TEXT( "File" ) );
		menu->AddItem( new WMainMenuBar::FMenuItem( TEXT( "Exit" ), []() { GIsRequestingExit = true; } ) );
		mainMenuBar.AddMenu( menu );
	}

	// Add menu 'Tools'
	{
		WMainMenuBar::FMenu*		menu = new WMainMenuBar::FMenu( TEXT( "Tools" ) );
		menu->AddItem( new WMainMenuBar::FMenuItem( TEXT( "Import texture" ), []() 
					   { 
						   WWindowImportTexture*		importTexture = new WWindowImportTexture();
						   importTexture->Init();
						   GWindowsManager->Add( importTexture );
					   } ) );
		mainMenuBar.AddMenu( menu );
	}
}

void FWorldEd::ProcessEvent( struct SWindowEvent& InWindowEvent )
{
	GImGUIEngine->ProcessEvent( InWindowEvent );
}

void FWorldEd::Tick()
{
	GImGUIEngine->BeginDraw();
	mainMenuBar.Tick();

	GWindowsManager->Tick();
	ImGui::ShowDemoWindow();
	GImGUIEngine->EndDraw();
}

void FWorldEd::Shutdown()
{
	GImGUIEngine->Shutdown();
}