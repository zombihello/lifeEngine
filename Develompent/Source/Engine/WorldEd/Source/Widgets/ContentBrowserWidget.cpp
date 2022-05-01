#include <qtextstream.h>
#include <qfile.h>
#include <qmenu.h>
#include <qdebug.h>
#include <qmessagebox.h>
#include <qinputdialog.h>
#include <qfiledialog.h>
#include <qdesktopservices.h>

#include "Containers/String.h"
#include "Containers/StringConv.h"
#include "Commandlets/CookPackagesCommandlet.h"
#include "Commandlets/ImportMeshCommandlet.h"
#include "Widgets/ContentBrowserWidget.h"
#include "System/ContentBrowser.h"
#include "Render/Material.h"
#include "WorldEd.h"
#include "ui_ContentBrowserWidget.h"

static FORCEINLINE void DeleteDir( const QString& InRouteDir )
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

static FORCEINLINE QString GetFilterOfSupportedFormats()
{
	static QString		result;
	if ( !result.isEmpty() )
	{
		return result;
	}

	std::wstring		allSupportedFormats;

	// Texture formats
	{
		const std::vector< std::wstring >&		textureExtension = LCookPackagesCommandlet::GetSupportedTextureExtensins();
		if ( !textureExtension.empty() )
		{
			result += "Texture formats (";
			for ( uint32 index = 0, count = textureExtension.size(); index < count; ++index )
			{
				std::wstring		format = FString::Format( TEXT( "*.%s%s" ), textureExtension[ index ].c_str(), index + 1 < count ? TEXT( ";" ) : TEXT( "" ) );
				result += format;
				allSupportedFormats += format;
			}
			result += ");;";
		}
	}

	// Audio formats
	{
		const std::vector< std::wstring >&		audioExtnesion = LCookPackagesCommandlet::GetSupportedAudioExtensins();
		if ( !audioExtnesion.empty() )
		{
			allSupportedFormats += !allSupportedFormats.empty() ? TEXT( ";" ) : TEXT( "" );
			result += "Audio formats (";
			for ( uint32 index = 0, count = audioExtnesion.size(); index < count; ++index )
			{
				std::wstring		format = FString::Format( TEXT( "*.%s%s" ), audioExtnesion[ index ].c_str(), index + 1 < count ? TEXT( ";" ) : TEXT( "" ) );
				result				+= format;
				allSupportedFormats += format;
			}
			result += ");;";
		}
	}

	// All supported formats
	{
		result += FString::Format( TEXT( "All supported formats (%s)" ), allSupportedFormats.c_str() );
	}

	return result;
}

static FORCEINLINE QMessageBox::StandardButton ShowDeleteFileMessageBox( QWidget* InParent, const QString& InTitle, const QString& InText, const QModelIndexList& InModelIndexList, QFileSystemModel* InFileSystemModel )
{
	QString							filesToDelete;
	check( InFileSystemModel );

	// Forming a list of deleted files
	for ( uint32 index = 0, count = InModelIndexList.length(); index < count; ++index )
	{
		QFileInfo		fileInfo	= InFileSystemModel->fileInfo( InModelIndexList[ index ] );
		QString			tempString	= fileInfo.absoluteFilePath();
		tempString.remove( 0, tempString.indexOf( "Content" ) );
		filesToDelete += tempString;

		if ( count > 3 && index + 1 == 3 )
		{
			filesToDelete += "<br>...<br>";
			break;
		}
		else
		{
			filesToDelete += "<br>";
		}
	}

	QString		finalText = QString::fromStdWString( FString::Format( TEXT( "%s<br><br><b>Files:</b><br>%s" ), InText.toStdWString().c_str(), filesToDelete.toStdWString().c_str() ) );
	return QMessageBox::warning( InParent, InTitle, finalText, QMessageBox::Cancel | QMessageBox::Ok );
}

