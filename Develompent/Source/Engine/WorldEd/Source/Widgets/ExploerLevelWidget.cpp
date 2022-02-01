#include "Widgets/ExploerLevelWidget.h"
#include "ui_ExploerLevelWidget.h"

WeExploerLevelWidget::WeExploerLevelWidget( QWidget* InParent /* = nullptr */ )
	: QWidget( InParent )
	, ui( new Ui::WeExploerLevelWidget() )
{
	// Init Qt UI
	ui->setupUi( this );
}

WeExploerLevelWidget::~WeExploerLevelWidget()
{
	delete ui;
}