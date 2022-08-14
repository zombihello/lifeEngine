#include <qdebug.h>
#include <qheaderview.h>
#include <qapplication.h>
#include <qmimedata.h>
#include <QMouseEvent>
#include <qstyleoption.h>
#include <qpen.h>
#include <qpainter.h>
#include <qdrag.h>
#include <qscrollbar.h>

#include "Misc/EngineGlobals.h"
#include "Misc/SharedPointer.h"
#include "System/ExploerLevel.h"
#include "System/World.h"

void WeExploerLevel::WeStyle::drawPrimitive( PrimitiveElement Element, const QStyleOption* Option, QPainter* Painter, const QWidget* Widget ) const
{
	if ( Element == QStyle::PE_IndicatorItemViewItemDrop )
	{
		Painter->setRenderHint( QPainter::Antialiasing, true );

		QPalette		palette;
		QColor			color( 23, 154, 224 );
		QPen			pen( color );

		pen.setWidth( 2 );
		color.setAlpha( 50 );

		QBrush			brush( color );

		Painter->setPen( pen );
		Painter->setBrush( brush );

		if ( Option->rect.height() == 0 )
		{
			Painter->drawEllipse( Option->rect.topLeft(), 3, 3 );
			Painter->drawLine( QPoint( Option->rect.topLeft().x() + 3, Option->rect.topLeft().y() ), Option->rect.topRight() );
		}
		else
		{
			Painter->drawRoundedRect( Option->rect, 5, 5 );
		}
	}
	else
	{
		QProxyStyle::drawPrimitive( Element, Option, Painter, Widget );
	}
}

WeExploerLevel::WeExploerLevel( QWidget* parent ) 
	: QListView( parent )
{
	//setModel( Scene::GetSingleton() );
	//header()->setSectionResizeMode( 0, QHeaderView::ResizeMode::ResizeToContents );

	exploerLevelModel = new WeExploerLevelModel( this );
	setModel( exploerLevelModel );

	style = new WeStyle();
	setStyle( style );

	// Connection to slots
	connect( this, SIGNAL( clicked( QModelIndex ) ), this, SLOT( OnClicked( QModelIndex ) ) );
}

WeExploerLevel::~WeExploerLevel()
{
	delete style;
}

void WeExploerLevel::mousePressEvent( QMouseEvent* Event )
{
	if ( !indexAt( Event->pos() ).isValid() )
	{
		setCurrentIndex( QModelIndex() );
		GWorld->UnselectAllActors();
	}
	//else
	//	resizeColumnToContents( 0 );

	QListView::mousePressEvent( Event );
}

void WeExploerLevel::dropEvent( QDropEvent * Event )
{
	QListView::dropEvent( Event );

	/*const QMimeData* mimeData = Event->mimeData();

	if ( !mimeData->hasFormat( "expploerlevel/gameobject" ) )
	{
		Event->ignore();
		return;
	}

	QByteArray					data = mimeData->data( "expploerlevel/gameobject" );
	QDataStream					dataStream( &data, QIODevice::ReadOnly );
	QModelIndex					targetModelIndex = indexAt( Event->pos() );
	int							countItems = 0;
	
	dataStream.readRawData( ( char* ) &countItems, sizeof( int ) );

	for ( int index = 0; index < countItems; ++index )
	{
		QModelIndex				modelIndex;
		QStandardItem*			item;
		dataStream.readRawData( ( char* ) &modelIndex, sizeof( QModelIndex ) );
	
		if ( !modelIndex.isValid() ) continue;
		item = gameObjectModel.itemFromIndex( modelIndex );

		if ( targetModelIndex.isValid() )
		{
			gameObjectModel.itemFromIndex( targetModelIndex )->appendRow( new QStandardItem( item->icon(), item->text() ) );
		}
		else
		{
			gameObjectModel.insertRow( gameObjectModel.rowCount(), new QStandardItem( item->icon(), item->text() ) );
		}
	}

	viewport()->update();*/
}

void WeExploerLevel::OnClicked( QModelIndex InIndex )
{
	if ( !InIndex.isValid() )
	{
		return;
	}

	if ( selectionModel()->selectedRows().size() <= 1 )
	{
		GWorld->UnselectAllActors();
	}

	GWorld->SelectActor( GWorld->GetActor( InIndex.row() ) );
}