static FORCEINLINE QMessageBox::StandardButton ShowDeleteAssetMessageBox( QWidget* InParent, const QString& InTitle, const QString& InText, const QModelIndexList& InModelIndexList, FPackage* InPackage )
{
	QString							assetsToDelete;
	check( InPackage );

	// Forming a list of deleted files
	for ( uint32 index = 0, count = InModelIndexList.length(); index < count; ++index )
	{
		FAssetInfo		assetInfo;
		InPackage->GetAssetInfo( InModelIndexList[ index ].row(), assetInfo );

		assetsToDelete += QString::fromStdWString( assetInfo.name );
		if ( count > 3 && index + 1 == 3 )
		{
			assetsToDelete += "<br>...<br>";
			break;
		}
		else
		{
			assetsToDelete += "<br>";
		}
	}

	QString		finalText = QString::fromStdWString( FString::Format( TEXT( "%s<br><br><b>Assets:</b><br>%s" ), InText.toStdWString().c_str(), assetsToDelete.toStdWString().c_str() ) );
	return QMessageBox::warning( InParent, InTitle, finalText, QMessageBox::Cancel | QMessageBox::Ok );
}

WeContentBrowserWidget::WeContentBrowserWidget( ERootDir InRootDir /* = RD_Game */, QWidget* InParent /* = nullptr */ )
	: QWidget( InParent )
	, ui( new Ui::WeContentBrowserWidget() )
{
	// Init Qt UI
	ui->setupUi( this );
	SetRootDir( InRootDir );

	// Connect to slots
	connect( ui->treeView_contentBrowser, SIGNAL( OnClickedInEmptySpace() ), this, SLOT( OnContentBrowserClickedInEmptySpace() ) );
	connect( ui->treeView_contentBrowser, SIGNAL( OnSelectedPackage( FPackage* ) ), this, SLOT( OnContentBrowserSelectedPackage( FPackage* ) ) );
}

WeContentBrowserWidget::~WeContentBrowserWidget()
{
	delete ui;
}

void WeContentBrowserWidget::SetRootDir( ERootDir InRootDir )
{
	/** List of root directories */
	static std::wstring		GRootDirs[] =
	{
		FString::Format( TEXT( "%s/Content/" ), appGameDir().c_str() ),				// RD_Game
		FString::Format( TEXT( "%s/Engine/Content/" ), appBaseDir().c_str() )		// RD_Engine
	};

	check( InRootDir >= 0 && InRootDir < RD_Max );
	ui->treeView_contentBrowser->SetRootDir( QFileInfo( QString::fromStdWString( GRootDirs[ InRootDir ] ) ).dir() );
}

