#include <qmessagebox.h>
#include <qfiledialog.h>
#include <qformlayout.h>
#include <qlabel.h>
#include <qcombobox.h>

#include "ui_AudioBankEditorWindow.h"
#include "Containers/StringConv.h"
#include "System/AssetsImport.h"
#include "Windows/AudioBankEditorWindow.h"
#include "Widgets/SectionWidget.h"
#include "Components/AudioComponent.h"
#include "WorldEd.h"

WeAudioBankEditorWindow::WeAudioBankEditorWindow( const TSharedPtr<CAudioBank>& InAudioBank, QWidget* InParent /* = nullptr */ )
	: QWidget( InParent )
	, bInit( false )
	, ui( new Ui::WeAudioBankEditorWindow() )
	, audioBank( InAudioBank )
	, label_sourceFileValue( nullptr )
	, toolButton_sourceFile( nullptr )
	, toolButton_sourceFileRemove( nullptr )
	, pushButton_play( nullptr )
	, pushButton_pause( nullptr )
	, pushButton_stop( nullptr )
	, audioComponent( nullptr )
	, assetsReloadedHandle( nullptr )
{
	check( InAudioBank );

	// Init Qt UI
	ui->setupUi( this );
	InitUI();

	// Subscribe to event when assets reload and create audio component for preview
	assetsReloadedHandle = SEditorDelegates::onAssetsReloaded.Add( std::bind( &WeAudioBankEditorWindow::OnAssetsReloaded, this, std::placeholders::_1 ) );

	audioComponent		= new CAudioComponent();
	audioComponent->SetAudioBank( audioBank->GetAssetHandle() );
	audioComponent->SetUISound( true );
	audioComponent->SetStreamable( true );
}

void WeAudioBankEditorWindow::InitUI()
{
	// Create UI
	// Section of file path
	{
		WeSectionWidget*	filePathSection		= new WeSectionWidget( "File Path", 300, this );
		QGridLayout*		gridLayout			= new QGridLayout( filePathSection );
		QLabel*				label_sourceFile	= new QLabel( "Source File:", filePathSection );

		label_sourceFileValue		= new QLabel( "%Value%", filePathSection );
		toolButton_sourceFile		= new QToolButton( filePathSection );
		toolButton_sourceFileRemove = new QToolButton( filePathSection );

		label_sourceFile->setSizePolicy( QSizePolicy::Maximum, QSizePolicy::Expanding );
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

	// Section of buttons for preview sound
	{
		WeSectionWidget*	previewSection = new WeSectionWidget( "Preview", 300, this );
		QHBoxLayout*		hBoxLayout = new QHBoxLayout( previewSection );
		pushButton_play		= new QPushButton( "Play", previewSection );
		pushButton_pause	= new QPushButton( "Pause", previewSection );
		pushButton_stop		= new QPushButton( "Stop", previewSection );

		hBoxLayout->setContentsMargins( 3, 3, 3, 3 );
		hBoxLayout->addWidget( pushButton_play );
		hBoxLayout->addWidget( pushButton_pause );
		hBoxLayout->addWidget( pushButton_stop );

		connect( pushButton_play, SIGNAL( clicked() ), this, SLOT( OnAudioPlay() ) );
		connect( pushButton_pause, SIGNAL( clicked() ), this, SLOT( OnAudioPause() ) );
		connect( pushButton_stop, SIGNAL( clicked() ), this, SLOT( OnAudioStop() ) );

		previewSection->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Preferred );
		previewSection->setContentLayout( *hBoxLayout );
		previewSection->expand( true );
		ui->frame->layout()->addWidget( previewSection );
	}

	// Spacer
	{
		QSpacerItem*		verticalSpacer = new QSpacerItem( 20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding );
		ui->frame->layout()->addItem( verticalSpacer );
	}

	// Set icons for actions
	ui->actionReimport->setIcon( QIcon( TCHAR_TO_ANSI( ( appBaseDir() + TEXT( "Engine/Editor/Icons/Import.png" ) ).c_str() ) ) );

	bInit = true;
	UpdateUI();
}

