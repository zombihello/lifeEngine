#include "Logger/LoggerMacros.h"
#include "Misc/EngineGlobals.h"
#include "Misc/WorldEdGlobals.h"
#include "Misc/UIGlobals.h"
#include "System/BaseEngine.h"
#include "System/AssetsImport.h"
#include "System/EditorEngine.h"
#include "Widgets/SelectAssetWidget.h"
#include "Windows/ContentBrowserWindow.h"
#include "ImGUI/ImGUIEngine.h"
#include "ImGUI/imgui_stdlib.h"

/** Table pathes to icons */
static const tchar*			GSelectAssetWidgetIconPaths[] =
{
	TEXT( "CSW_PasteAsset.png" ),		// IT_Insert
	TEXT( "CSW_ShowAsset.png" ),		// IT_Browse
	TEXT( "CSW_RemoveAsset.png" )		// IT_Remove
};
static_assert( ARRAY_COUNT( GSelectAssetWidgetIconPaths ) == CSelectAssetWidget::IT_Num, "Need full init GSelectAssetWidgetIconPaths array" );

/** Macro size image buttons */
#define  SELECTWIDGETASSET_IMAGEBUTTONSIZE		ImVec2( 16.f, 16.f )

/** Macro size of asset preview */
#define  SELECTWIDGETASSET_ASSETPREVIEWSIZE		ImVec2( 64.f, 64.f )

CSelectAssetWidget::CSelectAssetWidget( uint32 InAssetSlot )
	: bInit( false )
	, assetSlot( InAssetSlot )
{}

void CSelectAssetWidget::Init()
{
	bInit = true;

	// Loading icons
	std::wstring			errorMsg;
	PackageRef_t			package = GPackageManager->LoadPackage( TEXT( "" ), true );
	check( package );

	for ( uint32 index = 0; index < IT_Num; ++index )
	{
		const std::wstring				assetName	= CString::Format( TEXT( "SelectAssetWidget_%X" ), index );
		TAssetHandle<CTexture2D>&		assetHandle = icons[index];
		assetHandle						= package->Find( assetName );
		if ( !assetHandle.IsAssetValid() )
		{
			TSharedPtr<CTexture2D>		texture2D = CTexture2DImporter::Import( appBaseDir() + TEXT( "Engine/Editor/Icons/" ) + GSelectAssetWidgetIconPaths[index], errorMsg );
			if ( !texture2D )
			{
				LE_LOG( LT_Warning, LC_Editor, TEXT( "Fail to load texture editor icon '%s' for type 0x%X. Message: %s" ), GSelectAssetWidgetIconPaths[index], index, errorMsg.c_str() );
				assetHandle = GEngine->GetDefaultTexture();
			}
			else
			{
				texture2D->SetAssetName( assetName );
				assetHandle = texture2D->GetAssetHandle();
				package->Add( assetHandle );
			}
		}
	}
}

void CSelectAssetWidget::SetAssetReference( const std::wstring& InAssetReference, bool InIsEmitEvent /* = true */ )
{
	assetReference = TCHAR_TO_ANSI( InAssetReference.c_str() );
	if ( InIsEmitEvent )
	{
		onSelectedAsset.Broadcast( assetSlot, InAssetReference );
	}
}

void CSelectAssetWidget::Tick()
{
	check( bInit );

	// Preview asset
	if ( previewTexture )
	{
		ImGui::PushStyleColor( ImGuiCol_Button, ImVec4( 0.f, 0.f, 0.f, 0.f ) );
		ImGui::PushStyleColor( ImGuiCol_ButtonHovered, ImVec4( 0.f, 0.f, 0.f, 0.f ) );
		ImGui::PushStyleColor( ImGuiCol_ButtonActive, ImVec4( 0.f, 0.f, 0.f, 0.f ) );
		
		if ( ImGui::ImageButton( GImGUIEngine->LockTexture( previewTexture ), SELECTWIDGETASSET_ASSETPREVIEWSIZE ) )
		{
			onOpenAssetEditor.Broadcast( assetSlot );
		}
		if ( !assetReference.empty() && ImGui::IsItemHovered( ImGuiHoveredFlags_AllowWhenDisabled ) )
		{
			ImGui::SetTooltip( "Open asset editor" );
		}
		
		ImGui::PopStyleColor( 3 );
	}
	ImGui::SameLine();

	// Control panel
	if ( ImGui::BeginTable( "##SelectWidgetControlPanel", 1 ) )
	{
		// Label
		ImGui::TableNextColumn();
		ImGui::Text( label.c_str() );
		ImGui::TableNextColumn();

		// Input asset reference
		ImGui::PushItemWidth( -10 );
		if ( ImGui::InputTextWithHint( ( "##AssetReference_" + std::to_string( assetSlot ) ).c_str(), "None", &assetReference, ImGuiInputTextFlags_EnterReturnsTrue ) )
		{
			onSelectedAsset.Broadcast( assetSlot, ANSI_TO_TCHAR( assetReference.c_str() ) );
		}
		ImGui::PopItemWidth();
		ImGui::TableNextColumn();

		// Insert button
		if ( ImGui::ImageButton( GImGUIEngine->LockTexture( icons[IT_Insert].ToSharedPtr()->GetTexture2DRHI() ), SELECTWIDGETASSET_IMAGEBUTTONSIZE ) )
		{
			SetAssetReference( GEditorEngine->GetContentBrowserWindow()->GetSelectedAssetReference() );
		}
		if ( ImGui::IsItemHovered( ImGuiHoveredFlags_AllowWhenDisabled ) )
		{
			ImGui::SetTooltip( "Use asset selected in content browser" );
		}

		// Browser button
		ImGui::SameLine();
		if ( ImGui::ImageButton( GImGUIEngine->LockTexture( icons[IT_Browse].ToSharedPtr()->GetTexture2DRHI() ), SELECTWIDGETASSET_IMAGEBUTTONSIZE ) )
		{
			GEditorEngine->GetContentBrowserWindow()->ShowAsset( ANSI_TO_TCHAR( assetReference.c_str() ) );
		}
		if ( ImGui::IsItemHovered( ImGuiHoveredFlags_AllowWhenDisabled ) )
		{
			ImGui::SetTooltip( "Show asset in content browser" );
		}

		// Remove button
		ImGui::SameLine();
		if ( ImGui::ImageButton( GImGUIEngine->LockTexture( icons[IT_Remove].ToSharedPtr()->GetTexture2DRHI() ), SELECTWIDGETASSET_IMAGEBUTTONSIZE ) )
		{
			SetAssetReference( TEXT( "" ) );
		}
		if ( ImGui::IsItemHovered( ImGuiHoveredFlags_AllowWhenDisabled ) )
		{
			ImGui::SetTooltip( "Remove reference to asset" );
		}

		ImGui::EndTable();
	}
}