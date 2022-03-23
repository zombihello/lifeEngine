#include <qtextstream.h>
#include <qfile.h>
#include <qmenu.h>
#include <qdebug.h>
#include <qmessagebox.h>
#include <qinputdialog.h>
#include <qfiledialog.h>

#include "Containers/String.h"
#include "Containers/StringConv.h"
#include "Commandlets/ImportTextureCommandlet.h"
#include "Commandlets/ImportMeshCommandlet.h"
#include "Widgets/ContentBrowserWidget.h"
#include "System/ContentBrowser.h"
#include "Render/Material.h"
#include "WorldEd.h"
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
	// Create menu content for each format of file 
	QModelIndex				modelIndex = ui->treeView_contentBrowser->indexAt( InPoint );
	QFileSystemModel*		fileSystemModel = ui->treeView_contentBrowser->GetFileSystemModel();
	QFileInfo				fileInfo = fileSystemModel->fileInfo( modelIndex );
	
	// Menu 'Create'
	QMenu			menuCreate( "Create", this );
	QAction			actionCreateFolder( "Folder", this );
	QAction			actionCreatePackage( "Package", this );
	menuCreate.addAction( &actionCreateFolder );
	menuCreate.addAction( &actionCreatePackage );

	// Main menu
	QMenu			contextMenu( this );
	QAction			actionSavePackage( "Save", this );				//
	QAction			actionOpenPackage( "Open", this );				// } Actions with package
	QAction			actionUnloadPackage( "Unload", this );			//
	QAction			actionOpenLevel( "Open level", this );			//
	QAction			actionSaveLevel( "Save level", this );			// } Actions with map
	QAction			actionDeleteFile( "Delete", this );				//
	QAction			actionRenameFile( "Rename", this );				// } Actions with all files
	contextMenu.addAction( &actionSavePackage );
	contextMenu.addAction( &actionOpenPackage );
	contextMenu.addAction( &actionUnloadPackage );
	contextMenu.addSeparator();
	contextMenu.addAction( &actionOpenLevel );
	contextMenu.addAction( &actionSaveLevel );
	contextMenu.addSeparator();
	contextMenu.addMenu( &menuCreate );
	contextMenu.addAction( &actionDeleteFile );
	contextMenu.addAction( &actionRenameFile );

	// Disable actions with package if current file is not package
	if ( fileInfo.suffix() != FILE_PACKAGE_EXTENSION )
	{
		actionSavePackage.setEnabled( false );
		actionOpenPackage.setEnabled( false );
		actionUnloadPackage.setEnabled( false );
	}
	else
	{
		bool		bIsPackageOpened = GPackageManager->IsPackageLoaded( appQtAbsolutePathToEngine( fileInfo.absoluteFilePath() ) );
		actionOpenPackage.setEnabled( !bIsPackageOpened );
		actionUnloadPackage.setEnabled( bIsPackageOpened );
	}

	// Disable actions with map if current file is not map
	if ( fileInfo.suffix() != FILE_MAP_EXTENSION )
	{
		actionOpenLevel.setEnabled( false );
		actionSaveLevel.setEnabled( false );
	}

	// Disable actions if current file is not valid
	if ( !modelIndex.isValid() )
	{
		actionDeleteFile.setEnabled( false );
		actionRenameFile.setEnabled( false );
	}

	// Connect to signal of acntions
	connect( &actionDeleteFile, SIGNAL( triggered() ), this, SLOT( on_treeView_contentBrowser_contextMenu_delete() ) );
	connect( &actionRenameFile, SIGNAL( triggered() ), this, SLOT( on_treeView_contentBrowser_contextMenu_rename() ) );
	connect( &actionCreateFolder, SIGNAL( triggered() ), this, SLOT( on_treeView_contentBrowser_contextMenu_createFolder() ) );
	connect( &actionCreatePackage, SIGNAL( triggered() ), this, SLOT( on_treeView_contentBrowser_contextMenu_createPackage() ) );
	connect( &actionSavePackage, SIGNAL( triggered() ), this, SLOT( on_treeView_contentBrowser_contextMenu_SavePackage() ) );
	connect( &actionOpenPackage, SIGNAL( triggered() ), this, SLOT( on_treeView_contentBrowser_contextMenu_OpenPackage() ) );
	connect( &actionUnloadPackage, SIGNAL( triggered() ), this, SLOT( on_treeView_contentBrowser_contextMenu_UnloadPackage() ) );

	contextMenu.exec( QCursor::pos() );
}

