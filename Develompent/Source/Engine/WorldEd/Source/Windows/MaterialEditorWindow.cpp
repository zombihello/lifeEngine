#include "Containers/String.h"
#include "Containers/StringConv.h"
#include "Logger/LoggerMacros.h"
#include "Misc/UIGlobals.h"
#include "Windows/MaterialEditorWindow.h"
#include "Windows/TextureEditorWindow.h"
#include "Render/MaterialPreviewViewportClient.h"
#include "ImGUI/ImGUIEngine.h"
#include "Render/RenderUtils.h"

/** Table of texture parameters in material */
static const CName		GTextureParameterNames[] =
{
	CMaterial::albedoTextureParamName,
	CMaterial::normalTextureParamName,
	CMaterial::metallicTextureParamName,
	CMaterial::roughnessTextureParamName,
	CMaterial::emissionTextureParamName,
	CMaterial::aoTextureParamName
};

CMaterialEditorWindow::CMaterialEditorWindow( const TSharedPtr<CMaterial>& InMaterial )
	: CImGUILayer( CString::Format( TEXT( "Material Editor - %s" ), InMaterial->GetAssetName().c_str() ) )
	, bIsDefaultMaterial( false )
	, material( InMaterial )
	, viewportClient( new CMaterialPreviewViewportClient( InMaterial ) )
{
	flags |= LF_DestroyOnHide;

	// Init preview viewport
	viewportWidget.SetViewportClient( viewportClient, false );
	viewportWidget.SetEnabled( true );
	viewportWidget.Init();

	// Subscribe to event when assets try destroy of editing material and reload. It need is block
	assetsCanDeleteHandle	= SEditorDelegates::onAssetsCanDelete.Add( std::bind(	&CMaterialEditorWindow::OnAssetsCanDelete,	this, std::placeholders::_1, std::placeholders::_2	) );
	assetsReloadedHandle	= SEditorDelegates::onAssetsReloaded.Add( std::bind(	&CMaterialEditorWindow::OnAssetsReloaded,	this, std::placeholders::_1							) );
}

CMaterialEditorWindow::~CMaterialEditorWindow()
{
	viewportWidget.SetViewportClient( nullptr, false );
	delete viewportClient;

	// Unsubscribe from event when assets try destroy and reload
	SEditorDelegates::onAssetsCanDelete.Remove( assetsCanDeleteHandle );
	SEditorDelegates::onAssetsReloaded.Remove( assetsReloadedHandle );
}

void CMaterialEditorWindow::Init()
{
	CImGUILayer::Init();
	SetSize( Vector2D( 700.f, 450.f ) );

	bIsDefaultMaterial = GPackageManager->IsDefaultAsset( material->GetAssetHandle() );
	for ( uint32 index = 0; index < ARRAY_COUNT( GTextureParameterNames ); ++index )
	{
		TSharedPtr<CSelectAssetWidget>	selectAssetWidget = MakeSharedPtr<CSelectAssetWidget>( index );
		
		selectAssetWidget->Init();
		selectAssetWidget->SetLabel( TCHAR_TO_ANSI( GTextureParameterNames[index].ToString().c_str() ) );
		selectAssetWidget->OnSelectedAsset().Add( std::bind(	&CMaterialEditorWindow::OnSelectedAsset,	this, std::placeholders::_1, std::placeholders::_2	) );
		selectAssetWidget->OnOpenAssetEditor().Add( std::bind(	&CMaterialEditorWindow::OnOpenAssetEditor,	this, std::placeholders::_1							) );
		selectAssetWidgets.push_back( SSelectAssetHandle{GTextureParameterNames[index], nullptr, selectAssetWidget } );
	}
	UpdateAssetInfo();
}

void CMaterialEditorWindow::UpdateAssetInfo()
{
	for ( uint32 index = 0, count = selectAssetWidgets.size(); index < count; ++index )
	{
		SSelectAssetHandle&				selectAssetHandle = selectAssetWidgets[index];
		TAssetHandle<CTexture2D>		textureRef;
		
		material->GetTextureParameterValue( selectAssetHandle.parameterName, textureRef );
		if ( textureRef.IsAssetValid() && ( bIsDefaultMaterial || !GPackageManager->IsDefaultAsset( textureRef ) ) )
		{
			std::wstring	assetReference;
			MakeReferenceToAsset( textureRef, assetReference );
			selectAssetHandle.widget->SetAssetReference( assetReference, false );
		}
		else if ( !bIsDefaultMaterial )
		{
			textureRef.Reset();
			selectAssetHandle.widget->SetAssetReference( TEXT( "" ), false );
		}

		selectAssetHandle.asset = textureRef;
	}
}

