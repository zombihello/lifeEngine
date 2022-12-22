#include "Containers/String.h"
#include "Containers/StringConv.h"
#include "Misc/UIGlobals.h"
#include "Windows/AudioBankEditorWindow.h"
#include "Windows/FileDialog.h"
#include "Components/AudioComponent.h"
#include "Windows/DialogWindow.h"
#include "System/AssetsImport.h"

/** Table pathes to icons */
static const tchar* GAudioBankEditorIconPaths[] =
{
	TEXT( "Import.png" )		// IT_Import
};
static_assert( ARRAY_COUNT( GAudioBankEditorIconPaths ) == CAudioBankEditorWindow::IT_Num, "Need full init GAudioBankEditorIconPaths array" );

/** Macro size button in menu bar */
#define  AUDIOBANKEDITOR_MENUBAR_BUTTONSIZE		ImVec2( 16.f, 16.f )

CAudioBankEditorWindow::CAudioBankEditorWindow( const TSharedPtr<CAudioBank>& InAudioBank )
	: CImGUILayer( CString::Format( TEXT( "Audio Bank Editor - %s" ), InAudioBank->GetAssetName().c_str() ) )
	, audioBank( InAudioBank )
	, audioBankHandle( nullptr )
{
	flags |= ImGuiWindowFlags_MenuBar | LF_DestroyOnHide;

	// Subscribe to event when assets try destroy of editing audio bank and reload. It need is block
	assetsCanDeleteHandle = SEditorDelegates::onAssetsCanDelete.Add(	std::bind(	&CAudioBankEditorWindow::OnAssetsCanDelete, this, std::placeholders::_1, std::placeholders::_2	) );
	assetsReloadedHandle = SEditorDelegates::onAssetsReloaded.Add(		std::bind(	&CAudioBankEditorWindow::OnAssetsReloaded, this, std::placeholders::_1							) );

	// Create audio component
	audioComponent = new CAudioComponent();
	audioComponent->SetAudioBank( audioBank->GetAssetHandle() );
	audioComponent->SetUISound( true );
	audioComponent->SetStreamable( true );
}

CAudioBankEditorWindow::~CAudioBankEditorWindow()
{
	// Unsubscribe from event when assets try destroy and reload
	SEditorDelegates::onAssetsCanDelete.Remove( assetsCanDeleteHandle );
	SEditorDelegates::onAssetsReloaded.Remove( assetsReloadedHandle );

	// Delete audio component and close handle
	delete audioComponent;
	if ( audioBankHandle )
	{
		audioBank->CloseBank( audioBankHandle );
	}
}

void CAudioBankEditorWindow::Init()
{
	CImGUILayer::Init();
	SetSize( Vector2D( 365.f, 250.f ) );

	// Loading icons
	std::wstring			errorMsg;
	PackageRef_t			package = GPackageManager->LoadPackage( TEXT( "" ), true );
	check( package );

	for ( uint32 index = 0; index < IT_Num; ++index )
	{
		const std::wstring			assetName = CString::Format( TEXT( "AudioBankEditor_%X" ), index );
		TAssetHandle<CTexture2D>&	assetHandle = icons[index];
		assetHandle = package->Find( assetName );
		if ( !assetHandle.IsAssetValid() )
		{
			std::vector<TSharedPtr<CAsset>>		result;
			if ( !CTexture2DImporter::Import( appBaseDir() + TEXT( "Engine/Editor/Icons/" ) + GAudioBankEditorIconPaths[index], result, errorMsg ) )
			{
				LE_LOG( LT_Warning, LC_Editor, TEXT( "Fail to load audio bank editor icon '%s' for type 0x%X. Message: %s" ), GAudioBankEditorIconPaths[index], index, errorMsg.c_str() );
				assetHandle = GEngine->GetDefaultTexture();
			}
			else
			{
				TSharedPtr<CTexture2D>		texture2D = result[0];
				texture2D->SetAssetName( assetName );
				assetHandle = texture2D->GetAssetHandle();
				package->Add( assetHandle );
			}
		}
	}

	// Update asset info
	UpdateAssetInfo();
}

void CAudioBankEditorWindow::UpdateAssetInfo()
{
	// Close old audio bank handle
	if ( audioBankHandle )
	{
		audioBank->CloseBank( audioBankHandle );
	}

	// Open new audio bank handle
	audioBankHandle = audioBank->OpenBank( audioBankInfo );
}

