#include <qmenu.h>
#include <qinputdialog.h>

#include "Misc/EngineGlobals.h"
#include "System/World.h"
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

void WeExploerLevelWidget::on_listView_level_customContextMenuRequested( QPoint InPoint )
{
	QModelIndex		modelIndex		= ui->listView_level->indexAt( InPoint );
	uint32			numSelecteItems = ui->listView_level->selectionModel()->selectedRows().length();

	// Main menu
	QMenu			contextMenu( this );
	QAction			actionActorSpawn( "Spawn", this );
	QAction			actionActorRename( "Rename", this );
	QAction			actionActorDuplicate( "Duplicate", this );
	QAction			actionActorDelete( "Delete", this );
	QAction			actionActorProperties( "Properties", this );
	contextMenu.addAction( &actionActorSpawn );
	contextMenu.addSeparator();
	contextMenu.addAction( &actionActorRename );
	contextMenu.addAction( &actionActorDuplicate );
	contextMenu.addAction( &actionActorDelete );
	contextMenu.addSeparator();
	contextMenu.addAction( &actionActorProperties );

	const bool		bSelectedMoreOneItems = numSelecteItems > 1;
	const bool		bValidItem = modelIndex.isValid();

	actionActorRename.setEnabled( bValidItem && !bSelectedMoreOneItems );
	actionActorDuplicate.setEnabled( bValidItem );
	actionActorDelete.setEnabled( bValidItem );
	actionActorProperties.setEnabled( bValidItem && !bSelectedMoreOneItems );

	// Connect to signal of a actions
	connect( &actionActorSpawn, SIGNAL( triggered() ), this, SLOT( OnContextMenuActorSpawn() ) );
	connect( &actionActorRename, SIGNAL( triggered() ), this, SLOT( OnContextMenuActorRename() ) );
	connect( &actionActorDuplicate, SIGNAL( triggered() ), this, SLOT( OnContextMenuActorDuplicate() ) );
	connect( &actionActorDelete, SIGNAL( triggered() ), this, SLOT( OnContextMenuActorDelete() ) );
	connect( &actionActorProperties, SIGNAL( triggered() ), this, SLOT( OnContextMenuActorProperties() ) );

	contextMenu.exec( QCursor::pos() );
}

void WeExploerLevelWidget::OnContextMenuActorSpawn()
{}

void WeExploerLevelWidget::OnContextMenuActorRename()
{
	// Getting selected items
	QModelIndexList			modelIndexList = ui->listView_level->selectionModel()->selectedRows();
	check( modelIndexList.length() == 1 );		// Support rename only first actor
	
	// Get actor
	ActorRef_t		actor = GWorld->GetActor( modelIndexList[ 0 ].row() );

	// Get new actor name. If we not press 'ok' nothing apply and exit from method
	bool			bIsOk			= false;
	QString			newActorName	= QInputDialog::getText( this, "Enter", "New Asset Name", QLineEdit::Normal, QString::fromStdWString( actor->GetName() ), &bIsOk );
	if ( !bIsOk )
	{
		return;
	}

	// Change actor name
	actor->SetName( newActorName.toStdWString().c_str() );
}

void WeExploerLevelWidget::OnContextMenuActorDelete()
{
	// Getting selected items
	QModelIndexList		modelIndexList = ui->listView_level->selectionModel()->selectedRows();

	// Delete all selected actors
	std::vector<ActorRef_t>		actors;
	for ( uint32 index = 0, count = modelIndexList.length(); index < count; ++index )
	{
		actors.push_back( GWorld->GetActor( modelIndexList[ index ].row() ) );
	}

	for ( uint32 index = 0, count = actors.size(); index < count; ++index )
	{
		GWorld->DestroyActor( actors[ index ] );
	}

	// Clear selection
	ui->listView_level->selectionModel()->clearSelection();
}

void WeExploerLevelWidget::OnContextMenuActorDuplicate()
{}

void WeExploerLevelWidget::OnContextMenuActorProperties()
{}