void WeContentBrowserWidget::on_listView_packageBrowser_customContextMenuRequested( QPoint InPoint )
{
	QModelIndex				modelIndex = ui->listView_packageBrowser->indexAt( InPoint );

	// Menu 'Create'
	QMenu			menuCreate( "Create", this );
	QAction			actionCreateMaterial( "Material", this );
	menuCreate.addAction( &actionCreateMaterial );

	// Main menu
	QMenu			contextMenu( this );
	QAction			actionImportAsset( "Import asset", this );
	QAction			actionDeleteFile( "Delete", this );
	QAction			actionRenameFile( "Rename", this );
	contextMenu.addAction( &actionImportAsset );
	contextMenu.addSeparator();
	contextMenu.addMenu( &menuCreate );
	contextMenu.addSeparator();
	contextMenu.addAction( &actionDeleteFile );
	contextMenu.addAction( &actionRenameFile );

	// Disable actions if item is not valid
	if ( !modelIndex.isValid() )
	{
		actionDeleteFile.setEnabled( false );
		actionRenameFile.setEnabled( false );
	}

	// Connect to signal of a actions
	connect( &actionImportAsset, SIGNAL( triggered() ), this, SLOT( on_listView_packageBrowser_ImportAsset() ) );
	connect( &actionCreateMaterial, SIGNAL( triggered() ), this, SLOT( on_listView_packageBrowser_CreateMaterial() ) );
	connect( &actionDeleteFile, SIGNAL( triggered() ), this, SLOT( on_listView_packageBrowser_DeleteAsset() ) );
	connect( &actionRenameFile, SIGNAL( triggered() ), this, SLOT( on_listView_packageBrowser_RenameAsset() ) );

	contextMenu.exec( QCursor::pos() );
}

void WeContentBrowserWidget::on_listView_packageBrowser_ImportAsset()
{
	FPackageRef		package = ui->listView_packageBrowser->GetPackage();
	QString			supportedFileExtensions;
	check( package );

	// Texture formats
	{
		QString									finalString;
		const std::vector< std::wstring >&		textureExtension = LImportTextureCommandlet::GetSupportedExtensins();
		
		finalString += "Texture formats (";
		for ( uint32 index = 0, count = textureExtension.size(); index < count; ++index )
		{
			finalString += FString::Format( TEXT( "*.%s%s" ), textureExtension[ index ].c_str(), index + 1 < count ? TEXT( ";" ) : TEXT( "" ) );
		}
		finalString += ");;";
		supportedFileExtensions += finalString;
	}

	// Mesh formats
	{
		QString									finalString;
		const std::vector< std::wstring >&		meshExtension = LImportMeshCommandlet::GetSupportedExtensins();

		finalString += "Mesh formats (";
		for ( uint32 index = 0, count = meshExtension.size(); index < count; ++index )
		{
			finalString += FString::Format( TEXT( "*.%s%s" ), meshExtension[ index ].c_str(), index + 1 < count ? TEXT( ";" ) : TEXT( "" ) );
		}
		finalString += ")";
		supportedFileExtensions += finalString;
	}

	QString			srcPath = QFileDialog::getOpenFileName( this, "Select asset to import", QString(), supportedFileExtensions );
	if ( srcPath.isEmpty() )
	{
		return;
	}

	bool			bSeccussed = false;
	QFileInfo		fileInfo( srcPath );
	std::wstring	fileExtension = fileInfo.suffix().toStdWString();

	// We import texture
	if ( LImportTextureCommandlet::IsSupportedExtension( fileExtension ) )
	{
		std::wstring					engineSrcPath = appQtAbsolutePathToEngine( srcPath );
		std::wstring					engineDstPath = appQtAbsolutePathToEngine( QString::fromStdWString( package->GetFileName() ) );

		LImportTextureCommandlet		importTextureCommandlet;
		FTexture2DRef					texture2D = importTextureCommandlet.ConvertTexture2D( engineSrcPath, fileInfo.baseName().toStdWString() );
		if ( texture2D )
		{
			package->Add( texture2D );
			bSeccussed = package->Save( engineDstPath );
		}
	}

	// We import mesh
	if ( LImportMeshCommandlet::IsSupportedExtension( fileExtension ) )
	{
		std::wstring					engineSrcPath = appQtAbsolutePathToEngine( srcPath );
		std::wstring					engineDstPath = appQtAbsolutePathToEngine( QString::fromStdWString( package->GetFileName() ) );

		LImportMeshCommandlet			importMeshCommandlet;
		FStaticMeshRef					staticMesh = importMeshCommandlet.ConvertStaticMesh( engineSrcPath, fileInfo.baseName().toStdWString() );
		if ( staticMesh )
		{
			package->Add( staticMesh );
			bSeccussed = package->Save( engineDstPath );
		}
	}

	// If we added to package new asset - need refresh package browser
	if ( bSeccussed )
	{
		ui->listView_packageBrowser->Refresh();
	}
}

