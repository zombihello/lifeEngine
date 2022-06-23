#include "Actors/Actor.h"
#include "Widgets/ActorClassesWidget.h"
#include "System/ExploerClassesModel.h"
#include "ui_ActorClassesWidget.h"

WeActorClassesWidget::WeActorClassesWidget( QWidget* InParent /* = nullptr */ )
	: QWidget( InParent )
	, ui( new Ui::WeActorClassesWidget() )
	, exploerClassesModel( nullptr )
{
	// Init Qt UI
	ui->setupUi( this );
	InitUI();
}

WeActorClassesWidget::~WeActorClassesWidget()
{
	delete ui;
}

void WeActorClassesWidget::InitUI()
{
	exploerClassesModel			= new WeExploerClassesModel( AActor::StaticClass(), this );
	ui->treeView_classes->setModel( exploerClassesModel );
	ui->treeView_classes->header()->setHidden( true );
}