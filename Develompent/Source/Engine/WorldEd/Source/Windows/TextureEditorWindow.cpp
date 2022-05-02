#include "ui_TextureEditorWindow.h"
#include "Render/TexturePreviewViewportClient.h"
#include "Render/RenderUtils.h"
#include "Windows/TextureEditorWindow.h"

WeTextureEditorWindow::WeTextureEditorWindow( FTexture2D* InTexture2D, QWidget* InParent /* = nullptr */ )
	: QWidget( InParent )
	, ui( new Ui::WeTextureEditorWindow() )
	, texture2D( InTexture2D )
{
	check( InTexture2D );

	// Init Qt UI
	ui->setupUi( this );
	InitUI();

	// Init preview viewport
	ui->viewportPreview->SetViewportClient( new FTexturePreviewViewportClient( InTexture2D ), true );
	ui->viewportPreview->SetEnabled( true );
}

void WeTextureEditorWindow::InitUI()
{
	// Init splitter sizes
	QList< int >		splitterSizes = ui->splitter->sizes();
	splitterSizes.replace( 0, height() / 0.3f );
	splitterSizes.replace( 1, height() / 0.7f );
	ui->splitter->setSizes( splitterSizes );

	// Set info about asset
	uint32						sizeX = texture2D->GetSizeX();
	uint32						sizeY = texture2D->GetSizeY();
	const FPixelFormatInfo&		pixelFormatInfo = GPixelFormats[ texture2D->GetPixelFormat() ];

	ui->label_importedValue->setText( QString::fromStdWString( FString::Format( TEXT( "%ix%i" ), sizeX, sizeY ) ) );
	ui->label_formatValue->setText( QString::fromWCharArray( pixelFormatInfo.name ) );
	ui->label_resourceSizeValue->setText( QString::fromStdWString( FString::Format( TEXT( "%.2f Kb" ), ( pixelFormatInfo.blockBytes * sizeX * sizeY ) / 1024.f ) ) );
}

WeTextureEditorWindow::~WeTextureEditorWindow()
{
	delete ui;
}