void WeContentBrowserWidget::on_treeView_contentBrowser_customContextMenuRequested( QPoint InPoint )
{
	// Create menu content for each format of file 
	QModelIndex				modelIndex		= ui->treeView_contentBrowser->indexAt( InPoint );
	QModelIndexList			modelIndexList = ui->treeView_contentBrowser->selectionModel()->selectedRows();
	QFileSystemModel*		fileSystemModel = ui->treeView_contentBrowser->GetFileSystemModel();
	QFileInfo				fileInfo		= fileSystemModel->fileInfo( modelIndex );
	uint32					numSelecteItems = modelIndexList.length();

	// Menu 'Create'
	QMenu			menuCreate( "Create", this );
	QAction			actionCreateFolder( "Folder", this );
	QAction			actionCreatePackage( "Package", this );
	menuCreate.addAction( &actionCreateFolder );
	menuCreate.addAction( &actionCreatePackage );

	// Main menu
	QMenu			contextMenu( this );
	QAction			actionSavePackage( "Save", this );					//
	QAction			actionOpenPackage( "Open", this );					// } Actions with package
	QAction			actionUnloadPackage( "Unload", this );				//
	QAction			actionShowInExplorer( "Show In Explorer", this );	//
	QAction			actionOpenLevel( "Open Level", this );				//
	QAction			actionSaveLevel( "Save Level", this );				// } Actions with map
	QAction			actionDeleteFile( "Delete", this );					//
	QAction			actionRenameFile( "Rename", this );					// } Actions with all files
	contextMenu.addAction( &actionSavePackage );
	contextMenu.addAction( &actionOpenPackage );
	contextMenu.addAction( &actionUnloadPackage );
	contextMenu.addSeparator();
	contextMenu.addAction( &actionShowInExplorer );
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
		actionShowInExplorer.setEnabled( false );
	}

	// If in selection list exist not loaded package - disable rename action
	for ( uint32 index = 0, count = modelIndexList.length(); index < count; ++index )
	{
		QFileInfo		fileInfo = fileSystemModel->fileInfo( modelIndexList[ index ] );
		if ( !GPackageManager->IsPackageLoaded( appQtAbsolutePathToEngine( fileInfo.absoluteFilePath() ) ) )
		{
			actionSavePackage.setEnabled( false );
			break;
		}
	}

	// Disable actions if selected more of one item
	if ( numSelecteItems > 1 )
	{
		actionRenameFile.setEnabled( false );
		actionShowInExplorer.setEnabled( false );
	}

	// Connect to signal of actions
	connect( &actionDeleteFile, SIGNAL( triggered() ), this, SLOT( on_treeView_contentBrowser_contextMenu_delete() ) );
	connect( &actionRenameFile, SIGNAL( triggered() ), this, SLOT( on_treeView_contentBrowser_contextMenu_rename() ) );
	connect( &actionCreateFolder, SIGNAL( triggered() ), this, SLOT( on_treeView_contentBrowser_contextMenu_createFolder() ) );
	connect( &actionCreatePackage, SIGNAL( triggered() ), this, SLOT( on_treeView_contentBrowser_contextMenu_createPackage() ) );
	connect( &actionSavePackage, SIGNAL( triggered() ), this, SLOT( on_treeView_contentBrowser_contextMenu_SavePackage() ) );
	connect( &actionOpenPackage, SIGNAL( triggered() ), this, SLOT( on_treeView_contentBrowser_contextMenu_OpenPackage() ) );
	connect( &actionUnloadPackage, SIGNAL( triggered() ), this, SLOT( on_treeView_contentBrowser_contextMenu_UnloadPackage() ) );
	connect( &actionShowInExplorer, SIGNAL( triggered() ), this, SLOT( OnContentBrowserContextMenuShowInExplorer() ) );

	contextMenu.exec( QCursor::pos() );
}

