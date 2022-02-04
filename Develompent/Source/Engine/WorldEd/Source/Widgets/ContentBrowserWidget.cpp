#include <qtextstream.h>
#include <qfile.h>
#include <qmenu.h>
#include <qdebug.h>
#include <qmessagebox.h>
#include <qinputdialog.h>

#include "Containers/String.h"
#include "Containers/StringConv.h"
#include "Widgets/ContentBrowserWidget.h"
#include "System/ContentBrowser.h"
#include "ui_ContentBrowserWidget.h"

/**
 * Delete directory
 */
void DeleteDir( const QString& InRouteDir )
{
	QDir				dir( InRouteDir );
	QFile				file;
	QFileInfoList		entryInfoList = dir.entryInfoList( QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Filter::Hidden );

	for ( auto it = entryInfoList.begin(), itEnd = entryInfoList.end(); it != itEnd; ++it )
	{
		if ( it->isDir() )
		{
			DeleteDir( it->absoluteFilePath() );
		}
		else
		{
			QFile::setPermissions( it->absoluteFilePath(), QFile::ReadOwner | QFile::WriteOwner );
			QFile::remove( it->absoluteFilePath() );
		}
	}

	dir.rmdir( InRouteDir );
}

WeContentBrowserWidget::WeContentBrowserWidget( const tchar* InRootDir, QWidget* InParent /* = nullptr */ )
	: QWidget( InParent )
	, ui( new Ui::WeContentBrowserWidget() )
{
	// Init Qt UI
	ui->setupUi( this );

	// Set root dir for content browser
	check( InRootDir );
	ui->treeView_contentBrowser->SetRootDir( QFileInfo( QString::fromStdWString( InRootDir ) ).dir() );

	// Connect to slots
	connect( ui->treeView_contentBrowser, SIGNAL( OnOpenPackage() ), this, SLOT( on_treeView_contentBrowser_OnOpenPackage( class FPackage* InPackage ) ) );
}

WeContentBrowserWidget::~WeContentBrowserWidget()
{
	delete ui;
}

void WeContentBrowserWidget::on_treeView_contentBrowser_customContextMenuRequested( QPoint InPoint )
{
	QMenu			contextMenu( this );
	QAction			actionDelete( "Delete", this);
	QAction			actionRename( "Rename", this);

	QMenu			menuCreate( "Create", this);
	QAction			actionCreateFolder( "Folder", this);
	QAction			actionCreateMaterial( "Material", this );

	menuCreate.addAction( &actionCreateFolder );
	menuCreate.addAction( &actionCreateMaterial );

	if ( !ui->treeView_contentBrowser->indexAt( InPoint ).isValid() )
	{
		actionDelete.setEnabled( false );
		actionRename.setEnabled( false );
	}
	else
	{
		if ( ui->treeView_contentBrowser->selectionModel()->selectedRows().length() > 1 )
		{
			actionRename.setEnabled( false );
		}
	}

	connect( &actionDelete, SIGNAL( triggered() ), this, SLOT( on_treeView_contentBrowser_contextMenu_delete() ) );
	connect( &actionRename, SIGNAL( triggered() ), this, SLOT( on_treeView_contentBrowser_contextMenu_rename() ) );
	connect( &actionCreateFolder, SIGNAL( triggered() ), this, SLOT( on_treeView_contentBrowser_contextMenu_createFolder() ) );
	connect( &actionCreateMaterial, SIGNAL( triggered() ), this, SLOT( on_treeView_contentBrowser_contextMenu_createMaterial() ) );

	contextMenu.addMenu( &menuCreate );
	contextMenu.addAction( &actionDelete );
	contextMenu.addAction( &actionRename );
	contextMenu.addAction( contextMenu.addSeparator() );

	contextMenu.exec( QCursor::pos() );
}

