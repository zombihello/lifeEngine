#include <qcheckbox.h>
#include <qformlayout.h>
#include <qfiledialog.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qscrollarea.h>

#include "ui_StaticMeshEditorWindow.h"
#include "Containers/StringConv.h"
#include "Misc/WorldEdGlobals.h"
#include "System/AssetsImport.h"
#include "Windows/StaticMeshEditorWindow.h"
#include "Widgets/SectionWidget.h"
#include "Render/StaticMeshPreviewViewportClient.h"
#include "Render/RenderingThread.h"
#include "Widgets/SelectAssetWidget.h"
#include "WorldEd.h"

#include <vector>
#include "Misc/SharedPointer.h"
#include "System/Package.h"

WeStaticMeshEditorWindow::WeStaticMeshEditorWindow( const TSharedPtr<CStaticMesh>& InStaticMesh, QWidget* InParent /* = nullptr */ )
	: QWidget( InParent )
	, bInit( false )
	, ui( new Ui::WeStaticMeshEditorWindow() )
	, staticMesh( InStaticMesh )
	, viewportClient( nullptr )
	, assetsCanDeleteHandle( nullptr )
	, assetsReloadedHandle( nullptr )
	, label_sourceFileValue( nullptr )
	, toolButton_sourceFile( nullptr )
	, toolButton_sourceFileRemove( nullptr )
{
	check( InStaticMesh );

	// Init Qt UI
	ui->setupUi( this );
	InitUI();

	// Init preview viewport
	viewportClient = new CStaticMeshPreviewViewportClient( InStaticMesh );
	ui->viewportPreview->SetViewportClient( viewportClient, false );
	ui->viewportPreview->SetEnabled( true );
	bInit = true;

	// Subscribe to event when assets try destroy of editing static mesh and reload. It need is block
	assetsCanDeleteHandle	= SEditorDelegates::onAssetsCanDelete.Add(	std::bind( &WeStaticMeshEditorWindow::OnAssetsCanDelete, this, std::placeholders::_1, std::placeholders::_2 )	);
	assetsReloadedHandle	= SEditorDelegates::onAssetsReloaded.Add(	std::bind( &WeStaticMeshEditorWindow::OnAssetsReloaded, this, std::placeholders::_1 )							);
}

