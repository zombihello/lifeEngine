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

#include "Core/Logger/LoggerMacros.h"
#include "UI/Misc/UIGlobals.h"
#include "UI/ImGUI/ImGUIEngine.h"
#include "UI/ImGUI/ImGUIExtension.h"
#include "UI/System/DragNDrop.h"
#include "Engine/Misc/EngineGlobals.h"
#include "Engine/System/BaseEngine.h"
#include "Editor/System/EditorEngine.h"
#include "Editor/System/AssetsImport.h"
#include "Widgets/SelectAssetWidget.h"
#include "Windows/ContentBrowserWindow.h"
#include "Misc/WorldEdGlobals.h"

/*
==================
CSelectAssetWidget::CSelectAssetWidget
==================
*/
CSelectAssetWidget::CSelectAssetWidget( uint32 InAssetSlot )
	: bInit( false )
	, bSelectedAsset( false )
	, assetSlot( InAssetSlot )
{}

/*
==================
CSelectAssetWidget::Init
==================
*/
void CSelectAssetWidget::Init()
{
	bInit = true;
}

/*
==================
CSelectAssetWidget::SetAssetReference
==================
*/
void CSelectAssetWidget::SetAssetReference( const std::wstring& InAssetReference, bool InIsEmitEvent /* = true */ )
{
	assetReference = InAssetReference;
	if ( InIsEmitEvent )
	{
		onSelectedAsset.Broadcast( assetSlot, InAssetReference );
	}
}

/*
==================
CSelectAssetWidget::Tick
==================
*/
void CSelectAssetWidget::Tick()
{
	Assert( bInit );
	
	bool			bNeedOpenAssetEditor = false;
	bSelectedAsset = ImGui::SelectAsset( CString::Format( TEXT( "SelectAsset_%i" ), assetSlot ), label, assetReference, &bNeedOpenAssetEditor, previewTexture );
	
	// If was selected new asset reference then we broadcast event about it
	if ( bSelectedAsset )
	{
		onSelectedAsset.Broadcast( assetSlot, assetReference );
	}

	// Open asset editor if user pressed on button preview texture
	if ( bNeedOpenAssetEditor )
	{
		onOpenAssetEditor.Broadcast( assetSlot );
	}
}