void CAudioBankEditorWindow::OnTick()
{
	// Menu bar
	if ( ImGui::BeginMenuBar() )
	{
		// Button 'Import'
		{
			if ( ImGui::ImageButton( GImGUIEngine->LockTexture( icons[IT_Import].ToSharedPtr()->GetTexture2DRHI() ), AUDIOBANKEDITOR_MENUBAR_BUTTONSIZE ) )
			{
				std::wstring		errorMsg;
				if ( !GAssetFactory.Reimport( audioBank, errorMsg ) )
				{
					OpenPopup<CDialogWindow>( TEXT( "Error" ), CString::Format( TEXT( "The audio bank not reimported.\n\nMessage: %s" ), errorMsg.c_str() ), CDialogWindow::BT_Ok );
				}
			}
			if ( ImGui::IsItemHovered( ImGuiHoveredFlags_AllowWhenDisabled ) )
			{
				ImGui::SetTooltip( "Reimport Audio Bank" );
			}
		}
		ImGui::EndMenuBar();
	}

	// Info about audio bank
	if ( ImGui::BeginTable( "##InfoAudioBankTable", 2 ) )
	{
		// Draw audio format
		ImGui::TableNextColumn();
		ImGui::Text( "Format:" );
		ImGui::TableNextColumn();
		ImGui::Text( audioBankHandle ? TCHAR_TO_ANSI( appSampleFormatToText( audioBankInfo.format ).c_str() ) : "None" );
		ImGui::TableNextColumn();

		// Draw number channels in audio
		ImGui::Text( "Channels:" );
		ImGui::TableNextColumn();
		ImGui::Text( audioBankHandle ? std::to_string( audioBankInfo.numChannels ).c_str() : "0" );
		ImGui::TableNextColumn();

		// Draw audio rate
		ImGui::Text( "Rate:" );
		ImGui::TableNextColumn();
		ImGui::Text( audioBankHandle ? std::to_string( audioBankInfo.rate ).c_str() : "0" );
		ImGui::TableNextColumn();

		// Draw resource size
		ImGui::Text( "Resource Size:" );
		ImGui::TableNextColumn();
		ImGui::Text( audioBankHandle ? TCHAR_TO_ANSI( CString::Format( TEXT( "%.2f Kb" ), ( appGetNumSampleBytes( audioBankInfo.format ) * audioBankInfo.numSamples ) / 1024.f ).c_str() ) : "0 Kb" );
		ImGui::EndTable();
	}

	// File path
	ImGui::Spacing();
	if ( ImGui::CollapsingHeader( "File Path", ImGuiTreeNodeFlags_DefaultOpen ) )
	{
		if ( ImGui::BeginTable( "##FilePathTable", 3 ) )
		{
			// Source file
			ImGui::TableNextColumn();
			ImGui::Text( "Source File:" );
			ImGui::TableNextColumn();
			{
				const std::wstring&		originalSourceFile = audioBank->GetAssetSourceFile();
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
				fileDialogSetup.AddFormat( GAssetFactory.GetImporterInfo( AT_AudioBank ), ConvertAssetTypeToText( AT_AudioBank ) );

				// Show open file dialog
				if ( appShowOpenFileDialog( fileDialogSetup, openFileDialogResult ) && !openFileDialogResult.files.empty() )
				{
					audioBank->SetAssetSourceFile( openFileDialogResult.files[0] );
				}
			}

			// Clear source file path
			ImGui::SameLine();
			if ( ImGui::Button( "X" ) )
			{
				audioBank->SetAssetSourceFile( TEXT( "" ) );
			}
			ImGui::EndTable();
		}
	}

	// Buttons play, pause and stop
	ImGui::NewLine();
	ImGui::Separator();
	if ( ImGui::BeginTable( "##PreviewTable", 3 ) )
	{
		// Play sound
		ImGui::TableNextColumn();
		const ImVec2		buttonSize( ImGui::GetColumnWidth() , 0.f );
		if ( ImGui::Button( "Play", buttonSize ) )
		{
			audioComponent->Play();
		}

		// Pause sound
		ImGui::TableNextColumn();
		if ( ImGui::Button( "Pause", buttonSize ) )
		{
			audioComponent->Pause();
		}

		// Stop sound
		ImGui::TableNextColumn();
		if ( ImGui::Button( "Stop", buttonSize ) )
		{
			audioComponent->Stop();
		}


		ImGui::EndTable();
	}
}

void CAudioBankEditorWindow::OnAssetsCanDelete( const std::vector<TSharedPtr<CAsset>>& InAssets, SCanDeleteAssetResult& OutResult )
{
	// If in InAssets exist audio bank who is editing now - need is block
	for ( uint32 index = 0, count = InAssets.size(); index < count; ++index )
	{
		TSharedPtr<CAsset>		assetRef = InAssets[index];
		if ( assetRef == audioBank )
		{
			OutResult.Set( false );
			return;
		}
	}
}

void CAudioBankEditorWindow::OnAssetsReloaded( const std::vector<TSharedPtr<CAsset>>& InAssets )
{
	// If audio bank who is edition reloaded, we update UI
	for ( uint32 index = 0, count = InAssets.size(); index < count; ++index )
	{
		if ( InAssets[index] == audioBank )
		{
			UpdateAssetInfo();
			return;
		}
	}
}