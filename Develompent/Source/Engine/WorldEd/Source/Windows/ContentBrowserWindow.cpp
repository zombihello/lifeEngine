#include "Misc/CoreGlobals.h"
#include "Misc/EngineGlobals.h"
#include "Misc/Misc.h"
#include "Containers/StringConv.h"
#include "Logger/LoggerMacros.h"
#include "System/BaseEngine.h"
#include "System/BaseFileSystem.h"
#include "System/InputSystem.h"
#include "System/AssetsImport.h"
#include "Windows/ContentBrowserWindow.h"
#include "ImGUI/imgui_internal.h"
#include "ImGUI/imgui_stdlib.h"

/** Border size for buttons in asset viewer */
#define CONTENTBROWSER_ASSET_BORDERSIZE		1.f

/** Color of a selected asset */
#define  CONTENTBROWSER_ASSET_SELECTCOLOR	ImVec4( 0.f, 0.43f, 0.87f, 1.f )

/** Table of color buttons by asset type */
static ImVec4		GAssetBorderColors[] =
{
	ImVec4( 1.f, 1.f, 1.f, 1.f ),				// AT_Unknown
	ImVec4( 0.75f, 0.25f, 0.25f, 1.f ),			// AT_Texture2D
	ImVec4( 0.25f, 0.75f, 0.25f, 1.f ),			// AT_Material
	ImVec4( 0.f, 0.f, 0.f, 0.f ),				// AT_Script
	ImVec4( 0.f, 1.f, 1.f, 1.f ),				// AT_StaticMesh
	ImVec4( 0.38f, 0.33f, 0.83f, 1.f ),			// AT_AudioBank
	ImVec4( 0.78f, 0.75f, 0.5f, 1.f )			// AT_PhysicsMaterial
};
static_assert( ARRAY_COUNT( GAssetBorderColors ) == AT_Count, "Need full init GAssetBorderColors array" );

/** Table of path to asset icons by type */
static const tchar* GAssetIconPaths[] =
{
	TEXT( "Unknown.png" ),				// AT_Unknown
	TEXT( "Texture.png" ),				// AT_Texture2D
	TEXT( "Material.png" ),				// AT_Material
	TEXT( "Script.png" ),				// AT_Script
	TEXT( "StaticMesh.png" ),			// AT_StaticMesh
	TEXT( "Audio.png" ),				// AT_AudioBank
	TEXT( "PhysMaterial.png" )			// AT_PhysicsMaterial
};
static_assert( ARRAY_COUNT( GAssetIconPaths ) == AT_Count, "Need full init GAssetIconPaths array" );

CContentBrowserWindow::CContentBrowserWindow( const std::wstring& InName )
	: CImGUILayer( InName )
	, padding( 16.f )
	, thumbnailSize( 64.f )
{
	memset( filterAssetType, 1, ARRAY_COUNT( filterAssetType ) * sizeof( bool ) );
}

void CContentBrowserWindow::Init()
{
	CImGUILayer::Init();

	// Loading all of asset icons
	std::wstring		errorMsg;
	for ( uint32 index = 0; index < AT_Count; ++index )
	{
		TSharedPtr<CTexture2D>		texture2D;
		texture2D = CTexture2DImporter::Import( appBaseDir() + TEXT( "Engine/Editor/Thumbnails/" ) + GAssetIconPaths[index], errorMsg );
		if ( !texture2D )
		{
			LE_LOG( LT_Warning, LC_Editor, TEXT( "Fail to load asset icon '%s' for type 0x%X" ), GAssetIconPaths[index], index );
			assetIcons[index] = GEngine->GetDefaultTexture();
		}
		else
		{
			TAssetHandle<CTexture2D>	assetHandle = texture2D->GetAssetHandle();
			PackageRef_t				package = GPackageManager->LoadPackage( TEXT( "" ), true );
			check( package );

			package->Add( assetHandle );
			assetIcons[index] = assetHandle;
		}
	}
}