void WeStaticMeshEditorWindow::InitUI()
{
	// Section of materials
	{
		WeSectionWidget*	materialsSection		= new WeSectionWidget( "Materials", 300, this );
		QVBoxLayout*		verticalLayout			= new QVBoxLayout( materialsSection );
		QScrollArea*		scrollArea				= new QScrollArea( materialsSection );
		QWidget*			scrollWidgetContents	= new QWidget( materialsSection );
		QGridLayout*		gridLayout				= new QGridLayout( scrollWidgetContents );
		
		verticalLayout->setContentsMargins( 0, 0, 0, 0 );
		verticalLayout->setSpacing( 0 );
		gridLayout->setContentsMargins( 3, 3, 3, 3 );
		gridLayout->setVerticalSpacing( 1 );
		verticalLayout->addWidget( scrollArea );

		scrollArea->setWidget( scrollWidgetContents );
		scrollArea->setWidgetResizable( true );
		scrollArea->setFrameShape( QFrame::NoFrame );

		selectAsset_materials.resize( staticMesh->GetNumMaterials() );
		for ( uint32 index = 0, numMaterials = staticMesh->GetNumMaterials(); index < numMaterials; ++index )
		{
			WeSelectAssetWidget*		selectAsset = new WeSelectAssetWidget( index, materialsSection );
			gridLayout->addWidget( new QLabel( QString::number( index+1 ) + ":", materialsSection ), index, 0 );
			gridLayout->addWidget( selectAsset, index, 1 );
		
			// Add to array and connect to slot
			connect( selectAsset, SIGNAL( OnAssetReferenceChanged( uint32, const std::wstring& ) ), this, SLOT( OnSelectedAssetMaterial( uint32, const std::wstring& ) ) );
			selectAsset_materials[ index ] = selectAsset;
		}

		materialsSection->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Preferred );
		materialsSection->setContentLayout( *verticalLayout );
		materialsSection->expand( true );
		ui->frame->layout()->addWidget( materialsSection );
	}

	// Section of file path
	{
		WeSectionWidget*	filePathSection		= new WeSectionWidget( "File Path", 300, this );
		QGridLayout*		gridLayout			= new QGridLayout( filePathSection );
		QLabel*				label_sourceFile	= new QLabel( "Source File:", filePathSection );

		label_sourceFileValue					= new QLabel( "%Value%", filePathSection );
		toolButton_sourceFile					= new QToolButton( filePathSection );
		toolButton_sourceFileRemove				= new QToolButton( filePathSection );

		toolButton_sourceFile->setText( "..." );
		toolButton_sourceFileRemove->setText( "X" );
		gridLayout->setContentsMargins( 3, 3, 3, 3 );

		gridLayout->addWidget( label_sourceFile, 0, 0 );
		gridLayout->addWidget( label_sourceFileValue, 0, 1 );
		gridLayout->addWidget( toolButton_sourceFile, 0, 2 );
		gridLayout->addWidget( toolButton_sourceFileRemove, 0, 3 );

		connect( toolButton_sourceFile, SIGNAL( clicked() ), this, SLOT( on_toolButton_sourceFile_clicked() ) );
		connect( toolButton_sourceFileRemove, SIGNAL( clicked() ), this, SLOT( on_toolButton_sourceFileRemove_clicked() ) );

		filePathSection->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Preferred );
		filePathSection->setContentLayout( *gridLayout );
		filePathSection->expand( true );
		ui->frame->layout()->addWidget( filePathSection );
	}

	// Spacer
	{
		QSpacerItem*		verticalSpacer = new QSpacerItem( 20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding );
		ui->frame->layout()->addItem( verticalSpacer );
	}

	// Set icons for actions
	ui->actionReimport->setIcon( QIcon( TCHAR_TO_ANSI( ( appBaseDir() + TEXT( "Engine/Editor/Icons/Import.png" ) ).c_str() ) ) );

	// Update data in UI
	UpdateUI();
}

void WeStaticMeshEditorWindow::UpdateUI()
{
	// Update info about materials
	const std::vector< TAssetHandle<CMaterial> >&		materials = staticMesh->GetMaterials();
	for ( uint32 index = 0, count = materials.size(); index < count; ++index )
	{
		WeSelectAssetWidget*		selectAssetWidget = selectAsset_materials[ index ];
		check( selectAssetWidget );
	
		TAssetHandle<CMaterial>		material = materials[ index ];
		if ( material.IsAssetValid() && !GPackageManager->IsDefaultAsset( material ) )
		{
			std::wstring		assetReference;
			MakeReferenceToAsset( material, assetReference );
			selectAssetWidget->SetAssetReference( assetReference );
		}
		else
		{
			selectAssetWidget->ClearAssetReference();
		}
	}

	// Set info about asset
	const ÑBulkData<SStaticMeshVertexType>&		verteces	= staticMesh->GetVerteces();
	const ÑBulkData<uint32>&					indeces		= staticMesh->GetIndeces();
	
	ui->label_numVertecesValue->setText( QString::number( verteces.Num() ) );
	ui->label_numTrianglesValue->setText( QString::number( indeces.Num() / 3 ) );
	ui->label_resourceSizeValue->setText( QString::fromStdWString( CString::Format( TEXT( "%.2f Kb" ), ( verteces.Num() * sizeof( SStaticMeshVertexType ) + indeces.Num() * sizeof( uint32 ) ) / 1024.f ) ) );
	OnSourceFileChanged( QString::fromStdWString( staticMesh->GetAssetSourceFile() ) );
}

WeStaticMeshEditorWindow::~WeStaticMeshEditorWindow()
{
	FlushRenderingCommands();
	ui->viewportPreview->SetViewportClient( nullptr, false );
	delete viewportClient;
	delete ui;

	// Unsubscribe from event when assets try destroy and reload
	SEditorDelegates::onAssetsCanDelete.Remove( assetsCanDeleteHandle );
	SEditorDelegates::onAssetsReloaded.Remove( assetsReloadedHandle );
}

