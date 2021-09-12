#include "Misc/WorldEdGlobals.h"
#include "Windows/WindowsManager.h"
#include "Windows/WindowImportTexture.h"

WWindowImportTexture::WWindowImportTexture() :
	WBaseWindow( TEXT( "Import texture" ), ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_AlwaysAutoResize )
{}

void WWindowImportTexture::Init()
{
	WBaseWindow::Init();

	button_exit = new WButton();
	button_exit->Init();
	button_exit->SetName( TEXT( "Exit" ) );
	button_exit->SetCallback( [&]() { Close(); } );
	AddWidget( TEXT( "Button_Exit" ), button_exit );

	text_hello = new WText( TEXT( "Hello biiiiiiiiiig teeext" ) );
	text_hello->Init();
	AddWidget( TEXT( "Text_Hello" ), text_hello );
}