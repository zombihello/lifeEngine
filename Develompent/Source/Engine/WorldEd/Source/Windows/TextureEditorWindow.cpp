#include <qmessagebox.h>
#include <qfiledialog.h>
#include <qformlayout.h>
#include <qlabel.h>
#include <qcombobox.h>

#include "ui_TextureEditorWindow.h"
#include "Containers/StringConv.h"
#include "System/AssetsImport.h"
#include "Render/TexturePreviewViewportClient.h"
#include "Render/RenderUtils.h"
#include "Windows/TextureEditorWindow.h"
#include "Widgets/SectionWidget.h"
#include "WorldEd.h"

WeTextureEditorWindow::WeTextureEditorWindow( const TSharedPtr<FTexture2D>& InTexture2D, QWidget* InParent /* = nullptr */ )
	: QWidget( InParent )
	, bInit( false )
	, ui( new Ui::WeTextureEditorWindow() )
	, texture2D( InTexture2D )
	, viewportClient( new FTexturePreviewViewportClient( InTexture2D ) )
	, comboBox_addressU( nullptr )
	, comboBox_addressV( nullptr )
	, comboBox_filter( nullptr )
	, label_sourceFileValue( nullptr )
	, toolButton_sourceFile( nullptr )
	, toolButton_sourceFileRemove( nullptr )
{
	check( InTexture2D );

	// Init Qt UI
	ui->setupUi( this );
	InitUI();

	// Init preview viewport
	ui->viewportPreview->SetViewportClient( viewportClient, false );
	ui->viewportPreview->SetEnabled( true );
	bInit = true;
}

