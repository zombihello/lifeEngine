#include <qmessagebox.h>
#include <qfiledialog.h>

#include "ui_TextureEditorWindow.h"
#include "Containers/StringConv.h"
#include "System/AssetsImport.h"
#include "Render/TexturePreviewViewportClient.h"
#include "Render/RenderUtils.h"
#include "Windows/TextureEditorWindow.h"
#include "WorldEd.h"

WeTextureEditorWindow::WeTextureEditorWindow( FTexture2D* InTexture2D, QWidget* InParent /* = nullptr */ )
	: QWidget( InParent )
	, bInit( false )
	, ui( new Ui::WeTextureEditorWindow() )
	, texture2D( InTexture2D )
	, viewportClient( new FTexturePreviewViewportClient( InTexture2D ) )
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
	// Fill comboboxes and init him
	for ( uint32 index = 0; index < SAM_Max; ++index )
	{
		ui->comboBox_addressU->addItem( QString::fromStdWString( SamplerAddressModeToText( ( ESamplerAddressMode )index ) ) );
		ui->comboBox_addressV->addItem( QString::fromStdWString( SamplerAddressModeToText( ( ESamplerAddressMode )index ) ) );
	}

	for ( uint32 index = 0; index < SF_Max; ++index )
	{
		ui->comboBox_filter->addItem( QString::fromStdWString( SamplerFilterToText( ( ESamplerFilter )index ) ) );
	}

	ui->comboBox_addressU->setCurrentIndex( texture2D->GetAddressU() );
	ui->comboBox_addressV->setCurrentIndex( texture2D->GetAddressV() );
	ui->comboBox_filter->setCurrentIndex( texture2D->GetSamplerFilter() );
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
	ui->label_sourceFileValue->setText( InNewSourceFile );
	ui->label_sourceFileValue->setToolTip( InNewSourceFile );
	ui->actionReimport->setEnabled( !InNewSourceFile.isEmpty() );
	UICropSourceFileText();
}

void WeTextureEditorWindow::UICropSourceFileText()
{
	QFontMetrics		fontMetrices = ui->label_sourceFileValue->fontMetrics();
	uint32				maxTextWidth = Max( ui->label_sourceFileValue->size().width() - 2, 100 );
	ui->label_sourceFileValue->setText( fontMetrices.elidedText( QString::fromStdWString( texture2D->GetAssetSourceFile() ), Qt::TextElideMode::ElideRight, maxTextWidth ) );
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
	if ( FHelperAssetImporter::Reimport( texture2D, errorMessage ) )
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