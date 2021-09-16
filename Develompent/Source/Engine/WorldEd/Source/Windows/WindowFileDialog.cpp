#include "ImGui/ImGuiFileDialog.h"
#include "Misc/WorldEdGlobals.h"
#include "Windows/WindowsManager.h"
#include "Windows/WindowFileDialog.h"

WWindowFileDialog::WWindowFileDialog( const tchar* InTitle ) :
	WBaseWindow( InTitle, 0 )
{}

void WWindowFileDialog::Init()
{
	WBaseWindow::Init();
	ImGuiFileDialog::Instance()->OpenDialog( GetATitle(), GetATitle(), ".cpp,.h,.hpp", "." );
}

void WWindowFileDialog::Tick()
{
	if ( ImGuiFileDialog::Instance()->Display( GetATitle() ) )
	{
		// action if OK
		if ( ImGuiFileDialog::Instance()->IsOk() == true )
		{
			//std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
			//std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
			// action
		}
		// close
		ImGuiFileDialog::Instance()->Close();
		Close();
	}
}