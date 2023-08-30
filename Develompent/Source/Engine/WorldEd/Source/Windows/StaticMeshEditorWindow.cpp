#include "Containers/String.h"
#include "Containers/StringConv.h"
#include "Misc/UIGlobals.h"
#include "Misc/WorldEdGlobals.h"
#include "Windows/StaticMeshEditorWindow.h"
#include "Render/StaticMeshPreviewViewportClient.h"
#include "Windows/FileDialog.h"
#include "Windows/MaterialEditorWindow.h"
#include "windows/DialogWindow.h"
#include "System/AssetsImport.h"
#include "System/EditorEngine.h"

/** Macro size button in menu bar */
#define  STATICMESHEDITOR_MENUBAR_BUTTONSIZE	ImVec2( 16.f, 16.f )

/*
==================
CStaticMeshEditorWindow::CStaticMeshEditorWindow
==================
*/
CStaticMeshEditorWindow::CStaticMeshEditorWindow( const TSharedPtr<CStaticMesh>& InStaticMesh )
	: CImGUILayer( CString::Format( TEXT( "Static Mesh Editor - %s" ), InStaticMesh->GetAssetName().c_str() ) )
	, staticMesh( InStaticMesh )
	, viewportClient( new CStaticMeshPreviewViewportClient( InStaticMesh ) )
{
	flags |= ImGuiWindowFlags_MenuBar | LF_DestroyOnHide;

	// Init preview viewport
	viewportWidget.SetViewportClient( viewportClient, false );
	viewportWidget.SetEnabled( true );
	viewportWidget.Init();

	// Subscribe to event when assets try destroy of editing material and reload. It need is block
	assetsCanDeleteHandle = EditorDelegates::onAssetsCanDelete.Add(	std::bind( &CStaticMeshEditorWindow::OnAssetsCanDelete, this, std::placeholders::_1, std::placeholders::_2	) );
	assetsReloadedHandle = EditorDelegates::onAssetsReloaded.Add(		std::bind( &CStaticMeshEditorWindow::OnAssetsReloaded,	this, std::placeholders::_1							) );
}

/*
==================
CStaticMeshEditorWindow::~CStaticMeshEditorWindow
==================
*/
CStaticMeshEditorWindow::~CStaticMeshEditorWindow()
{
	viewportWidget.SetViewportClient( nullptr, false );
	delete viewportClient;

	// Unsubscribe from event when assets try destroy and reload
	EditorDelegates::onAssetsCanDelete.Remove( assetsCanDeleteHandle );
	EditorDelegates::onAssetsReloaded.Remove( assetsReloadedHandle );
}

/*
==================
CStaticMeshEditorWindow::Init
==================
*/
void CStaticMeshEditorWindow::Init()
{
	CImGUILayer::Init();
	SetSize( Vector2D( 700.f, 450.f ) );

	// Ini select asset widgets
	for ( uint32 index = 0, count = staticMesh->GetNumMaterials(); index < count; ++index )
	{
		TSharedPtr<CSelectAssetWidget>	selectAssetWidget = MakeSharedPtr<CSelectAssetWidget>( index );

		selectAssetWidget->Init();
		selectAssetWidget->SetLabel( CString::Format( TEXT( "Slot %i" ), index ) );
		selectAssetWidget->OnSelectedAsset().Add(	std::bind( &CStaticMeshEditorWindow::OnSelectedAsset,	this, std::placeholders::_1, std::placeholders::_2	) );
		selectAssetWidget->OnOpenAssetEditor().Add( std::bind( &CStaticMeshEditorWindow::OnOpenAssetEditor, this, std::placeholders::_1							) );
		selectAssetWidgets.push_back( SelectAssetHandle{ index, nullptr, selectAssetWidget } );
	}
	UpdateAssetInfo();
}

/*
==================
CStaticMeshEditorWindow::UpdateAssetInfo
==================
*/
void CStaticMeshEditorWindow::UpdateAssetInfo()
{
	for ( uint32 index = 0, count = selectAssetWidgets.size(); index < count; ++index )
	{
		SelectAssetHandle&			selectAssetHandle	= selectAssetWidgets[index];
		TAssetHandle<CMaterial>		materialRef			= staticMesh->GetMaterial( index );
		if ( materialRef.IsAssetValid() && !g_PackageManager->IsDefaultAsset( materialRef ) )
		{
			std::wstring	assetReference;
			MakeReferenceToAsset( materialRef, assetReference );
			selectAssetHandle.widget->SetAssetReference( assetReference, false );
		}
		else
		{
			materialRef.Reset();
			selectAssetHandle.widget->SetAssetReference( TEXT( "" ), false );
		}

		selectAssetHandle.asset = materialRef;
	}
}