void CContentBrowserWindow::OnTick()
{
	ImGui::Columns( 2 );
	bool	bCtrlDown = GInputSystem->IsKeyDown( BC_KeyLControl ) || GInputSystem->IsKeyDown( BC_KeyRControl );

	// Draw list of packages in file system
	ImGui::InputTextWithHint( "##FilterPackage", "Search", &filterPackage );
	ImGui::SameLine( 0, 0 );
	if ( ImGui::Button( "X##0" ) )
	{
		filterPackage.clear();
	}

	ImGui::BeginChild( "##Packages" );
	ImGui::PushStyleColor( ImGuiCol_Button, ImVec4( 0.f, 0.f, 0.f, 0.f ) );
	
	// Engine category packages
	if ( ImGui::TreeNode( "Engine" ) )
	{
		DrawPackageList( appBaseDir() + PATH_SEPARATOR TEXT( "Engine/Content/" ) );
		ImGui::TreePop();
	}

	// Game category packages
	if ( ImGui::TreeNode( "Game" ) )
	{
		DrawPackageList( appGameDir() + PATH_SEPARATOR TEXT( "Content/" ) );
		ImGui::TreePop();
	}

	ImGui::PopStyleColor();
	ImGui::EndChild();
	ImGui::NextColumn();

	// Draw assets in current package
	if ( package )
	{
		// Section of filter assets
		ImGui::InputTextWithHint( "##FilterAsset", "Search", &filterAsset );
		ImGui::SameLine( 0, 0 );
		if ( ImGui::Button( "X##1" ) )
		{
			filterAsset.clear();
		}

		ImGui::SameLine();
		ImGui::PushItemWidth( -1 );
		if ( ImGui::BeginCombo( "##FilterAssetTypes", GetPreviewFilterAssetType().c_str() ) )
		{
			bool		bEnabledAllTypes = IsShowAllAssetTypes();
			if ( ImGui::Selectable( "All", &bEnabledAllTypes, ImGuiSelectableFlags_DontClosePopups ) )
			{
				if ( bEnabledAllTypes )
				{
					memset( filterAssetType, 1, ARRAY_COUNT( filterAssetType ) * sizeof( bool ) );
				}
				else
				{
					memset( filterAssetType, 0, ARRAY_COUNT( filterAssetType ) * sizeof( bool ) );
				}
			}

			for ( uint32 index = AT_FirstType; index < AT_Count; ++index )
			{
				ImGui::Selectable( TCHAR_TO_ANSI( ConvertAssetTypeToText( ( EAssetType )index ).c_str() ), &filterAssetType[index-1], ImGuiSelectableFlags_DontClosePopups );
			}
			ImGui::EndCombo();
		}
		ImGui::PopItemWidth();
		ImGui::Separator();

		// Section of assets
		ImGui::BeginChild( "##Assets" );
		float	panelWidth		= ImGui::GetContentRegionAvail().x;
		int32	columnCount		= panelWidth / ( thumbnailSize + padding );
		if ( columnCount < 1 )
		{
			columnCount = 1;
		}
		ImGui::Columns( columnCount, 0, false );		
		
		SAssetInfo		assetInfo;
		for ( uint32 index = 0, count = package->GetNumAssets(); index < count; ++index )
		{
			package->GetAssetInfo( index, assetInfo );
			if ( filterAssetType[assetInfo.type-1] && CString::InString( assetInfo.name, ANSI_TO_TCHAR( filterAsset.c_str() ), true ) )
			{
				//ImGuiCol_ButtonHovered
				ImGui::PushStyleVar( ImGuiStyleVar_FrameBorderSize, CONTENTBROWSER_ASSET_BORDERSIZE );
				ImGui::PushStyleColor( ImGuiCol_Border, GAssetBorderColors[assetInfo.type] );	

				// If asset is selected, we set him button color to CONTENTBROWSER_ASSET_SELECTCOLOR
				bool	bSelectedAsset	= selectedAssets.find( assetInfo.name ) != selectedAssets.end();
				if ( bSelectedAsset )
				{
					ImGui::PushStyleColor( ImGuiCol_Button, CONTENTBROWSER_ASSET_SELECTCOLOR );
					ImGui::PushStyleColor( ImGuiCol_ButtonHovered, CONTENTBROWSER_ASSET_SELECTCOLOR );
					ImGui::PushStyleColor( ImGuiCol_ButtonActive, CONTENTBROWSER_ASSET_SELECTCOLOR );
				}

				// Draw of the asset's button, if him's icon isn't existing we draw simple button
				const TAssetHandle<CTexture2D>&		assetIconTexture = assetIcons[assetInfo.type];
				if ( assetIconTexture.IsAssetValid() )
				{
					ImGui::ImageButton( assetIconTexture.ToSharedPtr()->GetTexture2DRHI()->GetHandle(), { thumbnailSize, thumbnailSize } );
				}
				else
				{
					ImGui::Button( TCHAR_TO_ANSI( assetInfo.name.c_str() ), { thumbnailSize, thumbnailSize } );
				}
				
				// Item event handles
				if ( ImGui::IsItemHovered() )
				{				
					// If user clicked on asset
					if ( ImGui::IsMouseClicked( ImGuiMouseButton_Left ) )
					{
						if ( !bCtrlDown )
						{
							selectedAssets.clear();
						}

						if ( bSelectedAsset && bCtrlDown )
						{
							selectedAssets.erase( assetInfo.name );
						}
						else if ( !bSelectedAsset || bSelectedAsset && selectedAssets.empty() )
						{
							selectedAssets.insert( assetInfo.name );
						}
					}
					else if ( ImGui::IsMouseDoubleClicked( ImGuiMouseButton_Left ) )
					{
						// TODO: Implement editor of the asset
					}
					else if ( ImGui::IsMouseClicked( ImGuiMouseButton_Right ) )
					{
						ImGui::OpenPopup( "AssetContextMenu" );
					}
				}
				ImGui::PopStyleVar();
				ImGui::PopStyleColor( !bSelectedAsset ? 1 : 4 );

				ImGui::Text( TCHAR_TO_ANSI( assetInfo.name.c_str() ) );
				ImGui::NextColumn();
			}
		}

		// Window event handles
		if ( ImGui::IsWindowHovered() )
		{
			// If pressed Ctrl+A we select all assets in the current package
			if ( package && selectedAssets.size() != package->GetNumAssets() && bCtrlDown && GInputSystem->IsKeyDown( BC_KeyA ) )
			{
				selectedAssets.clear();
				SAssetInfo		assetInfo;

				for ( uint32 index = 0, count = package->GetNumAssets(); index < count; ++index )
				{
					package->GetAssetInfo( index, assetInfo );
					selectedAssets.insert( assetInfo.name );
				}
			}
			// Reset selection if clicked not on asset
			else if ( ImGui::IsMouseClicked( ImGuiMouseButton_Left ) )
			{
				selectedAssets.clear();
			}
		}

		if ( ImGui::BeginPopup( "AssetContextMenu" ) )
		{
			ImGui::MenuItem( "Reload" );
			ImGui::MenuItem( "Reimport" );
			ImGui::Separator();
			ImGui::MenuItem( "Delete" );
			ImGui::EndPopup();
		}

		ImGui::EndChild();
	}

	ImGui::EndColumns();
}