void WeContentBrowserWidget::on_listView_packageBrowser_customContextMenuRequested( QPoint InPoint )
{
	QModelIndex				modelIndex		= ui->listView_packageBrowser->indexAt( InPoint );
	FPackageRef				currentPackage	= ui->listView_packageBrowser->GetPackage();
	uint32					numSelecteItems = ui->listView_packageBrowser->selectionModel()->selectedRows().length();

	// Menu 'Create'
	QMenu			menuCreate( "Create", this );
	QAction			actionCreateMaterial( "Material", this );
	menuCreate.addAction( &actionCreateMaterial );

	// Main menu
	QMenu			contextMenu( this );
	QAction			actionImportAsset( "Import Asset", this );
	QAction			actionDeleteFile( "Delete", this );
	QAction			actionRenameFile( "Rename", this );
	contextMenu.addAction( &actionImportAsset );
	contextMenu.addSeparator();
	contextMenu.addMenu( &menuCreate );
	contextMenu.addSeparator();
	contextMenu.addAction( &actionDeleteFile );
	contextMenu.addAction( &actionRenameFile );

	// If current package is not valid - disable all actions
	if ( !currentPackage )
	{
		menuCreate.setEnabled( false );
		actionImportAsset.setEnabled( false );
		actionDeleteFile.setEnabled( false );
		actionRenameFile.setEnabled( false );
	}
	else
	{
		// Disable actions if item is not valid
		if ( !modelIndex.isValid() )
		{
			actionDeleteFile.setEnabled( false );
			actionRenameFile.setEnabled( false );
		}

		// Disable actions if selected more of one item
		if ( numSelecteItems > 1 )
		{
			actionRenameFile.setEnabled( false );
		}
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
	check( package );

	QStringList			selectedAssets = QFileDialog::getOpenFileNames( this, "Select Asset To Import", QString(), GetFilterOfSupportedFormats() );
	if ( selectedAssets.isEmpty() )
	{
		return;
	}

	// Import selected assets
	bool							bReplaceAll		= false;		//
	bool							bSkipAll		= false;		// } Replace or skip all assets with exist names in package
	LCookPackagesCommandlet			cookPackages;
	std::vector< FAssetRef >		loadedAssets;
	for ( uint32 index = 0, count = selectedAssets.size(); index < count; ++index )
	{
		QString			pathToAsset = selectedAssets[ index ];
		FAssetRef		asset;
		QFileInfo		fileInfo( pathToAsset );
		QString			assetName = fileInfo.baseName();
		std::wstring	fileExtension = fileInfo.suffix().toStdWString();

		// Check on already exist asset with name in package
		if ( package->IsExist( assetName.toStdWString() ) )
		{
			if ( bSkipAll )
			{
				continue;
			}

			// Show message if not setted flags bReplaceAll and bSkipAll
			if ( !bReplaceAll && !bSkipAll )
			{
				QMessageBox::StandardButton		resultButton = QMessageBox::question( this, "Question", QString::fromStdWString( FString::Format( TEXT( "Asset with name '<b>%s</b>' already exist in package. Replace it?" ), assetName.toStdWString().c_str() ) ), QMessageBox::Yes | QMessageBox::YesToAll | QMessageBox::No | QMessageBox::NoToAll );
				switch ( resultButton )
				{
				case QMessageBox::YesToAll:
					bReplaceAll = true;

				case QMessageBox::Yes:
					break;

				case QMessageBox::NoToAll:
					bSkipAll = true;

				case QMessageBox::No:
					continue;
				}
			}
		}

		// We import texture
		if ( LCookPackagesCommandlet::IsSupportedTextureExtension( fileExtension ) )
		{
			asset = cookPackages.ConvertTexture2D( appQtAbsolutePathToEngine( pathToAsset ) );
		}

		// We import audio bank
		if ( LCookPackagesCommandlet::IsSupportedAudioExtension( fileExtension ) )
		{	
			asset = cookPackages.ConvertAudioBank( appQtAbsolutePathToEngine( pathToAsset ) );
		}

		// Add asset to package
		if ( asset )
		{
			package->Add( asset );
			loadedAssets.push_back( asset );
		}
	}

	// If we added to package new assets - need refresh package browser and save package
	if ( package->IsDirty() )
	{
		package->Save( package->GetFileName() );
		ui->listView_packageBrowser->Refresh();
	}
}

void WeContentBrowserWidget::on_listView_packageBrowser_CreateMaterial()
{}

void WeContentBrowserWidget::on_listView_packageBrowser_DeleteAsset()
{
	// Getting selected items and current package
	FPackageRef				package			= ui->listView_packageBrowser->GetPackage();
	QModelIndexList			modelIndexList	= ui->listView_packageBrowser->selectionModel()->selectedRows();
	if ( !package )
	{
		return;
	}

	// If we not press 'ok' in message box - exit from method
	if ( ShowDeleteAssetMessageBox( this, "Warning", "Is need delete selected assets?", modelIndexList, package ) != QMessageBox::Ok )
	{
		return;
	}

	// Remove all selected assets from package
	std::vector< FAssetInfo >		assetInfosToDelete;
	for ( uint32 index = 0, count = modelIndexList.length(); index < count; ++index )
	{
		uint32			idAsset = modelIndexList[ index ].row();
		FAssetInfo		assetInfo;
		
		package->GetAssetInfo( idAsset, assetInfo );
		assetInfosToDelete.push_back( assetInfo );
	}

	for ( uint32 index = 0, count = assetInfosToDelete.size(); index < count; ++index )
	{
		package->Remove( assetInfosToDelete[ index ].name );
	}

	// If package is dirty - resave
	if ( package->IsDirty() )
	{
		package->Save( package->GetFileName() );
		ui->listView_packageBrowser->Refresh();
	}
}

void WeContentBrowserWidget::on_listView_packageBrowser_RenameAsset()
{
	// Getting selected items and current package
	FPackageRef				package = ui->listView_packageBrowser->GetPackage();
	QModelIndexList			modelIndexList = ui->listView_packageBrowser->selectionModel()->selectedRows();
	if ( !package )
	{
		return;
	}

	check( modelIndexList.length() == 1 );		// Support rename only first asset
	FAssetInfo		assetInfo;
	package->GetAssetInfo( modelIndexList[ 0 ].row(), assetInfo );		

	// Get new asset name and check on exist other asset with this name
	bool			bIsOk = false;
	QString			newAssetName;
	while ( !bIsOk )
	{
		// Get new asset name. If we not press 'ok' nothing apply and exit from method
		newAssetName = QInputDialog::getText( this, "Enter", "New Asset Name", QLineEdit::Normal, QString::fromStdWString( assetInfo.name ), &bIsOk );
		if ( !bIsOk )
		{
			return;
		}

		// If asset with new name already exist - try enter other name 
		if ( package->IsExist( newAssetName.toStdWString() ) )
		{
			QMessageBox::critical( this, "Error", QString::fromStdWString( FString::Format( TEXT( "Name '%s' already exist in package" ), newAssetName.toStdWString().c_str() ) ), QMessageBox::Ok );
			bIsOk = false;
		}
	}

	// Find asset in package
	FAssetRef		asset = package->Find( assetInfo.name );
	if ( !asset )
	{
		return;
	}

	// Change asset name
	asset->SetAssetName( newAssetName.toStdWString() );

	// Resave package
	package->Save( package->GetFileName() );
}

void WeContentBrowserWidget::on_treeView_contentBrowser_contextMenu_SavePackage()
{
	// Getting selected items and file system model
	QModelIndexList			modelIndexList	= ui->treeView_contentBrowser->selectionModel()->selectedRows();
	QFileSystemModel*		fileSystemModel = ui->treeView_contentBrowser->GetFileSystemModel();

	// Save all selected packages if is need
	for ( uint32 index = 0, count = modelIndexList.length(); index < count; ++index )
	{
		QFileInfo		fileInfo = fileSystemModel->fileInfo( modelIndexList[ index ] );
		std::wstring	pathPackage = appQtAbsolutePathToEngine( fileInfo.absoluteFilePath() );
		
		// If package is loaded - we resave him
		if ( GPackageManager->IsPackageLoaded( pathPackage ) )
		{
			FPackageRef		package = GPackageManager->LoadPackage( pathPackage );
			if ( package && package->IsDirty() )
			{
				package->Save( pathPackage );
			}
		}
	}
}

void WeContentBrowserWidget::on_treeView_contentBrowser_contextMenu_OpenPackage()
{
	// Getting selected items, file system model and current package in package browser
	QModelIndexList			modelIndexList		= ui->treeView_contentBrowser->selectionModel()->selectedRows();
	QFileSystemModel*		fileSystemModel		= ui->treeView_contentBrowser->GetFileSystemModel();
	bool					bIsNeedRepaint		= false;
	FPackageRef				lastLoadedPackage;

	// Load packages
	for ( uint32 index = 0, count = modelIndexList.length(); index < count; ++index )
	{
		QFileInfo		fileInfo	= fileSystemModel->fileInfo( modelIndexList[ index ] );
		std::wstring	pathPackage = appQtAbsolutePathToEngine( fileInfo.absoluteFilePath() );
		FPackageRef		package		= GPackageManager->LoadPackage( pathPackage );
		
		if ( package )
		{
			lastLoadedPackage = package;
			bIsNeedRepaint = true;
		}
	}

	// Set last loaded package for view in package browser
	if ( lastLoadedPackage )
	{
		ui->listView_packageBrowser->SetPackage( lastLoadedPackage );
	}

	// Repaint content browser is need
	if ( bIsNeedRepaint )
	{
		ui->treeView_contentBrowser->repaint();
	}
}

void WeContentBrowserWidget::on_treeView_contentBrowser_contextMenu_UnloadPackage()
{
	// Getting selected items, file system model and current package in package browser
	QModelIndexList			modelIndexList		= ui->treeView_contentBrowser->selectionModel()->selectedRows();
	QFileSystemModel*		fileSystemModel		= ui->treeView_contentBrowser->GetFileSystemModel();
	FPackageRef				currentPackage		= ui->listView_packageBrowser->GetPackage();

	// Unload packages
	bool		bIsNeedRepaint = false;
	ui->listView_packageBrowser->SetPackage( nullptr );
	
	for ( int index = 0, count = modelIndexList.length(); index < count; ++index )
	{
		QFileInfo		fileInfo	= fileSystemModel->fileInfo( modelIndexList[ index ] );
		std::wstring	pathPackage = appQtAbsolutePathToEngine( fileInfo.absoluteFilePath() );
		bool			bSeccussed	= GPackageManager->UnloadPackage( pathPackage );

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
	QModelIndexList					modelIndexList		= ui->treeView_contentBrowser->selectionModel()->selectedRows();
	QFileSystemModel*				fileSystemModel		= ui->treeView_contentBrowser->GetFileSystemModel();
	FPackageRef						currentPackage		= ui->listView_packageBrowser->GetPackage();
	QString							filesForDelete;

	// If we not press 'ok' in message box - exit from method
	if ( ShowDeleteFileMessageBox( this, "Warning", "Is need delete selected files?", modelIndexList, fileSystemModel ) != QMessageBox::Ok )
	{
		return;
	}

	// Delete all selected files
	for ( uint32 index = 0, count = modelIndexList.length(); index < count; ++index )
	{
		QFileInfo		fileInfo = fileSystemModel->fileInfo( modelIndexList[ index ] );
		if ( fileInfo.isDir() )
		{
			DeleteDir( fileInfo.absoluteFilePath() );
		}
		else
		{
			QFile( fileInfo.absoluteFilePath() ).remove();
		}

		// If this is  package and him opened in package browser, we close it
		if ( currentPackage && fileInfo.suffix() == FILE_PACKAGE_EXTENSION && appQtAbsolutePathToEngine( fileInfo.absoluteFilePath() ) == currentPackage->GetFileName() )
		{
			ui->listView_packageBrowser->SetPackage( nullptr );
		}
	}
}

void WeContentBrowserWidget::on_treeView_contentBrowser_contextMenu_rename()
{
	bool				isRename			= false;
	QFileSystemModel*	fileSystemModel		= ui->treeView_contentBrowser->GetFileSystemModel();
	QFileInfo			fileInfo			= fileSystemModel->fileInfo( ui->treeView_contentBrowser->currentIndex() );
	QString				newFileName;

	// Get new file name
	if ( fileInfo.isDir() )
	{
		newFileName = QInputDialog::getText( this, "Enter", "New Directory Name", QLineEdit::Normal, fileInfo.baseName(), &isRename );
	}
	else
	{
		newFileName = QInputDialog::getText( this, "Enter", "New File Name", QLineEdit::Normal, fileInfo.baseName(), &isRename );
	}

	// If we not press 'ok' nothing apply and exit from method
	if ( !isRename || newFileName.isEmpty() )
	{
		return;
	}

	// If this file is directory
	if ( fileInfo.isDir() )
	{
		QDir			dir;
		dir.rename( fileInfo.absoluteFilePath(), fileInfo.absolutePath() + "/" + newFileName );
	}

	// Else it's generic file
	else
	{
		QFile			file;

		// If this is package need call SetName in him
		if ( fileInfo.suffix() == FILE_PACKAGE_EXTENSION )
		{
			std::wstring	pathPackage				= appQtAbsolutePathToEngine( fileInfo.absoluteFilePath() );
			bool			bIsNeedUnloadPackage	= !GPackageManager->IsPackageLoaded( pathPackage );
			FPackageRef		package					= GPackageManager->LoadPackage( pathPackage );

			// If package failed loaded - we not change him name
			if ( !package )
			{
				QMessageBox::critical( this, "Error", QString::fromStdWString( FString::Format( TEXT( "File <b>'%s'</b> not renamed because failed loading package for change him name" ), pathPackage.c_str() ) ), QMessageBox::Ok );
				return;
			}

			// Else we change name and resave package
			package->SetName( newFileName.toStdWString() );
			package->Save( package->GetFileName() );

			// If need unload package - do it
			if ( bIsNeedUnloadPackage )
			{
				GPackageManager->UnloadPackage( pathPackage );
			}
		}

		file.rename( fileInfo.absoluteFilePath(), fileInfo.absolutePath() + "/" + newFileName + "." + fileInfo.suffix() );
	}
}

void WeContentBrowserWidget::on_treeView_contentBrowser_contextMenu_createFolder()
{
	bool				isOk = false;
	QFileSystemModel*	fileSystemModel		= ui->treeView_contentBrowser->GetFileSystemModel();
	QFileInfo			fileInfo			= fileSystemModel->fileInfo( ui->treeView_contentBrowser->currentIndex() );
	QString				dirName;

	// Get directory name
	dirName = QInputDialog::getText( this, "Enter", "Enter Directory Name", QLineEdit::Normal, "NewFolder", &isOk );

	// If we not press 'ok' nothing apply and exit from method
	if ( !isOk || dirName.isEmpty() )
	{
		return;
	}

	// Rename directory
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
	bool				isOk				= false;
	QFileSystemModel*	fileSystemModel		= ui->treeView_contentBrowser->GetFileSystemModel();
	QFileInfo			fileInfo			= fileSystemModel->fileInfo( ui->treeView_contentBrowser->currentIndex() );
	
	// Get package name
	QString				packageName = QInputDialog::getText( this, "Enter", "Enter Package Name", QLineEdit::Normal, "NewPackage", &isOk );
	
	// If we not press 'ok' nothing apply and exit from method
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
	std::wstring	fullPath	= appQtAbsolutePathToEngine( dir.absolutePath() + "/" + packageName + "." FILE_PACKAGE_EXTENSION );
	FPackageRef		package		= GPackageManager->LoadPackage( fullPath, true );
	package->Save( fullPath );
	GPackageManager->UnloadPackage( fullPath );
}

void WeContentBrowserWidget::OnContentBrowserClickedInEmptySpace()
{
	ui->listView_packageBrowser->SetPackage( nullptr );
}

void WeContentBrowserWidget::OnContentBrowserSelectedPackage( FPackage* InPackage )
{
	ui->listView_packageBrowser->SetPackage( InPackage );
}

void WeContentBrowserWidget::OnContentBrowserContextMenuShowInExplorer()
{
	QModelIndex				modelIndex = ui->treeView_contentBrowser->currentIndex();
	if ( !modelIndex.isValid() )
	{
		return;
	}

	QFileSystemModel*	fileSystemModel = ui->treeView_contentBrowser->GetFileSystemModel();
	QFileInfo			fileInfo		= fileSystemModel->fileInfo( modelIndex );
	QString				pathToDir		= fileInfo.absolutePath();
	if ( fileInfo.isDir() )
	{
		pathToDir += "/" + fileInfo.baseName();
	}

	QDesktopServices::openUrl( QUrl::fromLocalFile( pathToDir ) );
}

void WeContentBrowserWidget::on_comboBox_rootDir_currentIndexChanged( int InIndex )
{
	SetRootDir( ( ERootDir )InIndex );
}