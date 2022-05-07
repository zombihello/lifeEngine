#include <qcheckbox.h>
#include <qformlayout.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qlineedit.h>

#include "ui_MaterialEditorWindow.h"
#include "Windows/MaterialEditorWindow.h"
#include "Widgets/SectionWidget.h"
#include "Render/Shaders/ShaderManager.h"

WeMaterialEditorWindow::WeMaterialEditorWindow( FMaterial* InMaterial, QWidget* InParent /* = nullptr */ )
	: QWidget( InParent )
	, bInit( false )
	, ui( new Ui::WeMaterialEditorWindow() )
	, material( InMaterial )
	, checkBox_isTwoSided( nullptr )
	, checkBox_isWireframe( nullptr )
	, checkBox_staticMesh( nullptr )
	, checkBox_sprite( nullptr )
	, comboBox_vertexShader( nullptr )
	, comboBox_hullShader( nullptr )
	, comboBox_domainShader( nullptr )
	, comboBox_pixelShader( nullptr )
	, comboBox_geometryShader( nullptr )
	, comboBox_diffuseTexture( nullptr )
	, toolButton_removeDiffuse( nullptr )
{
	check( InMaterial );

	// Init Qt UI
	ui->setupUi( this );
	InitUI();
	bInit = true;
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
	}

	// Create shaders section
	{
		WeSectionWidget*	shadersSection			= new WeSectionWidget( "Shaders", 300, this );
		QFormLayout*		formLayout				= new QFormLayout();
		formLayout->setContentsMargins( 0, 3, 0, 3 );

		// Vertex shader
		comboBox_vertexShader = new QComboBox( shadersSection );
		formLayout->setWidget( 0, QFormLayout::LabelRole, new QLabel( "Vertex:", shadersSection ) );
		formLayout->setWidget( 0, QFormLayout::FieldRole, comboBox_vertexShader );
		
		// Hull shader
		comboBox_hullShader = new QComboBox( shadersSection );
		formLayout->setWidget( 1, QFormLayout::LabelRole, new QLabel( "Hull:", shadersSection ) );
		formLayout->setWidget( 1, QFormLayout::FieldRole, comboBox_hullShader );

		// Domain shader
		comboBox_domainShader = new QComboBox( shadersSection );
		formLayout->setWidget( 2, QFormLayout::LabelRole, new QLabel( "Domain:", shadersSection ) );
		formLayout->setWidget( 2, QFormLayout::FieldRole, comboBox_domainShader );

		// Pixel shader
		comboBox_pixelShader = new QComboBox( shadersSection );
		formLayout->setWidget( 3, QFormLayout::LabelRole, new QLabel( "Pixel:", shadersSection ) );
		formLayout->setWidget( 3, QFormLayout::FieldRole, comboBox_pixelShader );

		// Geometry shader
		comboBox_geometryShader = new QComboBox( shadersSection );
		formLayout->setWidget( 4, QFormLayout::LabelRole, new QLabel( "Geometry:", shadersSection ) );
		formLayout->setWidget( 4, QFormLayout::FieldRole, comboBox_geometryShader );

		// Fill comboboxes shader
		comboBox_vertexShader->addItem( "None" );
		comboBox_hullShader->addItem( "None" );
		comboBox_domainShader->addItem( "None" );
		comboBox_pixelShader->addItem( "None" );
		comboBox_geometryShader->addItem( "None" );

		const std::unordered_map< std::wstring, FShaderMetaType* >&			shaderTypes = FShaderManager::GetShaderTypes();
		for ( auto itShaderType = shaderTypes.begin(), itShaderTypeEnd = shaderTypes.end(); itShaderType != itShaderTypeEnd; ++itShaderType )
		{
			FShaderMetaType*		shaderType = itShaderType->second;
			switch ( shaderType->GetFrequency() )
			{
			case SF_Vertex:			comboBox_vertexShader->addItem( QString::fromStdWString( shaderType->GetName() ) );		break;
			case SF_Hull:			comboBox_hullShader->addItem( QString::fromStdWString( shaderType->GetName() ) );		break;
			case SF_Domain:			comboBox_domainShader->addItem( QString::fromStdWString( shaderType->GetName() ) );		break;
			case SF_Pixel:			comboBox_pixelShader->addItem( QString::fromStdWString( shaderType->GetName() ) );		break;
			case SF_Geometry:		comboBox_geometryShader->addItem( QString::fromStdWString( shaderType->GetName() ) );	break;
			}
		}

		shadersSection->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Preferred );
		shadersSection->setContentLayout( *formLayout );
		shadersSection->expand( true );
		ui->frame->layout()->addWidget( shadersSection );
	}

	// Create parameters section
	{
		WeSectionWidget*		parametersSection	= new WeSectionWidget( "Parameters", 300, this );
		QGridLayout*			gridLayout			= new QGridLayout();
		gridLayout->setContentsMargins( 0, 3, 0, 3 );

		comboBox_diffuseTexture		= new QComboBox( parametersSection );
		toolButton_removeDiffuse	= new QToolButton( parametersSection );

		comboBox_diffuseTexture->addItem( "None" );
		toolButton_removeDiffuse->setText( "X" );

		gridLayout->addWidget( new QLabel( "Diffuse:", parametersSection ), 0, 0 );
		gridLayout->addWidget( comboBox_diffuseTexture, 0, 1 );
		gridLayout->addWidget( toolButton_removeDiffuse, 0, 2 );

		parametersSection->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Preferred );
		parametersSection->setContentLayout( *gridLayout );
		parametersSection->expand( true );
		ui->frame->layout()->addWidget( parametersSection );
	}

	// Create spacer
	{
		QSpacerItem*		verticalSpacer = new QSpacerItem( 20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding );
		ui->frame->layout()->addItem( verticalSpacer );
	}
}

WeMaterialEditorWindow::~WeMaterialEditorWindow()
{
	delete ui;
}