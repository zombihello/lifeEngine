#include "Containers/String.h"
#include "Containers/StringConv.h"
#include "Logger/LoggerMacros.h"
#include "Misc/UIGlobals.h"
#include "Misc/WorldEdGlobals.h"
#include "Windows/TextureEditorWindow.h"
#include "Render/TexturePreviewViewportClient.h"
#include "Windows/FileDialog.h"
#include "System/AssetsImport.h"
#include "System/EditorEngine.h"
#include "Windows/DialogWindow.h"
#include "ImGUI/ImGUIEngine.h"

/** Table names of sampler address mode */
static const achar*		s_SamplerAddresModeNames[] =
{
	"Wrap",		// SAM_Wrap
	"Clamp",	// SAM_Clamp
	"Mirror",	// SAM_Mirror
	"Border"	// SAM_Border
};
static_assert( ARRAY_COUNT( s_SamplerAddresModeNames ) == SAM_Max, "Need full init s_SamplerAddresModeNames array" );

/** Table names of sampler filter */
static const achar*		s_SamplerFilterNames[] =
{
	"Point",				// SF_Point
	"Bilinear",				// SF_Bilinear
	"Trilinear",			// SF_Trilinear
	"Anisotropic Point",	// SF_AnisotropicPoint
	"Anisotropic Linear"	// SF_AnisotropicLinear
};
static_assert( ARRAY_COUNT( s_SamplerFilterNames ) == SF_Max, "Need full init s_SamplerFilterNames array" );

/** Macro size button in menu bar */
#define  TEXTUREEDITOR_MENUBAR_BUTTONSIZE	ImVec2( 16.f, 16.f )

/*
==================
CTextureEditorWindow::CTextureEditorWindow
==================
*/
CTextureEditorWindow::CTextureEditorWindow( const TSharedPtr<CTexture2D>& InTexture2D )
	: CImGUILayer( CString::Format( TEXT( "Texture Editor - %s" ), InTexture2D->GetAssetName().c_str() ) )
	, texture2D( InTexture2D )
	, viewportClient( new CTexturePreviewViewportClient( InTexture2D ) )
	, currentMipmap( 0 )
{
	flags |= ImGuiWindowFlags_MenuBar | LF_DestroyOnHide;

	// Init preview viewport
	viewportWidget.SetViewportClient( viewportClient, false );
	viewportWidget.SetEnabled( true );
	viewportWidget.Init();
}

/*
==================
CTextureEditorWindow::~CTextureEditorWindow
==================
*/
CTextureEditorWindow::~CTextureEditorWindow()
{
	viewportWidget.SetViewportClient( nullptr, false );
	FlushRenderingCommands();
	delete viewportClient;
}

/*
==================
CTextureEditorWindow::Init
==================
*/
void CTextureEditorWindow::Init()
{
	CImGUILayer::Init();
	SetSize( Vector2D( 700.f, 450.f ) );
}