void WeAudioBankEditorWindow::UpdateUI()
{
	// Set info about asset
	SAudioBankInfo			audioBankInfo;
	AudioBankHandle_t		audioBankHandle = audioBank->OpenBank( audioBankInfo );
	if ( audioBankHandle )
	{
		ui->label_formatValue->setText( QString::fromStdWString( appSampleFormatToText( audioBankInfo.format ) ) );
		ui->label_channelsValue->setText( QString::number( audioBankInfo.numChannels ) );
		ui->label_rateValue->setText( QString::number( audioBankInfo.rate ) );
		ui->label_resourceSizeValue->setText( QString::fromStdWString( ÑString::Format( TEXT( "%.2f Kb" ), ( appGetNumSampleBytes( audioBankInfo.format ) * audioBankInfo.numSamples ) / 1024.f ) ) );
		audioBank->CloseBank( audioBankHandle );
	}
	else
	{
		ui->label_formatValue->setText( "None" );
		ui->label_channelsValue->setText( "0" );
		ui->label_rateValue->setText( "0" );
		ui->label_resourceSizeValue->setText( "0 Kb" );
	}

	OnSourceFileChanged( QString::fromStdWString( audioBank->GetAssetSourceFile() ) );
}

WeAudioBankEditorWindow::~WeAudioBankEditorWindow()
{
	delete ui;
	delete audioComponent;

	// Unsubscribe from event when assetsreload
	SEditorDelegates::onAssetsReloaded.Remove( assetsReloadedHandle );
}


void WeAudioBankEditorWindow::OnSourceFileChanged( QString InNewSourceFile )
{
	label_sourceFileValue->setText( InNewSourceFile );
	label_sourceFileValue->setToolTip( InNewSourceFile );
	ui->actionReimport->setEnabled( !InNewSourceFile.isEmpty() );
	UICropSourceFileText();
}

void WeAudioBankEditorWindow::UICropSourceFileText()
{
	QFontMetrics		fontMetrices = label_sourceFileValue->fontMetrics();
	uint32				maxTextWidth = Max( label_sourceFileValue->size().width() - 2, 100 );
	label_sourceFileValue->setText( fontMetrices.elidedText( QString::fromStdWString( audioBank->GetAssetSourceFile() ), Qt::TextElideMode::ElideRight, maxTextWidth ) );
}

void WeAudioBankEditorWindow::resizeEvent( QResizeEvent* InEvent )
{
	UICropSourceFileText();
}

void WeAudioBankEditorWindow::on_toolButton_sourceFile_clicked()
{
	check( audioBank );
	QString			newSourceFile = QFileDialog::getOpenFileName( this, "Select New Source File", QString(), CHelperAssetImporter::MakeFilterOfSupportedExtensions( CHelperAssetImporter::ET_AudioBank ) );
	std::wstring	path = appQtAbsolutePathToEngine( newSourceFile );
	if ( newSourceFile.isEmpty() || audioBank->GetAssetSourceFile() == path )
	{
		return;
	}

	audioBank->SetAssetSourceFile( path );
	OnSourceFileChanged( newSourceFile );
	emit OnChangedAsset( audioBank );
}

void WeAudioBankEditorWindow::on_toolButton_sourceFileRemove_clicked()
{
	check( audioBank );

	audioBank->SetAssetSourceFile( TEXT( "" ) );
	OnSourceFileChanged( "" );
	emit OnChangedAsset( audioBank );
}

void WeAudioBankEditorWindow::on_actionReimport_triggered()
{
	check( audioBank );

	std::wstring		errorMessage;
	audioComponent->Stop();
	if ( CHelperAssetImporter::Reimport( audioBank->GetAssetHandle(), errorMessage ) )
	{
		emit OnChangedAsset( audioBank );
	}
	else
	{
		QMessageBox::critical( this, "Error", QString::fromStdWString( ÑString::Format( TEXT( "Failed reimport asset '<b>%s</b>'<br><br>Error: %s" ), audioBank->GetAssetName().c_str(), errorMessage.c_str() ) ) );
	}
}

void WeAudioBankEditorWindow::OnAudioPlay()
{
	audioComponent->Play();
}

void WeAudioBankEditorWindow::OnAudioPause()
{
	audioComponent->Pause();
}

void WeAudioBankEditorWindow::OnAudioStop()
{
	audioComponent->Stop();
}

void WeAudioBankEditorWindow::OnAssetsReloaded( const std::vector<TSharedPtr<class CAsset>>& InAssets )
{
	// If texture who is edition reloaded, we update UI
	for ( uint32 index = 0, count = InAssets.size(); index < count; ++index )
	{
		if ( InAssets[ index ] == audioBank )
		{
			UpdateUI();
			return;
		}
	}
}