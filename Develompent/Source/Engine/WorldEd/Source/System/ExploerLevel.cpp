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
	, bIgnoreEventSelectionChanged( false )
	, actorsSelectedDelegate( nullptr )
	, actorsUnselectedDelegate( nullptr )
{
	//setModel( Scene::GetSingleton() );
	//header()->setSectionResizeMode( 0, QHeaderView::ResizeMode::ResizeToContents );

	exploerLevelModel = new WeExploerLevelModel( this );
	setModel( exploerLevelModel );

	style = new WeStyle();
	setStyle( style );

	// Connection to slots
	connect( selectionModel(), SIGNAL( selectionChanged( const QItemSelection&, const QItemSelection& ) ), this, SLOT( OnSelectionChanged( const QItemSelection&, const QItemSelection& ) ) );

	actorsSelectedDelegate		= SEditorDelegates::onActorsSelected.Add(		std::bind( &WeExploerLevel::OnSelectedActors, this, std::placeholders::_1	) );
	actorsUnselectedDelegate	= SEditorDelegates::onActorsUnselected.Add(		std::bind( &WeExploerLevel::OnUnselectedActors, this, std::placeholders::_1 ) );
}

WeExploerLevel::~WeExploerLevel()
{
	delete style;

	if ( actorsSelectedDelegate )
	{
		SEditorDelegates::onActorsSelected.Remove( actorsSelectedDelegate );
	}

	if ( actorsUnselectedDelegate )
	{
		SEditorDelegates::onActorsUnselected.Remove( actorsUnselectedDelegate );
	}
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

void WeExploerLevel::OnSelectionChanged( const QItemSelection& InSelected, const QItemSelection& InDeselected )
{
	if ( bIgnoreEventSelectionChanged )
	{
		return;
	}

	QModelIndexList		selectedIndexes = selectionModel()->selectedIndexes();
	GWorld->UnselectAllActors();
	
	std::vector<ActorRef_t>		selectedActors;
	for ( uint32 index = 0, count = selectedIndexes.size(); index < count; ++index )
	{
		selectedActors.push_back( GWorld->GetActor( selectedIndexes.at( index ).row() ) );
	}

	if ( !selectedActors.empty() )
	{
		GWorld->SelectActors( selectedActors );
	}
}

void WeExploerLevel::OnSelectedActors( const std::vector<ActorRef_t>& InActors )
{
	QItemSelectionModel*		itemSelectionModel = selectionModel();
	for ( uint32 index = 0, count = InActors.size(); index < count; ++index )
	{
		QModelIndex		modelIndex = exploerLevelModel->ActorToModelIndex( InActors[index] );
		if ( !modelIndex.isValid() )
		{
			continue;
		}

		bIgnoreEventSelectionChanged = true;
		itemSelectionModel->select( modelIndex, QItemSelectionModel::SelectionFlag::Select );
		bIgnoreEventSelectionChanged = false;
	}
}

void WeExploerLevel::OnUnselectedActors( const std::vector<ActorRef_t>& InActors )
{
	QItemSelectionModel*	itemSelectionModel = selectionModel();
	for ( uint32 index = 0, count = InActors.size(); index < count; ++index )
	{
		QModelIndex		modelIndex = exploerLevelModel->ActorToModelIndex( InActors[index] );
		if ( !modelIndex.isValid() )
		{
			continue;
		}

		bIgnoreEventSelectionChanged = true;
		itemSelectionModel->select( modelIndex, QItemSelectionModel::SelectionFlag::Deselect );
		bIgnoreEventSelectionChanged = false;
	}
}