/*
==================
CStaticMeshEditorWindow::OnTick
==================
*/
void CStaticMeshEditorWindow::OnTick()
{
	// Menu bar
	if ( ImGui::BeginMenuBar() )
	{
		// Button 'Import'
		{
			if ( ImGui::ImageButton( g_ImGUIEngine->LockTexture( g_EditorEngine->GetIcon( IT_Import ).ToSharedPtr()->GetTexture2DRHI() ), STATICMESHEDITOR_MENUBAR_BUTTONSIZE ) )
			{
				std::wstring		errorMsg;
				if ( !g_AssetFactory.Reimport( staticMesh, errorMsg ) )
				{
					OpenPopup<CDialogWindow>( TEXT( "Error" ), CString::Format( TEXT( "The static mesh not reimported.\n\nMessage: %s" ), errorMsg.c_str() ), CDialogWindow::BT_Ok );
				}
			}
			if ( ImGui::IsItemHovered( ImGuiHoveredFlags_AllowWhenDisabled ) )
			{
				ImGui::SetTooltip( "Reimport Static Mesh" );
			}
		}
		ImGui::EndMenuBar();
	}
	ImGui::Columns( 2 );

	// Static mesh preview
	viewportWidget.SetSize( Vector2D( ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y ) );
	viewportWidget.Tick();
	ImGui::NextColumn();

	// Info about static mesh
	if ( ImGui::BeginTable( "##InfoStaticMeshTable", 2 ) )
	{
		// Draw count vertices
		ImGui::TableNextColumn();
		ImGui::Text( "Vertices:" );
		ImGui::TableNextColumn();
		ImGui::Text( std::to_string( staticMesh->GetVerteces().Num() ).c_str() );
		ImGui::TableNextColumn();

		// Draw texture format
		ImGui::Text( "Triangles:" );
		ImGui::TableNextColumn();
		ImGui::Text( std::to_string( staticMesh->GetIndeces().Num() / 3 ).c_str() );
		ImGui::TableNextColumn();

		// Resource size
		ImGui::Text( "Resource Size:" );
		ImGui::TableNextColumn();
		ImGui::Text( TCHAR_TO_ANSI( CString::Format( TEXT( "%.2f Kb" ), ( staticMesh->GetVerteces().Num() * sizeof( StaticMeshVertexType ) + staticMesh->GetIndeces().Num() * sizeof( uint32 ) ) / 1024.f ).c_str() ) );
		ImGui::EndTable();
	}

	// Materials
	if ( ImGui::CollapsingHeader( "Materials", ImGuiTreeNodeFlags_DefaultOpen ) )
	{
		float		heightMaterialsSection	= 80.f * ImGui::GetContentRegionAvail().y / 100.f;		// 80% from max height of content region
		ImGui::BeginChild( "##MaterialSlots", ImVec2( 0, heightMaterialsSection ) );
		for ( uint32 index = 0, count = selectAssetWidgets.size(); index < count; ++index )
		{
			const SelectAssetHandle		selectAssetHandle = selectAssetWidgets[index];
			if ( selectAssetHandle.widget )
			{
				selectAssetHandle.widget->Tick();
			}
		}
		ImGui::EndChild();
	}

	// File path
	if ( ImGui::CollapsingHeader( "File Path", ImGuiTreeNodeFlags_DefaultOpen ) )
	{
		if ( ImGui::BeginTable( "##FilePathTable", 3 ) )
		{
			// Source file
			ImGui::TableNextColumn();
			ImGui::Text( "Source File:" );
			ImGui::TableNextColumn();
			{
				const std::wstring&		originalSourceFile = staticMesh->GetAssetSourceFile();
				if ( !originalSourceFile.empty() )
				{
					ImGui::Text( TCHAR_TO_ANSI( originalSourceFile.c_str() ) );
					if ( ImGui::IsItemHovered( ImGuiHoveredFlags_AllowWhenDisabled ) )
					{
						ImGui::SetTooltip( TCHAR_TO_ANSI( originalSourceFile.c_str() ) );
					}
				}
			}

			// Select new source file path
			ImGui::TableNextColumn();
			if ( ImGui::Button( "..." ) )
			{
				CFileDialogSetup		fileDialogSetup;
				OpenFileDialogResult	openFileDialogResult;

				// Init file dialog settings
				fileDialogSetup.SetMultiselection( false );
				fileDialogSetup.SetTitle( TEXT( "Select New Source File" ) );
				fileDialogSetup.SetDirectory( Sys_GameDir() + PATH_SEPARATOR + TEXT( "Content/" ) );
				fileDialogSetup.AddFormat( g_AssetFactory.GetImporterInfo( AT_StaticMesh ), ConvertAssetTypeToText( AT_StaticMesh ) );

				// Show open file dialog
				if ( Sys_ShowOpenFileDialog( fileDialogSetup, openFileDialogResult ) && !openFileDialogResult.files.empty() )
				{
					staticMesh->SetAssetSourceFile( openFileDialogResult.files[0] );
				}
			}

			// Clear source file path
			ImGui::SameLine();
			if ( ImGui::Button( "X" ) )
			{
				staticMesh->SetAssetSourceFile( TEXT( "" ) );
			}
			ImGui::EndTable();
		}
	}
}

