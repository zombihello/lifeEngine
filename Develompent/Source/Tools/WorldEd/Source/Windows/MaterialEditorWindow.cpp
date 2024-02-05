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

#include "Core/Containers/String.h"
#include "Core/Containers/StringConv.h"
#include "Core/Logger/LoggerMacros.h"
#include "Engine/Render/RenderUtils.h"
#include "UI/Misc/UIGlobals.h"
#include "UI/ImGUI/ImGUIEngine.h"
#include "Windows/MaterialEditorWindow.h"
#include "Windows/TextureEditorWindow.h"
#include "Render/MaterialPreviewViewportClient.h"

/*
==================
CMaterialEditorWindow::CMaterialEditorWindow
==================
*/
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
	assetsCanDeleteHandle	= EditorDelegates::onAssetsCanDelete.Add( std::bind(	&CMaterialEditorWindow::OnAssetsCanDelete,	this, std::placeholders::_1, std::placeholders::_2	) );
	assetsReloadedHandle	= EditorDelegates::onAssetsReloaded.Add( std::bind(	&CMaterialEditorWindow::OnAssetsReloaded,	this, std::placeholders::_1							) );
}

/*
==================
CMaterialEditorWindow::~CMaterialEditorWindow
==================
*/
CMaterialEditorWindow::~CMaterialEditorWindow()
{
	viewportWidget.SetViewportClient( nullptr, false );
	delete viewportClient;

	// Unsubscribe from event when assets try destroy and reload
	EditorDelegates::onAssetsCanDelete.Remove( assetsCanDeleteHandle );
	EditorDelegates::onAssetsReloaded.Remove( assetsReloadedHandle );
}

/*
==================
CMaterialEditorWindow::Init
==================
*/
void CMaterialEditorWindow::Init()
{
	CImGUILayer::Init();
	SetSize( Vector2D( 700.f, 450.f ) );

	// Table of texture parameters in material
	static const CName s_TextureParameterNames[] =
	{
		CMaterial::albedoTextureParamName,
		CMaterial::normalTextureParamName,
		CMaterial::metallicTextureParamName,
		CMaterial::roughnessTextureParamName,
		CMaterial::emissionTextureParamName,
		CMaterial::aoTextureParamName
	};

	bIsDefaultMaterial = g_PackageManager->IsDefaultAsset( material->GetAssetHandle() );
	for ( uint32 index = 0; index < ARRAY_COUNT( s_TextureParameterNames ); ++index )
	{
		TSharedPtr<CSelectAssetWidget>	selectAssetWidget = MakeSharedPtr<CSelectAssetWidget>( index );
		
		selectAssetWidget->Init();
		selectAssetWidget->SetLabel( s_TextureParameterNames[index].ToString() );
		selectAssetWidget->OnSelectedAsset().Add( std::bind(	&CMaterialEditorWindow::OnSelectedAsset,	this, std::placeholders::_1, std::placeholders::_2	) );
		selectAssetWidget->OnOpenAssetEditor().Add( std::bind(	&CMaterialEditorWindow::OnOpenAssetEditor,	this, std::placeholders::_1							) );
		selectAssetWidgets.push_back( SelectAssetHandle{s_TextureParameterNames[index], nullptr, selectAssetWidget } );
	}
	UpdateAssetInfo();
}

/*
==================
CMaterialEditorWindow::UpdateAssetInfo
==================
*/
void CMaterialEditorWindow::UpdateAssetInfo()
{
	for ( uint32 index = 0, count = selectAssetWidgets.size(); index < count; ++index )
	{
		SelectAssetHandle&				selectAssetHandle = selectAssetWidgets[index];
		TAssetHandle<CTexture2D>		textureRef;
		
		material->GetTextureParameterValue( selectAssetHandle.parameterName, textureRef );
		if ( textureRef.IsAssetValid() && ( bIsDefaultMaterial || !g_PackageManager->IsDefaultAsset( textureRef ) ) )
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

/*
==================
CMaterialEditorWindow::OnTick
==================
*/
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
		if ( ImGui::Checkbox( "Is Two Sided", &bIsTwoSided ) )
		{
			material->SetTwoSided( bIsTwoSided );
		}

		// Is wireframe
		bool bIsWireframe		= material->IsWireframe();
		if ( ImGui::Checkbox( "Is Wireframe", &bIsWireframe ) )
		{
			material->SetWireframe( bIsWireframe );
		}

		// Is translucency
		bool bTranslucency		= material->IsTranslucency();
		if ( ImGui::Checkbox( "Is Translucency", &bTranslucency ) )
		{
			material->SetTranslucency( bTranslucency );
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
			const SelectAssetHandle		selectAssetHandle = selectAssetWidgets[index];
			if ( selectAssetHandle.widget )
			{
				// Update preview texture if it need
				Texture2DRHIRef_t	currentTexturePreview	= selectAssetHandle.widget->GetPreviewTexture();
				Texture2DRHIRef_t	newTexturePreview		= g_BlackTexture.GetTexture2DRHI();
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

/*
==================
CMaterialEditorWindow::OnSelectedAsset
==================
*/
void CMaterialEditorWindow::OnSelectedAsset( uint32 InAssetSlot, const std::wstring& InNewAssetReference )
{
	if ( !material )
	{
		return;
	}
	
	Assert( InAssetSlot < selectAssetWidgets.size() );
	SelectAssetHandle&		selectAssetHandle = selectAssetWidgets[InAssetSlot];

	// If asset reference is valid, we find asset
	TAssetHandle<CTexture2D>	newTexture2DRef;
	if ( !InNewAssetReference.empty() )
	{
		newTexture2DRef = g_PackageManager->FindAsset( InNewAssetReference, AT_Texture2D );
	}

	// If asset is not valid we clear asset reference
	if ( !newTexture2DRef.IsAssetValid() || ( !bIsDefaultMaterial && g_PackageManager->IsDefaultAsset( newTexture2DRef ) ) )
	{
		newTexture2DRef = nullptr;
		selectAssetHandle.widget->SetAssetReference( TEXT( "" ), false );
	}

	selectAssetHandle.asset = newTexture2DRef;
	material->SetTextureParameterValue( selectAssetHandle.parameterName, newTexture2DRef );
}

/*
==================
CMaterialEditorWindow::OnAssetsCanDelete
==================
*/
void CMaterialEditorWindow::OnAssetsCanDelete( const std::vector<TSharedPtr<CAsset>>& InAssets, CanDeleteAssetResult& OutResult )
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

/*
==================
CMaterialEditorWindow::OnAssetsReloaded
==================
*/
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

/*
==================
CMaterialEditorWindow::OnOpenAssetEditor
==================
*/
void CMaterialEditorWindow::OnOpenAssetEditor( uint32 InAssetSlot )
{
	if ( !material )
	{
		return;
	}

	Assert( InAssetSlot < selectAssetWidgets.size() );
	const SelectAssetHandle&		selectAssetHandle = selectAssetWidgets[InAssetSlot];

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