void WeStaticMeshEditorWindow::OnSourceFileChanged( QString InNewSourceFile )
{
	label_sourceFileValue->setText( InNewSourceFile );
	label_sourceFileValue->setToolTip( InNewSourceFile );
	ui->actionReimport->setEnabled( !InNewSourceFile.isEmpty() );
	UICropSourceFileText();
}

void WeStaticMeshEditorWindow::UICropSourceFileText()
{
	QFontMetrics		fontMetrices = label_sourceFileValue->fontMetrics();
	uint32				maxTextWidth = Max( label_sourceFileValue->size().width() - 2, 150 );
	label_sourceFileValue->setText( fontMetrices.elidedText( QString::fromStdWString( staticMesh->GetAssetSourceFile() ), Qt::TextElideMode::ElideRight, maxTextWidth ) );
}

void WeStaticMeshEditorWindow::resizeEvent( QResizeEvent* InEvent )
{
	UICropSourceFileText();
}

void WeStaticMeshEditorWindow::OnSelectedAssetMaterial( uint32 InAssetSlot, const std::wstring& InNewAssetReference )
{
	if ( !staticMesh )
	{
		return;
	}

	// If asset reference is valid, we find asset
	TAssetHandle<CMaterial>		newMaterialRef;
	if ( !InNewAssetReference.empty() )
	{
		newMaterialRef		= GPackageManager->FindAsset( InNewAssetReference, AT_Material );
	}

	// If asset is not valid we clear asset reference
	if ( !newMaterialRef.IsAssetValid() || GPackageManager->IsDefaultAsset( newMaterialRef ) )
	{
		newMaterialRef = nullptr;
		selectAsset_materials[ InAssetSlot ]->ClearAssetReference( false );
	}

	staticMesh->SetMaterial( InAssetSlot, newMaterialRef );
}

void WeStaticMeshEditorWindow::OnAssetsCanDelete( const std::vector< TSharedPtr<class CAsset> >& InAssets, SCanDeleteAssetResult& OutResult )
{
	CAsset::SetDependentAssets_t		dependentAssets;
	staticMesh->GetDependentAssets( dependentAssets );

	// If in InAssets exist static mesh who is editing now or him dependent assets - need is block
	for ( uint32 index = 0, count = InAssets.size(); index < count; ++index )
	{
		TSharedPtr<CAsset>		assetRef = InAssets[ index ];
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

void WeStaticMeshEditorWindow::OnAssetsReloaded( const std::vector< TSharedPtr<class CAsset> >& InAssets )
{
	// If static mesh who is edition reloaded, we update UI
	for ( uint32 index = 0, count = InAssets.size(); index < count; ++index )
	{
		if ( InAssets[ index ] == staticMesh )
		{
			UpdateUI();
			return;
		}
	}
}

void WeStaticMeshEditorWindow::on_toolButton_sourceFile_clicked()
{
	check( staticMesh );
	QString			newSourceFile = QFileDialog::getOpenFileName( this, "Select New Source File", QString(), CHelperAssetImporter::MakeFilterOfSupportedExtensions( CHelperAssetImporter::ET_StaticMesh ) );
	std::wstring	path = appQtAbsolutePathToEngine( newSourceFile );
	if ( newSourceFile.isEmpty() || staticMesh->GetAssetSourceFile() == path )
	{
		return;
	}

	staticMesh->SetAssetSourceFile( path );
	OnSourceFileChanged( newSourceFile );
	emit OnChangedAsset( staticMesh );
}

void WeStaticMeshEditorWindow::on_toolButton_sourceFileRemove_clicked()
{
	check( staticMesh );

	staticMesh->SetAssetSourceFile( TEXT( "" ) );
	OnSourceFileChanged( "" );
	emit OnChangedAsset( staticMesh );
}

void WeStaticMeshEditorWindow::on_actionReimport_triggered()
{
	check( staticMesh );

	std::wstring		errorMessage;
	if ( CHelperAssetImporter::Reimport( staticMesh->GetAssetHandle(), errorMessage ) )
	{
		emit OnChangedAsset( staticMesh );
	}
	else
	{
		QMessageBox::critical( this, "Error", QString::fromStdWString( CString::Format( TEXT( "Failed reimport asset '<b>%s</b>'<br><br>Error: %s" ), staticMesh->GetAssetName().c_str(), errorMessage.c_str() ) ) );
	}
}