void CContentBrowserWindow::DrawPackageList( const std::wstring& InRootDir )
{
	std::vector<std::wstring>	files = GFileSystem->FindFiles( InRootDir, true, true );
	for ( uint32 index = 0, count = files.size(); index < count; ++index )
	{
		std::wstring		file		= files[index];
		std::size_t			dotPos		= file.find_last_of( TEXT( "." ) );
		std::wstring		fullPath	= InRootDir + TEXT( "/" ) + file;

		// Draw tree in sub directory
		if ( dotPos == std::wstring::npos )
		{
			if ( ImGui::TreeNode( TCHAR_TO_ANSI( file.c_str() ) ) )
			{
				DrawPackageList( fullPath );
				ImGui::TreePop();
			}
			continue;
		}

		std::wstring		extension = file;
		extension.erase( 0, dotPos + 1 );
		if ( extension == TEXT( "pak" ) && CString::InString( file, ANSI_TO_TCHAR( filterPackage.c_str() ), true ) )
		{
			ImGui::Button( TCHAR_TO_ANSI( file.c_str() ) );
			if ( ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked( ImGuiMouseButton_Left ) )
			{
				package = GPackageManager->LoadPackage( fullPath );
				check( package );
				selectedAssets.clear();
			}
		}
	}
}

std::string CContentBrowserWindow::GetPreviewFilterAssetType() const
{
	if ( IsShowAllAssetTypes() )
	{
		return "All";
	}

	std::wstring	result;
	for ( uint32 index = AT_FirstType; index < AT_Count; ++index )
	{
		if ( filterAssetType[index - 1] )
		{
			result += CString::Format( TEXT( "%s%s" ), result.empty() ? TEXT( "" ) : TEXT( ", " ), ConvertAssetTypeToText( ( EAssetType )index ).c_str() );
		}
	}

	return TCHAR_TO_ANSI( result.c_str() );
}