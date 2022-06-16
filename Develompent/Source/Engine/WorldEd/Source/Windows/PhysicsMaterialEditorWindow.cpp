#include "ui_PhysicsMaterialEditorWindow.h"
#include "Misc/EngineGlobals.h"
#include "System/AssetsImport.h"
#include "Windows/PhysicsMaterialEditorWindow.h"
#include "Widgets/SectionWidget.h"
#include "WorldEd.h"

WePhysicsMaterialEditorWindow::WePhysicsMaterialEditorWindow( const TSharedPtr<CPhysicsMaterial>& InPhysMaterial, QWidget* InParent /* = nullptr */ )
	: QWidget( InParent )
	, bInit( false )
	, ui( new Ui::WePhysicsMaterialEditorWindow() )
	, physMaterial( InPhysMaterial )
	, assetsReloadedHandle( nullptr )
{
	check( InPhysMaterial );

	// Init Qt UI
	ui->setupUi( this );
	InitUI();

	// Subscribe to event when assets reload and create audio component for preview
	assetsReloadedHandle = SEditorDelegates::onAssetsReloaded.Add( std::bind( &WePhysicsMaterialEditorWindow::OnAssetsReloaded, this, std::placeholders::_1 ) );
}

void WePhysicsMaterialEditorWindow::InitUI()
{
	// Fill combobox of surface types
	for ( uint32 index = 0, count = GSurfaceTypeNames.size(); index < count; ++index )
	{
		ui->comboBox_surfaceType->addItem( QString::fromStdWString( GSurfaceTypeNames[ index ].first ) );
	}

	bInit = true;
	UpdateUI();
}

void WePhysicsMaterialEditorWindow::UpdateUI()
{
	// Set values of physics material
	{
		uint32			currentIndex		= 0;
		for ( uint32 index = 0, count = GSurfaceTypeNames.size(); index < count; ++index )
		{
			if ( GSurfaceTypeNames[ index ].second == physMaterial->GetSurfaceType() )
			{
				currentIndex = index;
				break;
			}
		}

		ui->comboBox_surfaceType->setCurrentIndex( GSurfaceTypeNames[ currentIndex ].second );
	}

	ui->doubleSpinBox_staticFriction->setValue( physMaterial->GetStaticFriction() );
	ui->doubleSpinBox_dynamicFriction->setValue( physMaterial->GetDynamicFriction() );
	ui->doubleSpinBox_density->setValue( physMaterial->GetDensity() );
	ui->doubleSpinBox_restitution->setValue( physMaterial->GetRestitution() );
}

WePhysicsMaterialEditorWindow::~WePhysicsMaterialEditorWindow()
{
	delete ui;

	// Unsubscribe from event when assetsreload
	SEditorDelegates::onAssetsReloaded.Remove( assetsReloadedHandle );
}

void WePhysicsMaterialEditorWindow::on_doubleSpinBox_staticFriction_valueChanged( double InValue )
{
	if ( !bInit )
	{
		return;
	}
	physMaterial->SetStaticFriction( InValue );
}

void WePhysicsMaterialEditorWindow::on_doubleSpinBox_dynamicFriction_valueChanged( double InValue )
{
	if ( !bInit )
	{
		return;
	}
	physMaterial->SetDynamicFriction( InValue );
}

void WePhysicsMaterialEditorWindow::on_doubleSpinBox_restitution_valueChanged( double InValue )
{
	if ( !bInit )
	{
		return;
	}
	physMaterial->SetRestitution( InValue );
}

void WePhysicsMaterialEditorWindow::on_doubleSpinBox_density_valueChanged( double InValue )
{
	if ( !bInit )
	{
		return;
	}
	physMaterial->SetDensity( InValue );
}

void WePhysicsMaterialEditorWindow::on_comboBox_surfaceType_currentIndexChanged( int InIndex )
{
	if ( !bInit )
	{
		return;
	}
	physMaterial->SetSurfaceType( GSurfaceTypeNames[ InIndex ].second );
}

void WePhysicsMaterialEditorWindow::OnAssetsReloaded( const std::vector<TSharedPtr<class CAsset>>& InAssets )
{
	// If texture who is edition reloaded, we update UI
	for ( uint32 index = 0, count = InAssets.size(); index < count; ++index )
	{
		if ( InAssets[ index ] == physMaterial )
		{
			UpdateUI();
			return;
		}
	}
}