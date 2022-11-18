#include "Misc/CoreGlobals.h"
#include "Misc/Misc.h"
#include "Containers/StringConv.h"
#include "Logger/LoggerMacros.h"
#include "System/BaseEngine.h"
#include "System/BaseFileSystem.h"
#include "System/AssetsImport.h"
#include "Windows/ContentBrowserWindow.h"
#include "ImGUI/imgui_internal.h"
#include "ImGUI/imgui_stdlib.h"

/** Border size for buttons in asset viewer */
#define CONTENTBROWSER_ASSET_BORDERSIZE		1.f

/** Selection colode */
#define CONTENTBROWSER_SELECTCOLOR			ImVec4( 0.f, 0.43f, 0.87f, 1.f )

/** Drag & drop type of a file node */
#define CONTENTBROWSER_DND_FILENODETYPE		"DND::FileNode"

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
{}

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

	// Create root nodes for engine and game directories
	engineRoot	= MakeSharedPtr<CFileTreeNode>( FNT_Folder, TEXT( "Engine" ), appBaseDir() + PATH_SEPARATOR TEXT( "Engine/Content/" ), this );
	gameRoot	= MakeSharedPtr<CFileTreeNode>( FNT_Folder, TEXT( "Game" ), appGameDir() + PATH_SEPARATOR TEXT( "Content/" ), this );
}

void CContentBrowserWindow::OnTick()
{
	ImGui::Columns( 2 );

	// Draw file system tree
	{
		ImGui::InputTextWithHint( "##FilterPackages", "Search", &filterInfo.fileName );
		ImGui::SameLine( 0, 0 );
		if ( ImGui::Button( "X##0" ) )
		{
			filterInfo.fileName.clear();
		}

		ImGui::BeginChild( "##Packages" );
		ImGui::PushStyleColor( ImGuiCol_Button, ImVec4( 0.f, 0.f, 0.f, 0.f ) );
		
		engineRoot->Tick();
		gameRoot->Tick();
		DrawPackagesPopupMenu();
		
		ImGui::PopStyleColor();	
		ImGui::EndChild();
		ImGui::NextColumn();
	}

	// Draw assets list
	{
		ImGui::InputTextWithHint( "##FilterAssets", "Search", &filterInfo.assetName );
		ImGui::SameLine( 0, 0 );
		if ( ImGui::Button( "X##1" ) )
		{
			filterInfo.assetName.clear();
		}

		ImGui::SameLine();
		if ( ImGui::BeginCombo( "##FilterAssetTypes", filterInfo.GetPreviewFilterAssetType().c_str() ) )
		{
			bool		bEnabledAllTypes = filterInfo.IsShowAllAssetTypes();
			if ( ImGui::Selectable( "All", &bEnabledAllTypes, ImGuiSelectableFlags_DontClosePopups ) )
			{
				filterInfo.SetShowAllAssetTypes( bEnabledAllTypes );
			}

			for ( uint32 index = AT_FirstType; index < AT_Count; ++index )
			{
				ImGui::Selectable( TCHAR_TO_ANSI( ConvertAssetTypeToText( ( EAssetType )index ).c_str() ), &filterInfo.assetTypes[index-1], ImGuiSelectableFlags_DontClosePopups );
			}
			ImGui::EndCombo();
		}
		ImGui::Separator();

		// Draw assets in the package
		if ( package )
		{
			// Section of assets
			ImGui::BeginChild( "##Assets" );
			float	panelWidth	= ImGui::GetContentRegionAvail().x;
			int32	columnCount = panelWidth / ( thumbnailSize + padding );
			if ( columnCount < 1 )
			{
				columnCount = 1;
			}

			ImGui::Columns( columnCount, 0, false );	
			for ( uint32 index = 0, count = assets.size(); index < count; ++index )
			{
				CAssetNode&		assetNode = assets[index];
				if ( filterInfo.assetTypes[assetNode.GetType() - 1] && CString::InString( assetNode.GetName(), ANSI_TO_TCHAR( filterInfo.assetName.c_str() ), true ) )
				{
					assetNode.Tick();
					ImGui::NextColumn();
				}
			}

			// Unselect all assets if clicked on empty space
			if ( ImGui::IsWindowHovered() && ( ImGui::IsMouseClicked( ImGuiMouseButton_Left ) || ( !ImGui::IsAnyItemHovered() && ImGui::IsMouseDown( ImGuiMouseButton_Right ) ) ) )
			{
				for ( uint32 index = 0, count = assets.size(); index < count; ++index )
				{
					assets[index].SetSelect( false );
				}
			}

			// Draw popup menu of assets
			DrawAssetsPopupMenu();
			ImGui::EndChild();
		}
	}

	ImGui::EndColumns();
}

