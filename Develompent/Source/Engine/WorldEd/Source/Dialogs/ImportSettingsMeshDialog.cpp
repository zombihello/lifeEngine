#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>

#include "Widgets/SectionWidget.h"
#include "Dialogs/ImportSettingsMeshDialog.h"

FORCEINLINE std::wstring AxisUpToText( WeImportSettingsMeshDialog::EAxis InAxisUp )
{
	switch ( InAxisUp )
	{
	case WeImportSettingsMeshDialog::A_PlusX:		return TEXT( "+X" );
	case WeImportSettingsMeshDialog::A_MinusX:		return TEXT( "-X" );
	case WeImportSettingsMeshDialog::A_PlusY:		return TEXT( "+Y" );
	case WeImportSettingsMeshDialog::A_MinusY:		return TEXT( "-Y" );
	case WeImportSettingsMeshDialog::A_PlusZ:		return TEXT( "+Z" );
	case WeImportSettingsMeshDialog::A_MinusZ:		return TEXT( "-Z" );

	default:
		appErrorf( TEXT( "Unknown axis" ) );
		return TEXT( "Unknown" );
	}
}

WeImportSettingsMeshDialog::WeImportSettingsMeshDialog( QWidget* InParent /* = nullptr */ )
	: QDialog( InParent )
	, comboBox_AxisUp( nullptr )
	, pushButton_importAll( nullptr )
	, pushButton_import( nullptr )
	, pushButton_cancel( nullptr )
{
	appMemzero( &importSettings, sizeof( SImportSettings ) );

	// Init UI
	InitUI();

	// Result of dialog by default
	setResult( DR_Cancel );
}

WeImportSettingsMeshDialog::~WeImportSettingsMeshDialog()
{}

void WeImportSettingsMeshDialog::InitUI()
{
	setWindowTitle( "Import Settings Mesh" );
	setMinimumSize( QSize( 300, 110 ) );
	setMaximumSize( QSize( 300, 110 ) );
	QVBoxLayout*			mainLayout		= new QVBoxLayout( this );

	// Mesh section
	{
		WeSectionWidget*	meshSection		= new WeSectionWidget( "Mesh", 300, this );
		QFormLayout*		formLayout		= new QFormLayout( meshSection );
		comboBox_AxisUp		= new QComboBox( meshSection );

		// Fill combobox of axis ups
		for ( uint32 index = 0; index < WeImportSettingsMeshDialog::A_Num; ++index )
		{
			comboBox_AxisUp->addItem( QString::fromStdWString( AxisUpToText( ( WeImportSettingsMeshDialog::EAxis )index ) ) );
		}
		importSettings.axisUp	= ( WeImportSettingsMeshDialog::EAxis )0;	// First item in combo box

		formLayout->setContentsMargins( 3, 3, 3, 3 );
		formLayout->setWidget( 0, QFormLayout::LabelRole, new QLabel( "Axis Up:", meshSection ) );
		formLayout->setWidget( 0, QFormLayout::FieldRole, comboBox_AxisUp );

		connect( comboBox_AxisUp, SIGNAL( currentIndexChanged( int ) ), this, SLOT( OnComboBoxAxisUp_CurrentIndexChanged( int ) ) );

		meshSection->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Preferred );
		meshSection->setContentLayout( *formLayout );
		meshSection->expand( true );
		mainLayout->addWidget( meshSection );
	}

	// Dialog buttons
	{
		QFrame*		line = new QFrame( this );
		line->setFrameShape( QFrame::HLine );
		line->setFrameShadow( QFrame::Sunken );
		mainLayout->addWidget( line );

		QHBoxLayout*	hBoxLyaout	= new QHBoxLayout( this );
		pushButton_importAll		= new QPushButton( this );
		pushButton_import			= new QPushButton( this );
		pushButton_cancel			= new QPushButton( this );

		pushButton_importAll->setText( "Import All" );
		pushButton_import->setText( "Import" );
		pushButton_cancel->setText( "Cancel" );

		connect( pushButton_importAll, SIGNAL( clicked() ), this, SLOT( OnPushButtonImportAll_Clicked() ) );
		connect( pushButton_import, SIGNAL( clicked() ), this, SLOT( OnPushButtonImport_Clicked() ) );
		connect( pushButton_cancel, SIGNAL( clicked() ), this, SLOT( OnPushButtonCancel_Clicked() ) );

		hBoxLyaout->setContentsMargins( 3, 3, 3, 3 );
		hBoxLyaout->addWidget( pushButton_importAll );
		hBoxLyaout->addWidget( pushButton_import );
		hBoxLyaout->addWidget( pushButton_cancel );
		mainLayout->addItem( hBoxLyaout );
	}

	// Spacer
	{
		QSpacerItem*		verticalSpacer = new QSpacerItem( 20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding );
		mainLayout->addItem( verticalSpacer );
	}

	setLayout( mainLayout );
}

void WeImportSettingsMeshDialog::UpdateUI()
{}

WeImportSettingsMeshDialog::EDialogResult WeImportSettingsMeshDialog::GetImportSettings( SImportSettings& OutImportSettings, QWidget* InParent /*= nullptr*/ )
{
	WeImportSettingsMeshDialog		importSettingsDialog;
	EDialogResult					dialogResult = ( WeImportSettingsMeshDialog::EDialogResult )importSettingsDialog.exec();
	if ( dialogResult != DR_Cancel )
	{
		OutImportSettings = importSettingsDialog.importSettings;
	}

	return dialogResult;
}

void WeImportSettingsMeshDialog::closeEvent( QCloseEvent* InEvent )
{
	QDialog::closeEvent( InEvent );
	done( DR_Cancel );
}

void WeImportSettingsMeshDialog::OnComboBoxAxisUp_CurrentIndexChanged( int InIndex )
{
	importSettings.axisUp = ( WeImportSettingsMeshDialog::EAxis )InIndex;
}

void WeImportSettingsMeshDialog::OnPushButtonImportAll_Clicked()
{
	done( DR_ImportAll );
}

void WeImportSettingsMeshDialog::OnPushButtonImport_Clicked()
{
	done( DR_Import );
}

void WeImportSettingsMeshDialog::OnPushButtonCancel_Clicked()
{
	done( DR_Cancel );
}