void CMaterialEditorWindow::OnTick()
{
	ImGui::Columns( 2 );

	// Material preview
	viewportWidget.SetSize( Vector2D( ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y ) );
	viewportWidget.Tick();
	ImGui::NextColumn();

	// General section
	if ( ImGui::CollapsingHeader( "General", ImGuiTreeNodeFlags_DefaultOpen ) )
	{
		// Only for editor
		bool bOnlyForEditor		= material->IsOnlyEditor();
		if ( ImGui::Checkbox( "Only For Editor", &bOnlyForEditor ) )
		{
			material->SetOnlyEditor( bOnlyForEditor );
		}
		
		// Is two sided
		bool bIsTwoSided		= material->IsTwoSided();
		if ( ImGui::Checkbox( "Is Two Sided", &bIsTwoSided ), &bIsTwoSided )
		{
			material->SetTwoSided( bIsTwoSided );
		}

		// Is wireframe
		bool bIsWireframe		= material->IsWireframe();
		if ( ImGui::Checkbox( "Is Wireframe", &bIsWireframe ), &bIsTwoSided )
		{
			material->SetWireframe( bIsWireframe );
		}
	}

	// Usage
	if ( ImGui::CollapsingHeader( "Usage", ImGuiTreeNodeFlags_DefaultOpen ) )
	{
		// Static mesh
		bool bUsageStaticMesh	= material->GetUsageFlags() & MU_StaticMesh;
		if ( ImGui::Checkbox( "Static Mesh", &bUsageStaticMesh ) )
		{
			material->UsageOnStaticMesh( bUsageStaticMesh );
		}

		// Sprite
		bool bUsageSprite		= material->GetUsageFlags() & MU_Sprite;
		if ( ImGui::Checkbox( "Sprite", &bUsageSprite ) )
		{
			material->UsageOnSpriteMesh( bUsageSprite );
		}
	}

	// Parameters
	if ( ImGui::CollapsingHeader( "Parameters", ImGuiTreeNodeFlags_DefaultOpen ) )
	{
		ImGui::BeginChild( "##Parameters" );
		for ( uint32 index = 0, count = selectAssetWidgets.size(); index < count; ++index )
		{
			const SSelectAssetHandle		selectAssetHandle = selectAssetWidgets[index];
			if ( selectAssetHandle.widget )
			{
				// Update preview texture if it need
				Texture2DRHIRef_t	currentTexturePreview	= selectAssetHandle.widget->GetPreviewTexture();
				Texture2DRHIRef_t	newTexturePreview		= GBlackTexture.GetTexture2DRHI();
				if ( selectAssetHandle.asset.IsAssetValid() )
				{
					newTexturePreview = selectAssetHandle.asset.ToSharedPtr()->GetTexture2DRHI();
				}

				if ( currentTexturePreview != newTexturePreview )
				{
					selectAssetHandle.widget->SetPreviewTexture( newTexturePreview );
				}

				// Now draw it!	
				selectAssetHandle.widget->Tick();
			}
		}
		ImGui::EndChild();
	}
}

void CMaterialEditorWindow::OnSelectedAsset( uint32 InAssetSlot, const std::wstring& InNewAssetReference )
{
	if ( !material )
	{
		return;
	}
	
	check( InAssetSlot < selectAssetWidgets.size() );
	SSelectAssetHandle&		selectAssetHandle = selectAssetWidgets[InAssetSlot];

	// If asset reference is valid, we find asset
	TAssetHandle<CTexture2D>	newTexture2DRef;
	if ( !InNewAssetReference.empty() )
	{
		newTexture2DRef = GPackageManager->FindAsset( InNewAssetReference, AT_Texture2D );
	}

	// If asset is not valid we clear asset reference
	if ( !newTexture2DRef.IsAssetValid() || ( !bIsDefaultMaterial && GPackageManager->IsDefaultAsset( newTexture2DRef ) ) )
	{
		newTexture2DRef = nullptr;
		selectAssetHandle.widget->SetAssetReference( TEXT( "" ), false );
	}

	selectAssetHandle.asset = newTexture2DRef;
	material->SetTextureParameterValue( selectAssetHandle.parameterName, newTexture2DRef );
}

void CMaterialEditorWindow::OnAssetsCanDelete( const std::vector<TSharedPtr<CAsset>>& InAssets, SCanDeleteAssetResult& OutResult )
{
	CAsset::SetDependentAssets_t		dependentAssets;
	material->GetDependentAssets( dependentAssets );

	// If in InAssets exist material who is editing now or him dependent assets - need is block
	for ( uint32 index = 0, count = InAssets.size(); index < count; ++index )
	{
		TSharedPtr<CAsset>		assetRef = InAssets[index];
		if ( assetRef == material )
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

void CMaterialEditorWindow::OnAssetsReloaded( const std::vector<TSharedPtr<CAsset>>& InAssets )
{
	// If material who is edition reloaded, we update UI
	for ( uint32 index = 0, count = InAssets.size(); index < count; ++index )
	{
		if ( InAssets[index] == material )
		{
			UpdateAssetInfo();
			return;
		}
	}
}

void CMaterialEditorWindow::OnOpenAssetEditor( uint32 InAssetSlot )
{
	if ( !material )
	{
		return;
	}

	check( InAssetSlot < selectAssetWidgets.size() );
	const SSelectAssetHandle&		selectAssetHandle = selectAssetWidgets[InAssetSlot];

	// Open texture editor if asset is valid
	if ( selectAssetHandle.asset.IsAssetValid() )
	{
		TSharedPtr<CAsset>		asset = selectAssetHandle.asset.ToSharedPtr();
		if ( asset->GetType() == AT_Texture2D )
		{
			MakeSharedPtr<CTextureEditorWindow>( asset )->Init();
		}
	}
}