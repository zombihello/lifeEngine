#include <qdebug.h>
#include <qheaderview.h>
#include <qapplication.h>
#include <qmimedata.h>
#include <QMouseEvent>
#include <qstyleoption.h>
#include <qpen.h>
#include <qpainter.h>

#include "System/ContentBrowser.h"
#include "Misc/CoreGlobals.h"
#include "System/Package.h"
#include "WorldEd.h"

void WeContentBrowser::WeStyle::drawPrimitive( PrimitiveElement InElement, const QStyleOption* InOption, QPainter* InPainter, const QWidget* InWidget ) const
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

WeContentBrowser::WeContentBrowser( QWidget* InParent /* = nullptr */ )
	: QTreeView( InParent )
{
	// Build support list of file extensions
	QStringList			filters;
	filters << "*." FILE_PACKAGE_EXTENSION;
	filters << "*." FILE_MAP_EXTENSION;

	fileSystemModel = new WeFileSystemModel( this );
	fileSystemModel->setFilter( QDir::AllDirs | QDir::QDir::AllEntries | QDir::QDir::NoDotAndDotDot | QDir::QDir::NoSymLinks );
	fileSystemModel->setNameFilters( filters );
	fileSystemModel->setNameFilterDisables( false );
	fileSystemModel->setRootPath( "" );

	setModel( fileSystemModel );
	header()->setSectionResizeMode( 0, QHeaderView::ResizeMode::ResizeToContents );

	// We remove all columns in the explorer (file type, size, creation date) and leave only the file name
	for ( int index = 1, count = fileSystemModel->columnCount(); index < count; ++index )
	{
		hideColumn( index );
	}

	style = new WeStyle();
	setStyle( style );

	connect( this, SIGNAL( clicked( QModelIndex ) ), this, SLOT( on_treeView_contentBrowser_clicked( QModelIndex ) ) );
}

WeContentBrowser::~WeContentBrowser()
{
	delete fileSystemModel;
	delete style;
}

void WeContentBrowser::SetRootDir( const QDir& InRootDir )
{
	rootDir = InRootDir;
	setRootIndex( fileSystemModel->index( rootDir.path() ) );
}

QFileSystemModel* WeContentBrowser::GetFileSystemModel() const
{
	return fileSystemModel;
}

const QDir& WeContentBrowser::GetRootDir() const
{
	return rootDir;
}

void WeContentBrowser::mousePressEvent( QMouseEvent* InEvent )
{
	if ( !indexAt( InEvent->pos() ).isValid() )
	{
		setCurrentIndex( QModelIndex() );
		emit OnOpenPackage( nullptr );
	}

	QTreeView::mousePressEvent( InEvent );
}

void WeContentBrowser::dragEnterEvent( QDragEnterEvent* InEvent )
{
	if ( InEvent->mimeData()->hasUrls() )
	{
		QList<QUrl>			urls = InEvent->mimeData()->urls();
		QFileInfo			fileInfo;

		for ( auto it = urls.begin(), itEnd = urls.end(); it != itEnd; ++it )
		{
			fileInfo.setFile( it->toLocalFile() );

			if ( fileInfo.isSymLink() )
			{
				InEvent->ignore();
				return;
			}
		}

		QTreeView::dragEnterEvent( InEvent );
	}
	else
	{
		InEvent->ignore();
	}
}

void WeContentBrowser::dragMoveEvent( QDragMoveEvent* InEvent )
{
	if ( InEvent->mimeData()->hasUrls() )
	{
		QTreeView::dragMoveEvent( InEvent );
	}
	else
	{
		InEvent->ignore();
	}
}

void WeContentBrowser::dropEvent( QDropEvent* InEvent )
{
	if ( InEvent->mimeData()->hasUrls() )
	{
		QString				routeToDropFile = InEvent->mimeData()->urls()[ 0 ].toLocalFile();
		if ( routeToDropFile.indexOf( rootDir.absolutePath() ) != -1 )
		{
			InEvent->setDropAction( Qt::MoveAction );
		}
		else
		{
			InEvent->setDropAction( Qt::CopyAction );
		}

		QTreeView::dropEvent( InEvent );
	}
	else
	{
		InEvent->ignore();
	}
}

void WeContentBrowser::on_treeView_contentBrowser_clicked( const QModelIndex& InIndex )
{
	QFileInfo		fileInfo = fileSystemModel->fileInfo( InIndex );
	if ( fileInfo.isFile() && fileInfo.suffix() == FILE_PACKAGE_EXTENSION )
	{
		FPackageRef		package = GPackageManager->LoadPackage( appQtAbsolutePathToEngine( fileInfo.absoluteFilePath() ) );
		if ( package )
		{

			emit OnOpenPackage( package );
		}

		repaint();
	}
}