/*
==================
CTextureEditorWindow::OnTick
==================
*/
void CTextureEditorWindow::OnTick()
{
	const SPixelFormatInfo&		pixelFormatInfo = g_PixelFormats[texture2D->GetPixelFormat()];
	uint32						sizeX = texture2D->GetSizeX();
	uint32						sizeY = texture2D->GetSizeY();
	uint32						numMips	= texture2D->GetNumMips();

	// Menu bar
	if ( ImGui::BeginMenuBar() )
	{
		// Button 'Import'
		if ( ImGui::ImageButton( g_ImGUIEngine->LockTexture( g_EditorEngine->GetIcon( IT_Import ).ToSharedPtr()->GetTexture2DRHI() ), TEXTUREEDITOR_MENUBAR_BUTTONSIZE ) )
		{
			std::wstring		errorMsg;
			if ( !g_AssetFactory.Reimport( texture2D, errorMsg ) )
			{
				OpenPopup<CDialogWindow>( TEXT( "Error" ), CString::Format( TEXT( "The texture 2D not reimported.\n\nMessage: %s" ), errorMsg.c_str() ), CDialogWindow::BT_Ok );
			}
		}
		if ( ImGui::IsItemHovered( ImGuiHoveredFlags_AllowWhenDisabled ) )
		{
			ImGui::SetTooltip( "Reimport Texture" );
		}

		// Button 'R'
		if ( ImGui::ImageButton( g_ImGUIEngine->LockTexture( g_EditorEngine->GetIcon( IT_Color_R ).ToSharedPtr()->GetTexture2DRHI() ), viewportClient->IsShowRedChannel(), TEXTUREEDITOR_MENUBAR_BUTTONSIZE ) )
		{
			viewportClient->ShowRedChannel( !viewportClient->IsShowRedChannel() );
		}
		if ( ImGui::IsItemHovered( ImGuiHoveredFlags_AllowWhenDisabled ) )
		{
			ImGui::SetTooltip( "Show R Channel" );
		}

		// Button 'G'
		if ( ImGui::ImageButton( g_ImGUIEngine->LockTexture( g_EditorEngine->GetIcon( IT_Color_G ).ToSharedPtr()->GetTexture2DRHI() ), viewportClient->IsShowGreenChannel(), TEXTUREEDITOR_MENUBAR_BUTTONSIZE ) )
		{
			viewportClient->ShowGreenChannel( !viewportClient->IsShowGreenChannel() );
		}
		if ( ImGui::IsItemHovered( ImGuiHoveredFlags_AllowWhenDisabled ) )
		{
			ImGui::SetTooltip( "Show G Channel" );
		}

		// Button 'B'
		if ( ImGui::ImageButton( g_ImGUIEngine->LockTexture( g_EditorEngine->GetIcon( IT_Color_B ).ToSharedPtr()->GetTexture2DRHI() ), viewportClient->IsShowBlueChannel(), TEXTUREEDITOR_MENUBAR_BUTTONSIZE ) )
		{
			viewportClient->ShowBlueChannel( !viewportClient->IsShowBlueChannel() );
		}
		if ( ImGui::IsItemHovered( ImGuiHoveredFlags_AllowWhenDisabled ) )
		{
			ImGui::SetTooltip( "Show B Channel" );
		}

		// Button 'A'
		if ( ImGui::ImageButton( g_ImGUIEngine->LockTexture( g_EditorEngine->GetIcon( IT_Color_A ).ToSharedPtr()->GetTexture2DRHI() ), viewportClient->IsShowAlphaChannel(), TEXTUREEDITOR_MENUBAR_BUTTONSIZE ) )
		{
			viewportClient->ShowAlphaChannel( !viewportClient->IsShowAlphaChannel() );
		}
		if ( ImGui::IsItemHovered( ImGuiHoveredFlags_AllowWhenDisabled ) )
		{
			ImGui::SetTooltip( "Show A Channel" );
		}

		// Select mipmap to view
		ImGui::PushItemWidth( 50 );
		ImGui::Text( "Mipmap: " );
		ImGui::SameLine();
		if ( ImGui::BeginCombo( "##MipmapCombo", TCHAR_TO_ANSI( CString::Format( TEXT( "%i" ), currentMipmap ).c_str() ) ) )
		{
			for ( uint32 index = 0, count = texture2D->GetNumMips(); index < count; ++index )
			{
				if ( ImGui::Selectable( TCHAR_TO_ANSI( CString::Format( TEXT( "%i" ), index ).c_str() ), currentMipmap == index, ImGuiSelectableFlags_DontClosePopups ) )
				{
					currentMipmap = index;
					viewportClient->SetMipmap( index );
				}
			}
			ImGui::EndCombo();
		}
		ImGui::PopItemWidth();

		ImGui::EndMenuBar();
	}
	ImGui::Columns( 2 );

	// Texture preview
	viewportWidget.SetSize( Vector2D( ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y ) );
	viewportWidget.Tick();
	ImGui::NextColumn();

	// Info and setting of texture
	if ( ImGui::BeginTable( "##InfoTextureTable", 2 ) )
	{
		// Draw size texture
		ImGui::TableNextColumn();
		ImGui::Text( "Imported:" );
		ImGui::TableNextColumn();
		ImGui::Text( TCHAR_TO_ANSI( CString::Format( TEXT( "%ix%i" ), sizeX, sizeY ).c_str() ) );
		ImGui::TableNextColumn();

		// Draw texture format
		ImGui::Text( "Format:" );
		ImGui::TableNextColumn();
		ImGui::Text( TCHAR_TO_ANSI( pixelFormatInfo.name ) );
		ImGui::TableNextColumn();

		// Number of mip levels
		ImGui::Text( "Num Mips:" );
		ImGui::TableNextColumn();
		ImGui::Text( TCHAR_TO_ANSI( CString::Format( TEXT( "%i" ), numMips ).c_str() ) );
		ImGui::TableNextColumn();

		// Resource size
		ImGui::Text( "Resource Size:" );
		ImGui::TableNextColumn();
		ImGui::Text( TCHAR_TO_ANSI( CString::Format( TEXT( "%.2f Kb" ), texture2D->GetMemorySize() / 1024.f ).c_str() ) );
		ImGui::EndTable();
	}

	// Sampler Settings
	ImGui::Spacing();
	if ( ImGui::CollapsingHeader( "Sampler Settings", ImGuiTreeNodeFlags_DefaultOpen ) )
	{
		if ( ImGui::BeginTable( "##SamplerSettingsTable", 2 ) )
		{
			// Address U
			ImGui::TableNextColumn();
			ImGui::Text( "Address U:" );
			ImGui::TableNextColumn();

			int32	samplerAddressMode = texture2D->GetAddressU();
			if ( ImGui::Combo( "##ComboAddressU", &samplerAddressMode, s_SamplerAddresModeNames, ARRAY_COUNT( s_SamplerAddresModeNames ) ) )
			{
				texture2D->SetAddressU( ( ESamplerAddressMode )samplerAddressMode );
			}
			ImGui::TableNextColumn();

			// Address V
			ImGui::Text( "Address V:" );
			ImGui::TableNextColumn();

			samplerAddressMode = texture2D->GetAddressV();
			if ( ImGui::Combo( "##ComboAddressV", &samplerAddressMode, s_SamplerAddresModeNames, ARRAY_COUNT( s_SamplerAddresModeNames ) ) )
			{
				texture2D->SetAddressV( ( ESamplerAddressMode )samplerAddressMode );
			}
			ImGui::TableNextColumn();

			// Filter
			ImGui::Text( "Filter:" );
			ImGui::TableNextColumn();

			int32	filterMode = texture2D->GetSamplerFilter();
			if ( ImGui::Combo( "##FilterMode", &filterMode, s_SamplerFilterNames, ARRAY_COUNT( s_SamplerFilterNames ) ) )
			{
				texture2D->SetSamplerFilter( ( ESamplerFilter )filterMode );
			}
			ImGui::EndTable();
		}
	}

	// Mipmap settings
	ImGui::Spacing();
	if ( ImGui::CollapsingHeader( "Mipmap", ImGuiTreeNodeFlags_DefaultOpen ) )
	{
		// Generate mipmaps
		if ( ImGui::Button( "Generate Mipmaps" ) )
		{
			texture2D->GenerateMipmaps();
		}

		// Remove mipmaps
		ImGui::SameLine();
		if ( ImGui::Button( "Remove Mipmaps" ) )
		{
			STexture2DMipMap		mipmap0 = texture2D->GetMip( 0 );
			texture2D->SetData( texture2D->GetPixelFormat(), mipmap0.sizeX, mipmap0.sizeY, mipmap0.data.GetStdContainer() );
			viewportClient->SetMipmap( 0 );
			currentMipmap = 0;
		}
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
				const std::wstring&		originalSourceFile = texture2D->GetAssetSourceFile();
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
				fileDialogSetup.SetDirectory( Sys_GameDir() + PATH_SEPARATOR + TEXT( "Content/" ) );
				fileDialogSetup.AddFormat( g_AssetFactory.GetImporterInfo( AT_Texture2D ), ConvertAssetTypeToText( AT_Texture2D ) );

				// Show open file dialog
				if ( Sys_ShowOpenFileDialog( fileDialogSetup, openFileDialogResult ) && !openFileDialogResult.files.empty() )
				{
					texture2D->SetAssetSourceFile( openFileDialogResult.files[0] );
				}
			}

			// Clear source file path
			ImGui::SameLine();
			if ( ImGui::Button( "X" ) )
			{
				texture2D->SetAssetSourceFile( TEXT( "" ) );
			}
			ImGui::EndTable();
		}
	}
}