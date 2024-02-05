/**
 * ************************************************************
 *                  This file is part of:
 *                      LIFEENGINE
 *          https://github.com/zombihello/lifeEngine
 * ************************************************************
 * Copyright (C) 2024 Yehor Pohuliaka.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "Core/Containers/StringConv.h"
#include "Windows/DialogWindow.h"

/** Table of buttons name by flag */
static const tchar* s_ButtonsText[] =
{
	TEXT( "Ok" ),			// BT_Ok
	TEXT( "Cancel" ),		// BT_Cancel
	TEXT( "Yes" ),			// BT_Yes
	TEXT( "Yes To All" ),	// BT_YesToAll
	TEXT( "No" ),			// BT_No
	TEXT( "No To All" )		// BT_NoToAll
};


/*
==================
CDialogWindow::CDialogWindow
==================
*/
CDialogWindow::CDialogWindow( const std::wstring& InName, const std::wstring& InMessage, uint32 InButtons /* = BT_Ok */ )
	: CImGUIPopup( InName )
	, buttons( InButtons )
	, message( InMessage )
{
	SetSize( Vector2D( 500, 300 ) );
}

/*
==================
CDialogWindow::OnTick
==================
*/
void CDialogWindow::OnTick()
{
	// Draw text message
	ImGui::TextWrapped( TCHAR_TO_ANSI( message.c_str() ) );
	ImGui::Separator();

	// Draw buttons
	for ( uint32 index = 0, count = ARRAY_COUNT( s_ButtonsText ); index < count; ++index )
	{
		if ( !( buttons & 1 << index ) )
		{
			continue;
		}

		if ( index > 0 )
		{
			ImGui::SameLine();
		}

		if ( ImGui::Button( TCHAR_TO_ANSI( s_ButtonsText[index] ), ImVec2{ 120.f, 0.f } ) )
		{
			onButtonPressed.Broadcast( ( EButtonType )( 1 << index ) );
			Close();
		}	
	}
}