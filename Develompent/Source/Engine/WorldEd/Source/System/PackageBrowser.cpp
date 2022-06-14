#include <qdebug.h>
#include <qheaderview.h>
#include <qapplication.h>
#include <qmimedata.h>
#include <QMouseEvent>
#include <qstyleoption.h>
#include <qpen.h>
#include <qpainter.h>

#include "System/PackageBrowser.h"

void WePackageBrowser::WeStyle::drawPrimitive( PrimitiveElement InElement, const QStyleOption* InOption, QPainter* InPainter, const QWidget* InWidget ) const
{
	if ( InElement == QStyle::PE_IndicatorItemViewItemDrop )
	{
		InPainter->setRenderHint( QPainter::Antialiasing, true );

		QPalette		palette;
		QColor			color( 23, 154, 224 );
		QPen			pen( color );
		
		pen.setWidth( 2 );
		color.setAlpha( 50 );
	
		QBrush			brush( color );

		InPainter->setPen( pen );
		InPainter->setBrush( brush );

		if ( InOption->rect.height() == 0 )
		{
			InPainter->drawEllipse( InOption->rect.topLeft(), 3, 3 );
			InPainter->drawLine( QPoint( InOption->rect.topLeft().x() + 3, InOption->rect.topLeft().y() ), InOption->rect.topRight() );
		}
		else
		{
			InPainter->drawRoundedRect( InOption->rect, 5, 5 );
		}
	}
	else
	{
		QProxyStyle::drawPrimitive( InElement, InOption, InPainter, InWidget );
	}
}

WePackageBrowser::WePackageBrowser( QWidget* InParent /* = nullptr */ )
	: QListView( InParent )
{
	packageModel = new WePackageModel( this );
	setModel( packageModel );

	style = new WeStyle();
	setStyle( style );

	// Connection to slots
	connect( this, SIGNAL( clicked( QModelIndex ) ), this, SLOT( OnClicked( QModelIndex ) ) );
}

WePackageBrowser::~WePackageBrowser()
{
	delete style;
}

void WePackageBrowser::mousePressEvent( QMouseEvent* InEvent )
{
	// If we clicked in empty space - clear all selections
	if ( !indexAt( InEvent->pos() ).isValid() )
	{
		selectionModel()->clear();
		emit OnSelectedAsset( TEXT( "" ) );
	}

	QListView::mousePressEvent( InEvent );
}

void WePackageBrowser::OnClicked( QModelIndex InIndex )
{
	// Getting current package
	PackageRef_t			package			= GetPackage();
	std::wstring		assetReference	= TEXT( "" );
	if ( package && InIndex.isValid() )
	{
		SAssetInfo		assetInfo;
		package->GetAssetInfo( InIndex.row(), assetInfo );
		
		// Make reference to asset
		if ( !MakeReferenceToAsset( package->GetName(), assetInfo.name, assetInfo.type, assetReference ) )
		{
			assetReference = TEXT( "" );
		}
	}

	emit OnSelectedAsset( assetReference );
}