void WeContentBrowserWidget::on_treeView_contentBrowser_contextMenu_delete()
{
	QModelIndexList					modelIndexList = ui->treeView_contentBrowser->selectionModel()->selectedRows();
	QFileSystemModel*				fileSystemModel = ui->treeView_contentBrowser->GetFileSystemModel();
	QString							filesForDelete;
	QString							tempString;
	QFileInfo						fileInfo;

	// Forming a list of deleted files
	for ( int index = 0, count = modelIndexList.length(); index < count; ++index )
	{
		fileInfo = fileSystemModel->fileInfo( modelIndexList[ index ] );
		tempString = fileInfo.absoluteFilePath();
		tempString.remove( 0, tempString.indexOf( "Content" ) );

		filesForDelete += tempString;

		if ( count > 3 && index + 1 == 3 )
		{
			filesForDelete += "<br>...<br>";
			break;
		}
		else
		{
			filesForDelete += "<br>";
		}
	}

	QMessageBox::StandardButton		resultMessageBox;
	if ( modelIndexList.length() == 1 )
	{
		resultMessageBox = QMessageBox::warning( this, "Warning", QString::fromStdWString( FString::Format( TEXT( "Delete selected asset:\n%s"), filesForDelete.toStdWString().c_str() ) ), QMessageBox::Cancel | QMessageBox::Ok);
	}
	else
	{
		resultMessageBox = QMessageBox::warning( this, "Warning", QString::fromStdWString( FString::Format( TEXT( "Delete selected assets:\n%s" ), filesForDelete.toStdWString().c_str() ) ), QMessageBox::Cancel | QMessageBox::Ok );
	}

	if ( resultMessageBox != QMessageBox::Ok )
	{
		return;
	}

	// Delete all selected files
	for ( int index = 0, count = modelIndexList.length(); index < count; ++index )
	{
		fileInfo = fileSystemModel->fileInfo( modelIndexList[ index ] );
		if ( fileInfo.isDir() )
		{
			DeleteDir( fileInfo.absoluteFilePath() );
		}
		else
		{
			QFile( fileInfo.absoluteFilePath() ).remove();
		}
	}
}

void WeContentBrowserWidget::on_treeView_contentBrowser_contextMenu_rename()
{
	bool				isRename = false;
	QFileSystemModel*	fileSystemModel = ui->treeView_contentBrowser->GetFileSystemModel();
	QFileInfo			fileInfo = fileSystemModel->fileInfo( ui->treeView_contentBrowser->currentIndex() );
	QString				newFileName;

	if ( fileInfo.isDir() )
	{
		newFileName = QInputDialog::getText( this, "Enter", "New dir name", QLineEdit::Normal, fileInfo.baseName(), &isRename);
	}
	else
	{
		newFileName = QInputDialog::getText( this, "Enter", "New file name", QLineEdit::Normal, fileInfo.baseName(), &isRename);
	}

	if ( !isRename || newFileName.isEmpty() )
	{
		return;
	}

	if ( fileInfo.isDir() )
	{
		QDir			dir;
		dir.rename( fileInfo.absoluteFilePath(), fileInfo.absolutePath() + "/" + newFileName );
	}
	else
	{
		QFile			file;
		file.rename( fileInfo.absoluteFilePath(), fileInfo.absolutePath() + "/" + newFileName + "." + fileInfo.suffix() );
	}
}

void WeContentBrowserWidget::on_treeView_contentBrowser_contextMenu_createFolder()
{
	bool				isRename = false;
	QFileSystemModel*	fileSystemModel = ui->treeView_contentBrowser->GetFileSystemModel();
	QFileInfo			fileInfo = fileSystemModel->fileInfo( ui->treeView_contentBrowser->currentIndex() );
	QString				dirName;

	dirName = QInputDialog::getText( this, "Enter", "Enter dir name", QLineEdit::Normal, "new folder", &isRename);
	if ( !isRename || dirName.isEmpty() ) return;

	QDir			dir;
	if ( fileInfo.fileName().isEmpty() )
	{
		dir = ui->treeView_contentBrowser->GetRootDir();
	}
	else if ( !fileInfo.isDir() )
	{
		dir.cd( fileInfo.absolutePath() );
	}
	else
	{
		dir.cd( fileInfo.absoluteFilePath() );
	}

	dir.mkdir( dirName );
}

void WeContentBrowserWidget::on_treeView_contentBrowser_contextMenu_createMaterial()
{}

void WeContentBrowserWidget::on_treeView_contentBrowser_OnOpenPackage( class FPackage* InPackage )
{
	ui->listView_packageBrowser->SetPackage( InPackage );
}