void CContentBrowserWindow::DrawAssetsPopupMenu()
{
	if ( ImGui::BeginPopupContextWindow( "", ImGuiMouseButton_Right ) )
	{
		bool		bSelectedAssets = false;
		for ( uint32 index = 0, count = assets.size(); index < count; ++index )
		{
			bSelectedAssets |= assets[index].IsSelected();
			if ( bSelectedAssets )
			{
				break;
			}
		}

		if ( ImGui::BeginMenu( "Create" ) )
		{
			ImGui::MenuItem( "Material" );
			if ( ImGui::BeginMenu( "Physics" ) )
			{
				ImGui::MenuItem( "Physics Material" );
				ImGui::EndMenu();
			}		
			ImGui::EndMenu();
		}

		ImGui::Separator();
		ImGui::MenuItem( "Reload", "", nullptr, bSelectedAssets );
		ImGui::MenuItem( "Import" );
		ImGui::MenuItem( "Reimport", "", nullptr, bSelectedAssets );
		ImGui::MenuItem( "Reimport With New File", "", nullptr, bSelectedAssets );
		ImGui::Separator();
		ImGui::MenuItem( "Delete", "", nullptr, bSelectedAssets );
		ImGui::MenuItem( "Rename", "", nullptr, bSelectedAssets );
		ImGui::MenuItem( "Copy Reference", "", nullptr, bSelectedAssets );
		ImGui::EndPopup();
	}
}

void CContentBrowserWindow::DrawPackagesPopupMenu()
{
	if ( ImGui::BeginPopupContextWindow( "", ImGuiMouseButton_Right ) )
	{
		std::vector<TSharedPtr<CFileTreeNode>>		selectedNode;
		engineRoot->GetSelectedNodes( selectedNode );
		gameRoot->GetSelectedNodes( selectedNode );

		bool bSelectedPackages = !selectedNode.empty();
		ImGui::MenuItem( "Save", "", nullptr, bSelectedPackages );
		ImGui::MenuItem( "Open", "", nullptr, bSelectedPackages );
		ImGui::MenuItem( "Unload", "", nullptr, bSelectedPackages );
		ImGui::MenuItem( "Reload", "", nullptr, bSelectedPackages );
		ImGui::Separator();
		ImGui::MenuItem( "Show In Explorer", "", nullptr, bSelectedPackages );
		ImGui::Separator();

		if ( ImGui::BeginMenu( "Create" ) )
		{
			ImGui::MenuItem( "Folder" );
			ImGui::MenuItem( "Package" );
			ImGui::EndMenu();
		}

		ImGui::MenuItem( "Delete", "", nullptr, bSelectedPackages );
		ImGui::MenuItem( "Rename", "", nullptr, bSelectedPackages );
		ImGui::EndPopup();
	}
}

std::string CContentBrowserWindow::SFilterInfo::GetPreviewFilterAssetType() const
{
	if ( IsShowAllAssetTypes() )
	{
		return "All";
	}

	std::wstring	result;
	for ( uint32 index = AT_FirstType; index < AT_Count; ++index )
	{
		if ( assetTypes[index - 1] )
		{
			result += CString::Format( TEXT( "%s%s" ), result.empty() ? TEXT( "" ) : TEXT( ", " ), ConvertAssetTypeToText( ( EAssetType )index ).c_str() );
		}
	}

	return TCHAR_TO_ANSI( result.c_str() );
}