void WeContentBrowserWidget::on_listView_packageBrowser_CreateMaterial()
{}

void WeContentBrowserWidget::on_listView_packageBrowser_DeleteAsset()
{
	// Getting selected items and current package
	FPackageRef				package = ui->listView_packageBrowser->GetPackage();
	QModelIndexList			modelIndexList = ui->listView_packageBrowser->selectionModel()->selectedRows();
	check( package );

	// Remove all selected assets from package
	for ( int index = 0, count = modelIndexList.length(); index < count; ++index )
	{
	}
}

void WeContentBrowserWidget::on_listView_packageBrowser_RenameAsset()
{}

void WeContentBrowserWidget::on_treeView_contentBrowser_contextMenu_SavePackage()
{
	// Getting selected items and file system model
	QModelIndexList			modelIndexList = ui->treeView_contentBrowser->selectionModel()->selectedRows();
	QFileSystemModel*		fileSystemModel = ui->treeView_contentBrowser->GetFileSystemModel();

	// Save all selected packages if is need
	for ( int index = 0, count = modelIndexList.length(); index < count; ++index )
	{
		QFileInfo		fileInfo = fileSystemModel->fileInfo( modelIndexList[ index ] );
		std::wstring	pathPackage = appQtAbsolutePathToEngine( fileInfo.absoluteFilePath() );
		
		FPackageRef		package = GPackageManager->LoadPackage( pathPackage );
		if ( package && package->IsDirty() )
		{
			package->Save( pathPackage );
		}
	}
}

void WeContentBrowserWidget::on_treeView_contentBrowser_contextMenu_OpenPackage()
{}

void WeContentBrowserWidget::on_treeView_contentBrowser_contextMenu_UnloadPackage()
{
	// Getting selected items, file system model and current package in package browser
	QModelIndexList			modelIndexList = ui->treeView_contentBrowser->selectionModel()->selectedRows();
	QFileSystemModel*		fileSystemModel = ui->treeView_contentBrowser->GetFileSystemModel();
	FPackageRef				currentPackage = ui->listView_packageBrowser->GetPackage();

	// Unload packages
	bool		bIsNeedRepaint = false;
	ui->listView_packageBrowser->SetPackage( nullptr );
	
	for ( int index = 0, count = modelIndexList.length(); index < count; ++index )
	{
		QFileInfo		fileInfo = fileSystemModel->fileInfo( modelIndexList[ index ] );
		std::wstring	pathPackage = appQtAbsolutePathToEngine( fileInfo.absoluteFilePath() );
		bool			bSeccussed = GPackageManager->UnloadPackage( pathPackage );

		// If successed close package, we mark about need repaint content browser
		if ( bSeccussed && !bIsNeedRepaint )
		{
			bIsNeedRepaint = true;
		}

		// If current package in viewer is closed, we forget about him
		if ( currentPackage && bSeccussed && pathPackage == currentPackage->GetFileName() )
		{
			currentPackage.SafeRelease();
		}
	}

	// If current package not closed, we restore viewer
	if ( currentPackage )
	{
		ui->listView_packageBrowser->SetPackage( currentPackage );
	}

	// Repaint content browser is need
	if ( bIsNeedRepaint )
	{
		ui->treeView_contentBrowser->repaint();
	}
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

void WeContentBrowserWidget::on_treeView_contentBrowser_contextMenu_createPackage()
{
	bool				isOk = false;
	QFileSystemModel*	fileSystemModel = ui->treeView_contentBrowser->GetFileSystemModel();
	QFileInfo			fileInfo = fileSystemModel->fileInfo( ui->treeView_contentBrowser->currentIndex() );
	
	QString				packageName = QInputDialog::getText( this, "Enter", "Enter package name", QLineEdit::Normal, "new folder", &isOk );
	if ( !isOk || packageName.isEmpty() )
	{
		return;
	}

	QDir			dir;
	if ( fileInfo.fileName().isEmpty() )
	{
		dir = ui->treeView_contentBrowser->GetRootDir();
	}
	else
	{
		dir.cd( !fileInfo.isDir() ? fileInfo.absolutePath() : fileInfo.absoluteFilePath() );
	}

	// Create package
	std::wstring	fullPath = appQtAbsolutePathToEngine( dir.absolutePath() + "/" + packageName + ".lpak" );
	FPackageRef		package = GPackageManager->LoadPackage( fullPath, true );
	package->Save( fullPath );
	GPackageManager->UnloadPackage( fullPath );
}

void WeContentBrowserWidget::on_treeView_contentBrowser_OnOpenPackage( class FPackage* InPackage )
{
	ui->listView_packageBrowser->SetPackage( InPackage );
}