void WeTextureEditorWindow::InitUI()
{
	// Create UI
	// Section of sampler settings
	{
		WeSectionWidget*	samplerSettingsSection	= new WeSectionWidget( "Sampler Settings", 300, this );
		QFormLayout*		formLayout				= new QFormLayout( samplerSettingsSection );
		comboBox_addressU	= new QComboBox( samplerSettingsSection );
		comboBox_addressV	= new QComboBox( samplerSettingsSection );
		comboBox_filter		= new QComboBox( samplerSettingsSection );

		// Fill comboBoxes
		for ( uint32 index = 0; index < SAM_Max; ++index )
		{
			comboBox_addressU->addItem( QString::fromStdWString( SamplerAddressModeToText( ( ESamplerAddressMode )index ) ) );
			comboBox_addressV->addItem( QString::fromStdWString( SamplerAddressModeToText( ( ESamplerAddressMode )index ) ) );
		}

		for ( uint32 index = 0; index < SF_Max; ++index )
		{
			comboBox_filter->addItem( QString::fromStdWString( SamplerFilterToText( ( ESamplerFilter )index ) ) );
		}

		formLayout->setContentsMargins( 3, 3, 3, 3 );
		formLayout->setWidget( 0, QFormLayout::LabelRole, new QLabel( "Address U:", samplerSettingsSection ) );
		formLayout->setWidget( 0, QFormLayout::FieldRole, comboBox_addressU );
		formLayout->setWidget( 1, QFormLayout::LabelRole, new QLabel( "Address V:", samplerSettingsSection ) );
		formLayout->setWidget( 1, QFormLayout::FieldRole, comboBox_addressV );
		formLayout->setWidget( 2, QFormLayout::LabelRole, new QLabel( "Filter:", samplerSettingsSection ) );
		formLayout->setWidget( 2, QFormLayout::FieldRole, comboBox_filter );

		connect( comboBox_addressU, SIGNAL( currentIndexChanged( int ) ), this, SLOT( on_comboBox_addressU_currentIndexChanged( int ) ) );
		connect( comboBox_addressV, SIGNAL( currentIndexChanged( int ) ), this, SLOT( on_comboBox_addressV_currentIndexChanged( int ) ) );
		connect( comboBox_filter, SIGNAL( currentIndexChanged( int ) ), this, SLOT( on_comboBox_filter_currentIndexChanged( int ) ) );

		samplerSettingsSection->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Preferred );
		samplerSettingsSection->setContentLayout( *formLayout );		
		samplerSettingsSection->expand( true );
		ui->frame->layout()->addWidget( samplerSettingsSection );
	}

	// Section of file path
	{
		WeSectionWidget*	filePathSection		= new WeSectionWidget( "File Path", 300, this );
		QGridLayout*		gridLayout			= new QGridLayout( filePathSection );
		QLabel*				label_sourceFile	= new QLabel( "Source File:", filePathSection );

		label_sourceFileValue		= new QLabel( "%Value%", filePathSection );
		toolButton_sourceFile		= new QToolButton( filePathSection );
		toolButton_sourceFileRemove = new QToolButton( filePathSection );

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

	// Init widgets
	comboBox_addressU->setCurrentIndex( texture2D->GetAddressU() );
	comboBox_addressV->setCurrentIndex( texture2D->GetAddressV() );
	comboBox_filter->setCurrentIndex( texture2D->GetSamplerFilter() );
	ui->actionR->setChecked( viewportClient->IsShowRedChannel() );
	ui->actionG->setChecked( viewportClient->IsShowGreenChannel() );
	ui->actionB->setChecked( viewportClient->IsShowBlueChannel() );
	ui->actionA->setChecked( viewportClient->IsShowAlphaChannel() );

	// Set info about asset
	uint32						sizeX = texture2D->GetSizeX();
	uint32						sizeY = texture2D->GetSizeY();
	const FPixelFormatInfo&		pixelFormatInfo = GPixelFormats[ texture2D->GetPixelFormat() ];

	ui->label_importedValue->setText( QString::fromStdWString( FString::Format( TEXT( "%ix%i" ), sizeX, sizeY ) ) );
	ui->label_formatValue->setText( QString::fromWCharArray( pixelFormatInfo.name ) );
	ui->label_resourceSizeValue->setText( QString::fromStdWString( FString::Format( TEXT( "%.2f Kb" ), ( pixelFormatInfo.blockBytes * sizeX * sizeY ) / 1024.f ) ) );
	OnSourceFileChanged( QString::fromStdWString( texture2D->GetAssetSourceFile() ) );

	// Set icons for actions
	ui->actionReimport->setIcon( QIcon( TCHAR_TO_ANSI( ( appBaseDir() + TEXT( "Engine/Editor/Icons/Import.png" ) ).c_str() ) ) );
	ui->actionR->setIcon( QIcon( TCHAR_TO_ANSI( ( appBaseDir() + TEXT( "Engine/Editor/Icons/Color_R.png" ) ).c_str() ) ) );
	ui->actionG->setIcon( QIcon( TCHAR_TO_ANSI( ( appBaseDir() + TEXT( "Engine/Editor/Icons/Color_G.png" ) ).c_str() ) ) );
	ui->actionB->setIcon( QIcon( TCHAR_TO_ANSI( ( appBaseDir() + TEXT( "Engine/Editor/Icons/Color_B.png" ) ).c_str() ) ) );
	ui->actionA->setIcon( QIcon( TCHAR_TO_ANSI( ( appBaseDir() + TEXT( "Engine/Editor/Icons/Color_A.png" ) ).c_str() ) ) );
}

WeTextureEditorWindow::~WeTextureEditorWindow()
{
	ui->viewportPreview->SetViewportClient( nullptr, false );
	delete viewportClient;
	delete ui;
}


void WeTextureEditorWindow::OnSourceFileChanged( QString InNewSourceFile )
{
	label_sourceFileValue->setText( InNewSourceFile );
	label_sourceFileValue->setToolTip( InNewSourceFile );
	ui->actionReimport->setEnabled( !InNewSourceFile.isEmpty() );
	UICropSourceFileText();
}

