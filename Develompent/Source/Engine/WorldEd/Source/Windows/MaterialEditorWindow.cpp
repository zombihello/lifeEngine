#include <qcheckbox.h>
#include <qformlayout.h>
#include <qcombobox.h>
#include <qlabel.h>

#include "ui_MaterialEditorWindow.h"
#include "Misc/WorldEdGlobals.h"
#include "System/AssetDataBase.h"
#include "Windows/MaterialEditorWindow.h"
#include "Widgets/SectionWidget.h"
#include "Render/MaterialPreviewViewportClient.h"
#include "Render/RenderingThread.h"
#include "Widgets/SelectAssetWidget.h"
#include "WorldEd.h"

#include <vector>
#include "Misc/SharedPointer.h"
#include "System/Package.h"

WeMaterialEditorWindow::WeMaterialEditorWindow( const TSharedPtr<FMaterial>& InMaterial, QWidget* InParent /* = nullptr */ )
	: QWidget( InParent )
	, bInit( false )
	, ui( new Ui::WeMaterialEditorWindow() )
	, material( InMaterial )
	, checkBox_isTwoSided( nullptr )
	, checkBox_isWireframe( nullptr )
	, checkBox_staticMesh( nullptr )
	, checkBox_sprite( nullptr )
	, selectAsset_diffuse( nullptr )
	, viewportClient( nullptr )
	, assetsCanDeleteHandle( nullptr )
{
	check( InMaterial );

	// Init Qt UI
	ui->setupUi( this );
	InitUI();

	// Init preview viewport
	viewportClient = new FMaterialPreviewViewportClient( InMaterial );
	ui->viewportPreview->SetViewportClient( viewportClient, false );
	ui->viewportPreview->SetEnabled( true );
	bInit = true;

	// Subscribe to event when assets try destroy of editing material. It need is block
	assetsCanDeleteHandle = FEditorDelegates::onAssetsCanDelete.Add( std::bind( &WeMaterialEditorWindow::OnAssetsCanDelete, this, std::placeholders::_1, std::placeholders::_2 ) );
}

void WeMaterialEditorWindow::InitUI()
{
	// Create general section
	{
		WeSectionWidget*	generalSection		= new WeSectionWidget( "General", 300, this );
		QVBoxLayout*		verticalLayout		= new QVBoxLayout();
		
		checkBox_isTwoSided		= new QCheckBox( "Is Two Sided", generalSection );
		checkBox_isWireframe	= new QCheckBox( "Is Wireframe", generalSection );

		verticalLayout->setContentsMargins( 0, 3, 0, 3 );
		verticalLayout->addWidget( checkBox_isTwoSided );
		verticalLayout->addWidget( checkBox_isWireframe );
		
		generalSection->setContentLayout( *verticalLayout );
		generalSection->expand( true );
		ui->frame->layout()->addWidget( generalSection );

		// Init states
		checkBox_isTwoSided->setChecked( material->IsTwoSided() );
		checkBox_isWireframe->setChecked( material->IsWireframe() );

		// Connect to slots
		connect( checkBox_isTwoSided, SIGNAL( toggled( bool ) ), this, SLOT( OnCheckBoxIsTwoSidedToggled( bool ) ) );
		connect( checkBox_isWireframe, SIGNAL( toggled( bool ) ), this, SLOT( OnCheckBoxIsWireframeToggled( bool ) ) );
	}

	// Create usage section
	{
		WeSectionWidget*	usageSection		= new WeSectionWidget( "Usage", 300, this );
		QVBoxLayout*		verticalLayout		= new QVBoxLayout();
		
		checkBox_staticMesh	= new QCheckBox( "Static Mesh", usageSection );
		checkBox_sprite		= new QCheckBox( "Sprite", usageSection );

		verticalLayout->setContentsMargins( 0, 3, 0, 3 );
		verticalLayout->addWidget( checkBox_staticMesh );
		verticalLayout->addWidget( checkBox_sprite );

		usageSection->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Preferred );
		usageSection->setContentLayout( *verticalLayout );
		usageSection->expand( true );
		ui->frame->layout()->addWidget( usageSection );

		// Init states
		checkBox_staticMesh->setChecked( material->GetUsageFlags() & MU_StaticMesh );
		checkBox_sprite->setChecked( material->GetUsageFlags() & MU_Sprite );

		// Connect to slots
		connect( checkBox_staticMesh, SIGNAL( toggled( bool ) ), this, SLOT( OnCheckBoxStaticMeshToggled( bool ) ) );
		connect( checkBox_sprite, SIGNAL( toggled( bool ) ), this, SLOT( OnCheckBoxSpriteToggled( bool ) ) );
	}

	// Create parameters section
	{
		WeSectionWidget*		parametersSection	= new WeSectionWidget( "Parameters", 300, this );
		QGridLayout*			gridLayout			= new QGridLayout();
		gridLayout->setContentsMargins( 0, 3, 0, 3 );

		selectAsset_diffuse		= new WeSelectAssetWidget( parametersSection );
		{
			TAssetHandle<FTexture2D>		diffuseTextureRef;
			material->GetTextureParameterValue( TEXT( "diffuse" ), diffuseTextureRef );
			if ( diffuseTextureRef.IsAssetValid() && !GPackageManager->IsDefaultAsset( diffuseTextureRef ) )
			{
				std::wstring		assetReference;
				MakeReferenceToAsset( diffuseTextureRef, assetReference );
				selectAsset_diffuse->SetAssetReference( assetReference );
			}
		}

		gridLayout->addWidget( new QLabel( "Diffuse:", parametersSection ), 0, 0 );
		gridLayout->addWidget( selectAsset_diffuse, 0, 1 );

		parametersSection->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Preferred );
		parametersSection->setContentLayout( *gridLayout );
		parametersSection->expand( true );
		ui->frame->layout()->addWidget( parametersSection );

		// Connect to slots
		connect( selectAsset_diffuse, SIGNAL( OnAssetReferenceChanged( const std::wstring& ) ), this, SLOT( OnSelectedAssetDiffuse( const std::wstring& ) ) );
	}

	// Create spacer
	{
		QSpacerItem*		verticalSpacer = new QSpacerItem( 20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding );
		ui->frame->layout()->addItem( verticalSpacer );
	}
}

