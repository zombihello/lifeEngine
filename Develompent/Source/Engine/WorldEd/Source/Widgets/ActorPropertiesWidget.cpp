#include "Widgets/ActorPropertiesWidget.h"
#include "ui_ActorPropertiesWidget.h"

WeActorPropertiesWidget::WeActorPropertiesWidget( QWidget* InParent /* = nullptr */ )
	: QWidget( InParent )
	, ui( new Ui::WeActorPropertiesWidget() )
{
	// Init Qt UI
	ui->setupUi( this );
}

WeActorPropertiesWidget::~WeActorPropertiesWidget()
{
	delete ui;
}