CContentBrowserWindow::CFileTreeNode::CFileTreeNode( EFileNodeType InType, const std::wstring& InName, const std::wstring& InPath, CContentBrowserWindow* InOwner )
	: bFreshed( false )
	, bSelected( false )
	, type( InType )
	, path( InPath )
	, name( InName )
	, owner( InOwner )
{
	check( owner );
}

void CContentBrowserWindow::CFileTreeNode::Tick()
{
	// Reset freshed flag in node
	bFreshed = false;

	// Refresh info about file system
	Refresh();

	// If after refreshing of the node flag is staying in FALSE - this is folder/file in FS not exist already
	if ( !bFreshed )
	{
		RemoveFromParent();
		return;
	}

	// Draw interface
	switch ( type )
	{
		// Draw folder node
	case FNT_Folder:
	{
		// Set style for selected node
		bool	bNeedPopStyleColor = false;
		if ( bSelected )
		{
			bNeedPopStyleColor = true;
			ImGui::PushStyleColor( ImGuiCol_Header,			CONTENTBROWSER_SELECTCOLOR );
			ImGui::PushStyleColor( ImGuiCol_HeaderActive,	CONTENTBROWSER_SELECTCOLOR );
			ImGui::PushStyleColor( ImGuiCol_HeaderHovered,	CONTENTBROWSER_SELECTCOLOR );
		}
		
		bool bTreeNode = ImGui::TreeNodeEx( TCHAR_TO_ANSI( name.c_str() ), ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_OpenOnArrow  | ImGuiTreeNodeFlags_OpenOnDoubleClick );

		// Drag n drop handle
		DragNDropHandle();

		// Item event handles
		ProcessEvents();

		// Pop selection node style
		if ( bNeedPopStyleColor )
		{
			ImGui::PopStyleColor( 3 );
		}

		// Draw tree
		if ( bTreeNode )
		{
			for ( uint32 index = 0, count = children.size(); index < count; ++index )
			{
				children[index]->Tick();
			}
			ImGui::TreePop();
		}
		break;
	}

		// Draw file node
	case FNT_File:
		// Set style for selected node
		bool	bNeedPopStyleColor = false;
		if ( bSelected )
		{
			bNeedPopStyleColor = true;
			ImGui::PushStyleColor( ImGuiCol_Button,			CONTENTBROWSER_SELECTCOLOR );
			ImGui::PushStyleColor( ImGuiCol_ButtonHovered,	CONTENTBROWSER_SELECTCOLOR );
			ImGui::PushStyleColor( ImGuiCol_ButtonActive,	CONTENTBROWSER_SELECTCOLOR );
		}

		ImGui::Button( TCHAR_TO_ANSI( name.c_str() ) );

		// Drag n drop handle
		DragNDropHandle();

		// Item event handles
		ProcessEvents();

		// Pop style color
		if ( bNeedPopStyleColor )
		{
			ImGui::PopStyleColor( 3 );
		}
		break;
	}
}

void CContentBrowserWindow::CFileTreeNode::ProcessEvents()
{
	const bool		bCtrlDown = GInputSystem->IsKeyDown( BC_KeyLControl ) || GInputSystem->IsKeyDown( BC_KeyRControl );
	if ( ImGui::IsItemHovered() )
	{
		// Select node if we press left mouse button
		if ( ImGui::IsMouseClicked( ImGuiMouseButton_Left ) )
		{
			if ( !bCtrlDown )
			{
				owner->engineRoot->SetSelect( false );
				owner->gameRoot->SetSelect( false );
			}

			switch ( type )
			{
			case FNT_Folder:
				SetSelect( !bSelected && bCtrlDown );
				break;

			case FNT_File:
				SetSelect( !bSelected );
				break;
			}
		}
		// Select node for popup menu if we press right mouse button
		else if ( ImGui::IsMouseDown( ImGuiMouseButton_Right ) )
		{
			if ( !bSelected )
			{
				owner->engineRoot->SetSelect( false );
				owner->gameRoot->SetSelect( false );
				SetSelect( true, false );
			}				
		}

		// If we double clicked by left mouse button, we must open package (if node is file)
		if ( type == FNT_File && !GInputSystem->IsKeyDown( BC_KeyLControl ) && !GInputSystem->IsKeyDown( BC_KeyRControl ) && ImGui::IsMouseDoubleClicked( ImGuiMouseButton_Left ) )
		{
			owner->package = GPackageManager->LoadPackage( path );
			check( owner->package );

			// Getting asset infos from current package
			owner->assets.clear();
			for ( uint32 index = 0, count = owner->package->GetNumAssets(); index < count; ++index )
			{
				SAssetInfo		assetInfo;
				owner->package->GetAssetInfo( index, assetInfo );
				owner->assets.push_back( CAssetNode( assetInfo, owner ) );
			}
		}
	}

	// Reset selection if clicked not on mode
	if ( ImGui::IsWindowHovered() && !ImGui::IsAnyItemHovered() && ( ImGui::IsMouseClicked( ImGuiMouseButton_Left ) || ImGui::IsMouseDown( ImGuiMouseButton_Right ) ) )
	{
		owner->assets.clear();
		owner->package = nullptr;
		owner->engineRoot->SetSelect( false );
		owner->gameRoot->SetSelect( false );
	}
}

