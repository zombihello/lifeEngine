#include "Logger/LoggerMacros.h"
#include "Misc/EngineGlobals.h"
#include "Misc/WorldEdGlobals.h"
#include "Misc/UIGlobals.h"
#include "System/BaseEngine.h"
#include "System/AssetsImport.h"
#include "System/EditorEngine.h"
#include "System/DragNDrop.h"
#include "Widgets/SelectAssetWidget.h"
#include "Windows/ContentBrowserWindow.h"
#include "ImGUI/ImGUIEngine.h"
#include "ImGUI/ImGUIExtension.h"

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
	bSelectedAsset = ImGui::SelectAsset( L_Sprintf( TEXT( "SelectAsset_%i" ), assetSlot ), label, assetReference, &bNeedOpenAssetEditor, previewTexture );
	
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