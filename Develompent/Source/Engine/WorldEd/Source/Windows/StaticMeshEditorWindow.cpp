#include "Containers/String.h"
#include "Containers/StringConv.h"
#include "Windows/StaticMeshEditorWindow.h"
#include "Render/StaticMeshPreviewViewportClient.h"
#include "Windows/FileDialog.h"
#include "Windows/MaterialEditorWindow.h"

CStaticMeshEditorWindow::CStaticMeshEditorWindow( const TSharedPtr<CStaticMesh>& InStaticMesh )
	: CImGUILayer( CString::Format( TEXT( "Static Mesh Editor - %s" ), InStaticMesh->GetAssetName().c_str() ) )
	, staticMesh( InStaticMesh )
	, viewportClient( new CStaticMeshPreviewViewportClient( InStaticMesh ) )
{
	// Init preview viewport
	viewportWidget.SetViewportClient( viewportClient, false );
	viewportWidget.SetEnabled( true );
	viewportWidget.Init();

	// Subscribe to event when assets try destroy of editing material and reload. It need is block
	assetsCanDeleteHandle = SEditorDelegates::onAssetsCanDelete.Add(	std::bind( &CStaticMeshEditorWindow::OnAssetsCanDelete, this, std::placeholders::_1, std::placeholders::_2	) );
	assetsReloadedHandle = SEditorDelegates::onAssetsReloaded.Add(		std::bind( &CStaticMeshEditorWindow::OnAssetsReloaded,	this, std::placeholders::_1							) );
}

CStaticMeshEditorWindow::~CStaticMeshEditorWindow()
{
	viewportWidget.SetViewportClient( nullptr, false );
	delete viewportClient;

	// Unsubscribe from event when assets try destroy and reload
	SEditorDelegates::onAssetsCanDelete.Remove( assetsCanDeleteHandle );
	SEditorDelegates::onAssetsReloaded.Remove( assetsReloadedHandle );
}

void CStaticMeshEditorWindow::Init()
{
	CImGUILayer::Init();
	SetSize( Vector2D( 700.f, 450.f ) );

	for ( uint32 index = 0, count = staticMesh->GetNumMaterials(); index < count; ++index )
	{
		TSharedPtr<CSelectAssetWidget>	selectAssetWidget = MakeSharedPtr<CSelectAssetWidget>( index );

		selectAssetWidget->Init();
		selectAssetWidget->SetLabel( "Slot " + std::to_string( index ) );
		selectAssetWidget->OnSelectedAsset().Add(	std::bind( &CStaticMeshEditorWindow::OnSelectedAsset,	this, std::placeholders::_1, std::placeholders::_2	) );
		selectAssetWidget->OnOpenAssetEditor().Add( std::bind( &CStaticMeshEditorWindow::OnOpenAssetEditor, this, std::placeholders::_1							) );
		selectAssetWidgets.push_back( SSelectAssetHandle{ index, nullptr, selectAssetWidget } );
	}
	UpdateAssetInfo();
}

void CStaticMeshEditorWindow::UpdateAssetInfo()
{
	for ( uint32 index = 0, count = selectAssetWidgets.size(); index < count; ++index )
	{
		SSelectAssetHandle&			selectAssetHandle	= selectAssetWidgets[index];
		TAssetHandle<CMaterial>		materialRef			= staticMesh->GetMaterial( index );
		if ( materialRef.IsAssetValid() && !GPackageManager->IsDefaultAsset( materialRef ) )
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

void CStaticMeshEditorWindow::OnTick()
{
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
		ImGui::Text( TCHAR_TO_ANSI( CString::Format( TEXT( "%.2f Kb" ), ( staticMesh->GetVerteces().Num() * sizeof( SStaticMeshVertexType ) + staticMesh->GetIndeces().Num() * sizeof( uint32 ) ) / 1024.f ).c_str() ) );
		ImGui::EndTable();
	}

	// Materials
	if ( ImGui::CollapsingHeader( "Materials", ImGuiTreeNodeFlags_DefaultOpen ) )
	{
		float		heightMaterialsSection	= 80.f * ImGui::GetContentRegionAvail().y / 100.f;		// 80% from max height of content region
		ImGui::BeginChild( "##MaterialSlots", ImVec2( 0, heightMaterialsSection ) );
		for ( uint32 index = 0, count = selectAssetWidgets.size(); index < count; ++index )
		{
			const SSelectAssetHandle		selectAssetHandle = selectAssetWidgets[index];
			if ( selectAssetHandle.widget )
			{
				selectAssetHandle.widget->Tick();
				ImGui::NewLine();
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
				SOpenFileDialogResult	openFileDialogResult;

				// Init file dialog settings
				fileDialogSetup.SetMultiselection( false );
				fileDialogSetup.SetTitle( TEXT( "Select New Source File" ) );
				fileDialogSetup.SetDirectory( appGameDir() + PATH_SEPARATOR + TEXT( "Content/" ) );
				fileDialogSetup.AddFormat( GAssetFactory.GetImporterInfo( AT_StaticMesh ), ConvertAssetTypeToText( AT_StaticMesh ) );

				// Show open file dialog
				if ( appShowOpenFileDialog( fileDialogSetup, openFileDialogResult ) && !openFileDialogResult.files.empty() )
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

void CStaticMeshEditorWindow::OnVisibilityChanged( bool InNewVisibility )
{
	// If visibility of the window changed to FALSE, we destroy him
	if ( !InNewVisibility )
	{
		Destroy();
	}
}

void CStaticMeshEditorWindow::ProcessEvent( struct SWindowEvent& InWindowEvent )
{
	// Process event in viewport client
	if ( viewportWidget.IsHovered() )
	{
		viewportClient->ProcessEvent( InWindowEvent );
	}
}

void CStaticMeshEditorWindow::OnSelectedAsset( uint32 InAssetSlot, const std::wstring& InNewAssetReference )
{
	if ( !staticMesh )
	{
		return;
	}

	check( InAssetSlot < selectAssetWidgets.size() );
	SSelectAssetHandle&		selectAssetHandle = selectAssetWidgets[InAssetSlot];

	// If asset reference is valid, we find asset
	TAssetHandle<CMaterial>		newMaterialRef;
	if ( !InNewAssetReference.empty() )
	{
		newMaterialRef = GPackageManager->FindAsset( InNewAssetReference, AT_Material );
	}

	// If asset is not valid we clear asset reference
	if ( !newMaterialRef.IsAssetValid() || GPackageManager->IsDefaultAsset( newMaterialRef ) )
	{
		newMaterialRef = nullptr;
		selectAssetHandle.widget->SetAssetReference( TEXT( "" ), false );
	}

	selectAssetHandle.asset = newMaterialRef;
	staticMesh->SetMaterial( InAssetSlot, newMaterialRef );
}

void CStaticMeshEditorWindow::OnAssetsCanDelete( const std::vector<TSharedPtr<CAsset>>& InAssets, SCanDeleteAssetResult& OutResult )
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

void CStaticMeshEditorWindow::OnOpenAssetEditor( uint32 InAssetSlot )
{
	if ( !staticMesh )
	{
		return;
	}

	check( InAssetSlot < selectAssetWidgets.size() );
	const SSelectAssetHandle&	selectAssetHandle = selectAssetWidgets[InAssetSlot];

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