void CContentBrowserWindow::CFileTreeNode::DragNDropHandle()
{
	// Begin drag n drop folder/package to other
	if ( ImGui::BeginDragDropSource() )
	{
		TSharedPtr<CFileTreeNode>*		ptrThis = new TSharedPtr<CFileTreeNode>();
		*ptrThis = AsShared();

		ImGui::SetDragDropPayload( CONTENTBROWSER_DND_FILENODETYPE, &ptrThis, sizeof( TSharedPtr<CFileTreeNode>* ), ImGuiCond_Once );
		ImGui::EndDragDropSource();
	}

	// Drag n drop target for packages/folders
	if ( ImGui::BeginDragDropTarget() )
	{
		const ImGuiPayload*		imguiPayload = ImGui::AcceptDragDropPayload( CONTENTBROWSER_DND_FILENODETYPE );
		if ( imguiPayload )
		{
			TSharedPtr<CFileTreeNode>*		pFileNode = ( *( TSharedPtr<CFileTreeNode>** )imguiPayload->Data );
			check( pFileNode );
			LE_LOG( LT_Log, LC_Dev, ( *pFileNode  )->GetPath().c_str() );
			delete pFileNode;
		}
		ImGui::EndDragDropTarget();
	}
}

void CContentBrowserWindow::CFileTreeNode::Refresh()
{
	// If node not exist in file system or need filter him, we mark him not freshed
	if ( ( type == FNT_File && !CString::InString( name, ANSI_TO_TCHAR( owner->filterInfo.fileName.c_str() ), true ) ) || !GFileSystem->IsExistFile( path, type == FNT_Folder ? true : false ) )
	{
		bFreshed = false;
		return;
	}

	std::vector<std::wstring>	files			= GFileSystem->FindFiles( path, true, true );
	for ( uint32 index = 0, count = files.size(); index < count; ++index )
	{
		std::wstring					file		= files[index];
		std::size_t						dotPos		= file.find_last_of( TEXT( "." ) );
		std::wstring					fullPath	= path + TEXT( "/" ) + file;
		TSharedPtr<CFileTreeNode>		childNode;

		// Add child node if this is directory
		if ( dotPos == std::wstring::npos )
		{
			childNode = FindChild( fullPath );
			if ( !childNode )
			{
				childNode = MakeSharedPtr<CFileTreeNode>( FNT_Folder, file, fullPath, owner );
				children.push_back( childNode );
			}

			childNode->Refresh();
			continue;
		}

		// If this package file we add new child node
		std::wstring		extension = file;
		extension.erase( 0, dotPos + 1 );
		if ( extension == TEXT( "pak" ) )
		{
			childNode = FindChild( fullPath );
			if ( !childNode )
			{
				childNode = MakeSharedPtr<CFileTreeNode>( FNT_File, file, fullPath, owner );
				children.push_back( childNode );
			}

			childNode->bFreshed = true;
		}
	}

	bFreshed = true;
}