/*
==================
CStaticMeshEditorWindow::ProcessEvent
==================
*/
void CStaticMeshEditorWindow::ProcessEvent( struct WindowEvent& InWindowEvent )
{
	// Process event in viewport client
	if ( viewportWidget.IsHovered() )
	{
		viewportClient->ProcessEvent( InWindowEvent );
	}
}

/*
==================
CStaticMeshEditorWindow::OnSelectedAsset
==================
*/
void CStaticMeshEditorWindow::OnSelectedAsset( uint32 InAssetSlot, const std::wstring& InNewAssetReference )
{
	if ( !staticMesh )
	{
		return;
	}

	Assert( InAssetSlot < selectAssetWidgets.size() );
	SelectAssetHandle&		selectAssetHandle = selectAssetWidgets[InAssetSlot];

	// If asset reference is valid, we find asset
	TAssetHandle<CMaterial>		newMaterialRef;
	if ( !InNewAssetReference.empty() )
	{
		newMaterialRef = g_PackageManager->FindAsset( InNewAssetReference, AT_Material );
	}

	// If asset is not valid we clear asset reference
	if ( !newMaterialRef.IsAssetValid() || g_PackageManager->IsDefaultAsset( newMaterialRef ) )
	{
		newMaterialRef = nullptr;
		selectAssetHandle.widget->SetAssetReference( TEXT( "" ), false );
	}

	selectAssetHandle.asset = newMaterialRef;
	staticMesh->SetMaterial( InAssetSlot, newMaterialRef );
}

/*
==================
CStaticMeshEditorWindow::OnAssetsCanDelete
==================
*/
void CStaticMeshEditorWindow::OnAssetsCanDelete( const std::vector<TSharedPtr<CAsset>>& InAssets, CanDeleteAssetResult& OutResult )
{
	CAsset::SetDependentAssets_t		dependentAssets;
	staticMesh->GetDependentAssets( dependentAssets );

	// If in InAssets exist static mesh who is editing now or him dependent assets - need is block
	for ( uint32 index = 0, count = InAssets.size(); index < count; ++index )
	{
		TSharedPtr<CAsset>		assetRef = InAssets[index];
		if ( assetRef == staticMesh )
		{
			OutResult.Set( false );
			return;
		}

		// Look in dependent assets
		for ( auto itDependentAsset = dependentAssets.begin(), itDependentAssetEnd = dependentAssets.end(); itDependentAsset != itDependentAssetEnd; ++itDependentAsset )
		{
			if ( *itDependentAsset == assetRef )
			{
				OutResult.Set( false );
				return;
			}
		}
	}
}

/*
==================
CStaticMeshEditorWindow::OnAssetsReloaded
==================
*/
void CStaticMeshEditorWindow::OnAssetsReloaded( const std::vector<TSharedPtr<CAsset>>& InAssets )
{
	// If static mesh who is edition reloaded, we update UI
	for ( uint32 index = 0, count = InAssets.size(); index < count; ++index )
	{
		if ( InAssets[index] == staticMesh )
		{
			UpdateAssetInfo();
			return;
		}
	}
}

/*
==================
CStaticMeshEditorWindow::OnOpenAssetEditor
==================
*/
void CStaticMeshEditorWindow::OnOpenAssetEditor( uint32 InAssetSlot )
{
	if ( !staticMesh )
	{
		return;
	}

	Assert( InAssetSlot < selectAssetWidgets.size() );
	const SelectAssetHandle&	selectAssetHandle = selectAssetWidgets[InAssetSlot];

	// Open material editor if asset is valid
	if ( selectAssetHandle.asset.IsAssetValid() )
	{
		TSharedPtr<CAsset>		asset = selectAssetHandle.asset.ToSharedPtr();
		if ( asset->GetType() == AT_Material )
		{
			MakeSharedPtr<CMaterialEditorWindow>( asset )->Init();
		}
	}
}