WeMaterialEditorWindow::~WeMaterialEditorWindow()
{
	FlushRenderingCommands();
	ui->viewportPreview->SetViewportClient( nullptr, false );
	delete viewportClient;
	delete ui;

	// Unsubscribe from event when assets try destroy
	FEditorDelegates::onAssetsCanDelete.Remove( assetsCanDeleteHandle );
}

void WeMaterialEditorWindow::OnCheckBoxIsTwoSidedToggled( bool InValue )
{
	if ( !material )
	{
		return;
	}
	material->SetTwoSided( InValue );
}

void WeMaterialEditorWindow::OnCheckBoxIsWireframeToggled( bool InValue )
{
	if ( !material )
	{
		return;
	}
	material->SetWireframe( InValue );
}

void WeMaterialEditorWindow::OnCheckBoxStaticMeshToggled( bool InValue )
{
	if ( !material )
	{
		return;
	}
	material->UsageOnStaticMesh( InValue );
}

void WeMaterialEditorWindow::OnCheckBoxSpriteToggled( bool InValue )
{
	if ( !material )
	{
		return;
	}
	material->UsageOnSpriteMesh( InValue );
}

void WeMaterialEditorWindow::OnSelectedAssetDiffuse( const std::wstring& InNewAssetReference )
{
	if ( !material )
	{
		return;
	}

	// If asset reference is valid, we find asset
	TAssetHandle<FTexture2D>		newTexture2DRef;
	if ( !InNewAssetReference.empty() )
	{
		newTexture2DRef = GPackageManager->FindAsset( InNewAssetReference, AT_Texture2D );
	}

	// If asset is not valid we clear asset reference
	if ( !newTexture2DRef.IsAssetValid() || GPackageManager->IsDefaultAsset( newTexture2DRef ) )
	{
		newTexture2DRef = nullptr;
		selectAsset_diffuse->ClearAssetReference( false );
	}

	material->SetTextureParameterValue( TEXT( "diffuse" ), newTexture2DRef );
}

void WeMaterialEditorWindow::OnAssetsCanDelete( const std::vector< TSharedPtr<class FAsset> >& InAssets, FCanDeleteAssetResult& OutResult )
{
	FAsset::FSetDependentAssets		dependentAssets;
	material->GetDependentAssets( dependentAssets );

	// If in InAssets exist material who is editing now or him dependent assets - need is block
	for ( uint32 index = 0, count = InAssets.size(); index < count; ++index )
	{
		TSharedPtr<FAsset>		assetRef = InAssets[ index ];
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