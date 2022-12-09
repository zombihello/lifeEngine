#include "Windows/TextureEditorWindow.h"

CTextureEditorWindow::CTextureEditorWindow( const TSharedPtr<CTexture2D>& InTexture2D, const std::wstring& InName )
	: CImGUILayer( InName )
	, texture2D( InTexture2D )
{}

void CTextureEditorWindow::OnTick()
{
	ImGui::Image( texture2D->GetTexture2DRHI()->GetHandle(), ImVec2( texture2D->GetSizeX(), texture2D->GetSizeY() ) );
}