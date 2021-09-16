#include "Misc/WorldEdGlobals.h"
#include "Windows/WindowsManager.h"
#include "Windows/WindowImportTexture.h"
#include "Windows/WindowFileDialog.h"

WWindowImportTexture::WWindowImportTexture() :
	WBaseWindow( TEXT( "Import texture" ), ImGuiWindowFlags_NoCollapse )
{}

void WWindowImportTexture::Init()
{
	WBaseWindow::Init();
	//SetSize( FVector2D( 250, 250 ) );

	button_InputTexture = new WButton();
	button_InputTexture->SetName( TEXT( "..." ) );
	button_InputTexture->OnButtonClicked().Add( std::bind( &WWindowImportTexture::ButtonClicked_SelectInputTexture, this, std::placeholders::_1 ) );
	button_InputTexture->Init();

	inputText_InputTexture = new WInputText( TEXT( "Input:" ) );
	inputText_InputTexture->Init();

	sameLine_InputSection = new WSameLine();
	sameLine_InputSection->Init();
	sameLine_InputSection->AddWidget( inputText_InputTexture );
	sameLine_InputSection->AddWidget( button_InputTexture );
	AddWidget( sameLine_InputSection );

	button_OutputTexture = new WButton();
	button_OutputTexture->SetName( TEXT( "..." ) );
	button_OutputTexture->Init();

	inputText_OutputText = new WInputText( TEXT( "Output:" ) );
	inputText_OutputText->Init();

	sameLine_OutputSection = new WSameLine();
	sameLine_OutputSection->Init();
	sameLine_OutputSection->AddWidget( inputText_OutputText );
	sameLine_OutputSection->AddWidget( button_OutputTexture );
	AddWidget( sameLine_OutputSection );

	button_Cancel = new WButton();
	button_Cancel->SetName( TEXT( "Cancel" ) );
	button_Cancel->OnButtonClicked().Add( std::bind( &WWindowImportTexture::ButtonClicked_Cancel, this, std::placeholders::_1 ) );
	button_Cancel->Init();

	button_Convert = new WButton();
	button_Convert->SetName( TEXT( "Convert" ) );
	button_Convert->Init();

	sameLine_CommandSection = new WSameLine();
	sameLine_CommandSection->Init();
	sameLine_CommandSection->AddWidget( button_Cancel );
	sameLine_CommandSection->AddWidget( button_Convert );
	AddWidget( sameLine_CommandSection );
}

void WWindowImportTexture::ButtonClicked_SelectInputTexture( class WButton* InButton )
{
	WWindowFileDialog*		fileDialog = new WWindowFileDialog( TEXT( "Select texture for import" ) );
	fileDialog->Init();

	fileDialog->OnWindowOpened().Add( [&]( class WBaseWindow* InWindow )
										 {
											 check( InWindow );
											 SetEnable( false );
											 GWindowsManager->Add( InWindow );
										 } );
	fileDialog->OnWindowClosed().Add( [&]( class WBaseWindow* InWindow )
										 {
											 check( InWindow );
											 SetEnable( true );
											 GWindowsManager->Remove( InWindow );
										 } );
	fileDialog->Open();
}

void WWindowImportTexture::ButtonClicked_Cancel( class WButton* InButton )
{
	Close();
}