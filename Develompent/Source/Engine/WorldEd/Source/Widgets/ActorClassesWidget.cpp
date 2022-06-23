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
	filterProxyModel.setSourceModel( exploerClassesModel );
	filterProxyModel.setFilterKeyColumn( 0 );
	filterProxyModel.setFilterRole( Qt::DisplayRole );

	ui->treeView_classes->setModel( &filterProxyModel );
	ui->treeView_classes->header()->setHidden( true );
}

void WeActorClassesWidget::on_lineEdit_search_textEdited( QString InValue )
{
	filterProxyModel.SetFilterPattern( InValue + "*" );
}

void WeActorClassesWidget::on_toolButton_clearSearch_clicked()
{
	ui->lineEdit_search->setText( "" );
	filterProxyModel.SetFilterPattern( "*" );
}