void WeTextureEditorWindow::UICropSourceFileText()
{
	QFontMetrics		fontMetrices = label_sourceFileValue->fontMetrics();
	uint32				maxTextWidth = Max( label_sourceFileValue->size().width() - 2, 100 );
	label_sourceFileValue->setText( fontMetrices.elidedText( QString::fromStdWString( texture2D->GetAssetSourceFile() ), Qt::TextElideMode::ElideRight, maxTextWidth ) );
}

void WeTextureEditorWindow::resizeEvent( QResizeEvent* InEvent )
{
	UICropSourceFileText();
}

void WeTextureEditorWindow::on_comboBox_addressU_currentIndexChanged( int InIndex )
{ 
	check( texture2D );
	if ( !bInit || texture2D->GetAddressU() == ( ESamplerAddressMode )InIndex )
	{
		return;
	}

	texture2D->SetAddressU( ( ESamplerAddressMode )InIndex );
	emit OnChangedAsset( texture2D );
}

void WeTextureEditorWindow::on_comboBox_addressV_currentIndexChanged( int InIndex )
{
	check( texture2D );
	if ( !bInit || texture2D->GetAddressV() == ( ESamplerAddressMode )InIndex )
	{
		return;
	}

	texture2D->SetAddressV( ( ESamplerAddressMode )InIndex );
	emit OnChangedAsset( texture2D );
}

void WeTextureEditorWindow::on_comboBox_filter_currentIndexChanged( int InIndex )
{
	check( texture2D );
	if ( !bInit || texture2D->GetSamplerFilter() == ( ESamplerFilter )InIndex )
	{
		return;
	}

	texture2D->SetSamplerFilter( ( ESamplerFilter )InIndex );
	emit OnChangedAsset( texture2D );
}

void WeTextureEditorWindow::on_toolButton_sourceFile_clicked()
{
	check( texture2D );
	QString			newSourceFile = QFileDialog::getOpenFileName( this, "Select New Source File", QString(), FHelperAssetImporter::MakeFilterOfSupportedExtensions( FHelperAssetImporter::ET_Texture2D ) );
	std::wstring	path = appQtAbsolutePathToEngine( newSourceFile );
	if ( newSourceFile.isEmpty() || texture2D->GetAssetSourceFile() == path )
	{
		return;
	}

	texture2D->SetAssetSourceFile( path );
	OnSourceFileChanged( newSourceFile );
	emit OnChangedAsset( texture2D );
}

void WeTextureEditorWindow::on_toolButton_sourceFileRemove_clicked()
{
	check( texture2D );

	texture2D->SetAssetSourceFile( TEXT( "" ) );
	OnSourceFileChanged( "" );
	emit OnChangedAsset( texture2D );
}

void WeTextureEditorWindow::on_actionReimport_triggered()
{
	check( texture2D );

	std::wstring		errorMessage;
	if ( FHelperAssetImporter::Reimport( texture2D->GetAssetHandle(), errorMessage ) )
	{
		emit OnChangedAsset( texture2D );
	}
	else
	{
		QMessageBox::critical( this, "Error", QString::fromStdWString( FString::Format( TEXT( "Failed reimport asset '<b>%s</b>'<br><br>Error: %s" ), texture2D->GetAssetName().c_str(), errorMessage.c_str() ) ) );
	}
}

void WeTextureEditorWindow::on_actionR_toggled( bool InValue )
{
	check( viewportClient );
	viewportClient->ShowRedChannel( InValue );
}

void WeTextureEditorWindow::on_actionG_toggled( bool InValue )
{
	check( viewportClient );
	viewportClient->ShowGreenChannel( InValue );
}

void WeTextureEditorWindow::on_actionB_toggled( bool InValue )
{
	check( viewportClient );
	viewportClient->ShowBlueChannel( InValue );
}

void WeTextureEditorWindow::on_actionA_toggled( bool InValue )
{
	check( viewportClient );
	viewportClient->ShowAlphaChannel( InValue );
}