void CContentBrowserWindow::CFileTreeNode::GetSelectedNodes( std::vector<TSharedPtr<CFileTreeNode>>& OutSelectedNodes, bool InIsIgnoreChildren /* = false */ ) const
{
	if ( bSelected )
	{
		OutSelectedNodes.push_back( AsShared() );
	}

	if ( !InIsIgnoreChildren )
	{
		for ( uint32 index = 0, count = children.size(); index < count; ++index )
		{
			children[index]->GetSelectedNodes( OutSelectedNodes, InIsIgnoreChildren );
		}
	}
}

void CContentBrowserWindow::CAssetNode::Tick()
{
	ImGui::PushStyleVar( ImGuiStyleVar_FrameBorderSize, CONTENTBROWSER_ASSET_BORDERSIZE );
	ImGui::PushStyleColor( ImGuiCol_Border, GAssetBorderColors[info.type] );

	// If asset is selected, we set him button color to CONTENTBROWSER_SELECTCOLOR
	bool		bNeedPopStyleColor = false;
	if ( bSelected )
	{
		bNeedPopStyleColor = true;
		ImGui::PushStyleColor( ImGuiCol_Button, CONTENTBROWSER_SELECTCOLOR );
		ImGui::PushStyleColor( ImGuiCol_ButtonHovered, CONTENTBROWSER_SELECTCOLOR );
		ImGui::PushStyleColor( ImGuiCol_ButtonActive, CONTENTBROWSER_SELECTCOLOR );
	}

	// Draw of the asset's button, if him's icon isn't existing we draw simple button
	const TAssetHandle<CTexture2D>&		assetIconTexture = owner->assetIcons[info.type];
	if ( assetIconTexture.IsAssetValid() )
	{
		ImGui::ImageButton( assetIconTexture.ToSharedPtr()->GetTexture2DRHI()->GetHandle(), { owner->thumbnailSize, owner->thumbnailSize } );
	}
	else
	{
		ImGui::Button( TCHAR_TO_ANSI( info.name.c_str() ), { owner->thumbnailSize, owner->thumbnailSize } );
	}

	// Process events
	ProcessEvents();

	ImGui::TextWrapped( TCHAR_TO_ANSI( info.name.c_str() ) );

	ImGui::PopStyleVar();
	ImGui::PopStyleColor( !bNeedPopStyleColor ? 1 : 4 );
}

void CContentBrowserWindow::CAssetNode::ProcessEvents()
{
	const bool		bCtrlDown = GInputSystem->IsKeyDown( BC_KeyLControl ) || GInputSystem->IsKeyDown( BC_KeyRControl );
	if ( ImGui::IsItemHovered() )
	{
		// Select asset if we press left mouse button
		if ( ImGui::IsMouseClicked( ImGuiMouseButton_Left ) )
		{
			if ( !bCtrlDown )
			{
				for ( uint32 index = 0, count = owner->assets.size(); index < count; ++index )
				{
					owner->assets[index].SetSelect( false );
				}
			}

			SetSelect( !bSelected );
		}
		// Select asset for popup menu if we press right mouse button
		else if ( ImGui::IsMouseDown( ImGuiMouseButton_Right ) )
		{
			if ( !bSelected )
			{
				for ( uint32 index = 0, count = owner->assets.size(); index < count; ++index )
				{
					owner->assets[index].SetSelect( false );
				}
				SetSelect( true );
			}
		}

		// If we double clicked by left mouse button, we must open editor for this type asset
		if ( ImGui::IsMouseDoubleClicked( ImGuiMouseButton_Left ) )
		{
			// TODO: Implement editor of the asset
			LE_LOG( LT_Warning, LC_Dev, TEXT( "CContentBrowserWindow::DrawAssets: Need implement editor of the asset" ) );
		}
	}

	// If pressed Ctrl+A we select all assets in the current package
	if ( ImGui::IsWindowHovered() && ( bCtrlDown && GInputSystem->IsKeyDown( BC_KeyA ) ) )
	{
		for ( uint32 index = 0, count = owner->assets.size(); index < count; ++index )
		{
			owner->assets[index].SetSelect( true );
		}
	}
}