#include <qtextstream.h>
#include <qfile.h>
#include <qmenu.h>
#include <qdebug.h>
#include <qmessagebox.h>
#include <qinputdialog.h>
#include <qfiledialog.h>
#include <qdesktopservices.h>
#include <qclipboard.h>

#include "Misc/CoreGlobals.h"
#include "Misc/WorldEdGlobals.h"
#include "Containers/String.h"
#include "Containers/StringConv.h"
#include "Commandlets/ImportMeshCommandlet.h"
#include "Widgets/ContentBrowserWidget.h"
#include "Windows/MainWindow.h"
#include "Windows/TextureEditorWindow.h"
#include "Windows/MaterialEditorWindow.h"
#include "System/ContentBrowser.h"
#include "System/EditorEngine.h"
#include "System/AssetDataBase.h"
#include "System/AssetsImport.h"
#include "System/BaseFileSystem.h"
#include "Misc/TableOfContents.h"
#include "Render/Material.h"
#include "WorldEd.h"
#include "ui_ContentBrowserWidget.h"

static FORCEINLINE void DeleteDir( const QString& InRouteDir, bool& OutIsDirtyTOC, std::vector< QString >& OutUsedPackage )
{
	QDir				dir( InRouteDir );
	QFile				file;
	QFileInfoList		entryInfoList = dir.entryInfoList( QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Filter::Hidden );

	for ( auto it = entryInfoList.begin(), itEnd = entryInfoList.end(); it != itEnd; ++it )
	{
		if ( it->isDir() )
		{
			DeleteDir( it->absoluteFilePath(), OutIsDirtyTOC, OutUsedPackage );
		}
		else
		{
			// If this package, we remove entry from TOC file
			if ( it->suffix() == FILE_PACKAGE_EXTENSION )
			{
				std::wstring		enginePath = appQtAbsolutePathToEngine( it->absoluteFilePath() );
				
				// We try unload package for remove unused assets.
				// If package still used, we skip him
				if ( !GPackageManager->UnloadPackage( enginePath ) )
				{
					OutUsedPackage.push_back( it->baseName() );
					continue;
				}

				// Else we remove entry from TOC file
				GTableOfContents.RemoveEntry( enginePath );
				OutIsDirtyTOC = true;
			}

			QFile::setPermissions( it->absoluteFilePath(), QFile::ReadOwner | QFile::WriteOwner );
			QFile::remove( it->absoluteFilePath() );
		}
	}

	dir.rmdir( InRouteDir );
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
	connect( ui->listView_packageBrowser, SIGNAL( OnSelectedAsset( const std::wstring& ) ), this, SLOT( OnListViewPackageBrowserSelectedAsset( const std::wstring& ) ) );
}

WeContentBrowserWidget::~WeContentBrowserWidget()
{
	delete ui;
}

void WeContentBrowserWidget::SetRootDir( ERootDir InRootDir )
{
	check( InRootDir >= 0 && InRootDir < RD_Max );
	
	std::wstring		rootDirectory;
	switch ( InRootDir )
	{
	case RD_Engine:		rootDirectory = GAssetDataBase.GetEngineContentDir();	break;
	case RD_Game:		rootDirectory = GAssetDataBase.GetGameContentDir();		break;

	default:
		appErrorf( TEXT( "Unsupported type root directory '0x%X'" ), InRootDir );
		rootDirectory = TEXT( "Unknown" );
	}

	ui->treeView_contentBrowser->SetRootDir( QFileInfo( QString::fromStdWString( rootDirectory ) ).dir() );
}

void WeContentBrowserWidget::ShowAsset( const std::wstring& InAssetReference )
{
	// If asset reference is not valid, we nothing do
	if ( InAssetReference.empty() )
	{
		return;
	}

	// Parse asset reference
	EAssetType		assetType;
	std::wstring	packageName;
	std::wstring	assetName;
	bool			bResult = ParseReferenceToAsset( InAssetReference, packageName, assetName, assetType );
	if ( !bResult )
	{
		return;
	}

	// Find path to package from TOC table and load him
	FPackageRef		package;
	std::wstring	packagePath;
	{
		packagePath = GTableOfContents.GetPackagePath( packageName );
		if ( !packagePath.empty() )
		{
			package = GPackageManager->LoadPackage( packagePath );
		}
	}

	// If package not loaded, we exit from method
	if ( !package )
	{
		return;
	}

	// Find index of asset in package
	bool	bFinded		= false;
	uint32	indexAsset	= 0;
	uint32	numAssets	= package->GetNumAssets();
	for ( indexAsset = 0; indexAsset < numAssets; ++indexAsset )
	{
		FAssetInfo		assetInfo;
		package->GetAssetInfo( indexAsset, assetInfo );

		if ( assetInfo.type == assetType && assetInfo.name == assetName )
		{
			bFinded = true;
			break;
		}
	}

	// If we finded asset - show in content browser
	if ( bFinded )
	{
		ui->treeView_contentBrowser->SetCurrentFile( QString::fromStdWString( packagePath ) );
		ui->listView_packageBrowser->SetPackage( package );
		ui->listView_packageBrowser->setCurrentIndex( ui->listView_packageBrowser->model()->index( indexAsset, 0 ) );
	}
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
	QAction			actionReloadPackage( "Reload", this );				//
	QAction			actionShowInExplorer( "Show In Explorer", this );	//
	QAction			actionOpenLevel( "Open Level", this );				//
	QAction			actionSaveLevel( "Save Level", this );				// } Actions with map
	QAction			actionDeleteFile( "Delete", this );					//
	QAction			actionRenameFile( "Rename", this );					// } Actions with all files
	contextMenu.addAction( &actionSavePackage );
	contextMenu.addAction( &actionOpenPackage );
	contextMenu.addAction( &actionUnloadPackage );
	contextMenu.addAction( &actionReloadPackage );
	contextMenu.addSeparator();
	contextMenu.addAction( &actionShowInExplorer );
	contextMenu.addSeparator();
	contextMenu.addAction( &actionOpenLevel );
	contextMenu.addAction( &actionSaveLevel );
	contextMenu.addSeparator();
	contextMenu.addMenu( &menuCreate );
	contextMenu.addAction( &actionDeleteFile );
	contextMenu.addAction( &actionRenameFile );

	const bool		bPackage				= fileInfo.suffix() == FILE_PACKAGE_EXTENSION;
	bool			bExistLoadedPackage		= false;
	bool			bExistUnloadedPackage	= false;
	const bool		bMap					= fileInfo.suffix() == FILE_MAP_EXTENSION;
	const bool		bValidItem				= modelIndex.isValid();
	const bool		bSelectedMoreOneItems	= numSelecteItems > 1;

	if ( bPackage )
	{
		for ( uint32 index = 0, count = modelIndexList.length(); index < count && ( !bExistLoadedPackage || !bExistUnloadedPackage ); ++index )
		{
			QFileInfo		fileInfo = fileSystemModel->fileInfo( modelIndexList[ index ] );
			if ( !GPackageManager->IsPackageLoaded( appQtAbsolutePathToEngine( fileInfo.absoluteFilePath() ) ) )
			{
				bExistUnloadedPackage = true;
			}
			else
			{
				bExistLoadedPackage = true;
			}
		}
	}

	actionSavePackage.setEnabled( bPackage && bExistLoadedPackage );				//
	actionOpenPackage.setEnabled( bPackage && bExistUnloadedPackage );				// } Disable actions with package if current file is not package
	actionUnloadPackage.setEnabled( bPackage && bExistLoadedPackage );				//
	actionReloadPackage.setEnabled( bPackage && bExistLoadedPackage );				//
	actionOpenLevel.setEnabled( bMap );												//
	actionSaveLevel.setEnabled( bMap );												// } Disable actions with map if current file is not map
	actionDeleteFile.setEnabled( bValidItem );										//   Disable actions if current file is not valid
	actionRenameFile.setEnabled( bValidItem && !bSelectedMoreOneItems );			// 
	actionShowInExplorer.setEnabled( bValidItem && !bSelectedMoreOneItems );		// } Disable actions if selected more of one item

	// Connect to signal of actions
	connect( &actionDeleteFile, SIGNAL( triggered() ), this, SLOT( on_treeView_contentBrowser_contextMenu_delete() ) );
	connect( &actionRenameFile, SIGNAL( triggered() ), this, SLOT( on_treeView_contentBrowser_contextMenu_rename() ) );
	connect( &actionCreateFolder, SIGNAL( triggered() ), this, SLOT( on_treeView_contentBrowser_contextMenu_createFolder() ) );
	connect( &actionCreatePackage, SIGNAL( triggered() ), this, SLOT( on_treeView_contentBrowser_contextMenu_createPackage() ) );
	connect( &actionSavePackage, SIGNAL( triggered() ), this, SLOT( on_treeView_contentBrowser_contextMenu_SavePackage() ) );
	connect( &actionOpenPackage, SIGNAL( triggered() ), this, SLOT( on_treeView_contentBrowser_contextMenu_OpenPackage() ) );
	connect( &actionUnloadPackage, SIGNAL( triggered() ), this, SLOT( on_treeView_contentBrowser_contextMenu_UnloadPackage() ) );
	connect( &actionReloadPackage, SIGNAL( triggered() ), this, SLOT( on_treeView_contentBrowser_contextMenu_ReloadPackage() ) );
	connect( &actionShowInExplorer, SIGNAL( triggered() ), this, SLOT( OnContentBrowserContextMenuShowInExplorer() ) );
	contextMenu.exec( QCursor::pos() );
}

void WeContentBrowserWidget::on_listView_packageBrowser_customContextMenuRequested( QPoint InPoint )
{
	QModelIndex				modelIndex		= ui->listView_packageBrowser->indexAt( InPoint );
	FPackageRef				currentPackage	= ui->listView_packageBrowser->GetPackage();
	uint32					numSelecteItems = ui->listView_packageBrowser->selectionModel()->selectedRows().length();
	QModelIndexList			modelIndexList = ui->listView_packageBrowser->selectionModel()->selectedRows();

	// Menu 'Create'
	QMenu			menuCreate( "Create", this );
	QAction			actionCreateMaterial( "Material", this );
	menuCreate.addAction( &actionCreateMaterial );

	// Main menu
	QMenu			contextMenu( this );
	QAction			actionReload( "Reload", this );
	QAction			actionImport( "Import", this );
	QAction			actionReimport( "Reimport", this );
	QAction			actionReimportWithNewFile( "Reimport With New File", this );
	QAction			actionDeleteFile( "Delete", this );
	QAction			actionRenameFile( "Rename", this );
	QAction			actionCopyReference( "Copy Reference", this );
	contextMenu.addMenu( &menuCreate );
	contextMenu.addSeparator();
	contextMenu.addAction( &actionReload );
	contextMenu.addAction( &actionImport );
	contextMenu.addAction( &actionReimport );
	contextMenu.addAction( &actionReimportWithNewFile );
	contextMenu.addSeparator();
	contextMenu.addAction( &actionDeleteFile );
	contextMenu.addAction( &actionRenameFile );
	contextMenu.addAction( &actionCopyReference );

	const bool		bExistCurrentPackage				= currentPackage;
	const bool		bValidItem							= modelIndex.isValid();
	const bool		bSelectedMoreOneItems				= numSelecteItems > 1;
	bool			bExistSupportedReimportAssetType	= false;

	if ( bExistCurrentPackage )
	{
		for ( uint32 index = 0, count = modelIndexList.length(); index < count && !bExistSupportedReimportAssetType; ++index )
		{
			FAssetInfo		assetInfo;
			currentPackage->GetAssetInfo( modelIndexList[ index ].row(), assetInfo );
			bExistSupportedReimportAssetType = assetInfo.type == AT_Texture2D || assetInfo.type == AT_AudioBank;
		}
	}

	menuCreate.setEnabled( bExistCurrentPackage );																								//
	actionImport.setEnabled( bExistCurrentPackage );																							// } If current package is not valid - disable all actions
	actionReimport.setEnabled( bExistCurrentPackage && bValidItem && bExistSupportedReimportAssetType );										//
	actionReimportWithNewFile.setEnabled( bExistCurrentPackage && bValidItem && !bSelectedMoreOneItems && bExistSupportedReimportAssetType );	//   Not all assets can be imported
	actionDeleteFile.setEnabled( bExistCurrentPackage && bValidItem );																			// } Disable actions if item is not valid
	actionRenameFile.setEnabled( bExistCurrentPackage && bValidItem && !bSelectedMoreOneItems );												//   Disable actions if selected more of one item
	actionCopyReference.setEnabled( bExistCurrentPackage && bValidItem && !bSelectedMoreOneItems );												//
	actionReload.setEnabled( bExistCurrentPackage && bValidItem );																				//

	// Connect to signal of a actions
	connect( &actionImport, SIGNAL( triggered() ), this, SLOT( on_listView_packageBrowser_Import() ) );
	connect( &actionReimport, SIGNAL( triggered() ), this, SLOT( on_listView_packageBrowser_Reimport() ) );
	connect( &actionReimportWithNewFile, SIGNAL( triggered() ), this, SLOT( on_listView_packageBrowser_ReimportWithNewFile() ) );
	connect( &actionCreateMaterial, SIGNAL( triggered() ), this, SLOT( on_listView_packageBrowser_CreateMaterial() ) );
	connect( &actionDeleteFile, SIGNAL( triggered() ), this, SLOT( on_listView_packageBrowser_DeleteAsset() ) );
	connect( &actionRenameFile, SIGNAL( triggered() ), this, SLOT( on_listView_packageBrowser_RenameAsset() ) );
	connect( &actionCopyReference, SIGNAL( triggered() ), this, SLOT( on_listView_packageBrowser_CopyReferenceToAsset() ) );
	connect( &actionReload, SIGNAL( triggered() ), this, SLOT( on_listView_packageBrowser_ReloadAsset() ) );
	contextMenu.exec( QCursor::pos() );
}

void WeContentBrowserWidget::on_listView_packageBrowser_Import()
{
	FPackageRef		package = ui->listView_packageBrowser->GetPackage();
	check( package );

	QStringList			selectedAssets = QFileDialog::getOpenFileNames( this, "Select Asset To Import", QString(), FHelperAssetImporter::MakeFilterOfSupportedExtensions() );
	if ( selectedAssets.isEmpty() )
	{
		return;
	}

	// Import selected assets
	bool							bReplaceAll		= false;	//
	bool							bSkipAll		= false;	// } Replace or skip all assets with exist names in package
	std::vector< QString >			errorAssets;				// Array with errors of assets
	for ( uint32 index = 0, count = selectedAssets.size(); index < count; ++index )
	{
		std::wstring							errorMessage;
		QString									path = selectedAssets[ index ];
		TAssetHandle<FAsset>					assetPtr;
		FHelperAssetImporter::EImportResult		importResult = FHelperAssetImporter::Import( path, package, assetPtr, errorMessage, bReplaceAll && !bSkipAll );

		// Show message if not setted flags bReplaceAll and bSkipAll
		if ( !bReplaceAll && !bSkipAll && importResult == FHelperAssetImporter::IR_AlreadyExist )
		{
			QMessageBox::StandardButton		resultButton = QMessageBox::question( this, "Question", QString::fromStdWString( FString::Format( TEXT( "Asset with name <b>'%s'</b> already exist in package. Replace it?" ), QFileInfo( path ).baseName().toStdWString().c_str() ) ), QMessageBox::Yes | QMessageBox::YesToAll | QMessageBox::No | QMessageBox::NoToAll );
			switch ( resultButton )
			{
			case QMessageBox::YesToAll:
				bReplaceAll = true;

			case QMessageBox::Yes:
				importResult = FHelperAssetImporter::Import( selectedAssets[ index ], package, assetPtr, errorMessage, true );
				break;

			case QMessageBox::NoToAll:
				bSkipAll = true;

			case QMessageBox::No:
				continue;
			}
		}

		// If failed import asset - add error message to array
		if ( importResult == FHelperAssetImporter::IR_Error )
		{
			errorAssets.push_back( path + " : " + QString::fromStdWString( errorMessage ) );
		}
	}

	// If we added to package new assets - need refresh package browser
	if ( package->IsDirty() )
	{
		ui->listView_packageBrowser->Refresh();
	}

	// If we have error assets - show message box
	if ( !errorAssets.empty() )
	{
		ShowMessageBoxWithList( this, "Error", "Failed import next assets", "Assets", errorAssets, true );
	}
}

void WeContentBrowserWidget::on_listView_packageBrowser_Reimport()
{
	// Getting selected items and current package
	FPackageRef				package = ui->listView_packageBrowser->GetPackage();
	QModelIndexList			modelIndexList = ui->listView_packageBrowser->selectionModel()->selectedRows();
	if ( !package )
	{
		return;
	}

	// Reimport selected assets
	std::vector< QString >			errorAssets;				// Array with errors of assets
	for ( uint32 index = 0, count = modelIndexList.length(); index < count; ++index )
	{
		uint32			idAsset = modelIndexList[ index ].row();
		FAssetInfo		assetInfo;
		package->GetAssetInfo( idAsset, assetInfo );

		std::wstring			errorMessage;
		TAssetHandle<FAsset>	assetHandle = package->Find( assetInfo.name );
		TSharedPtr<FAsset>		assetRef = assetHandle.ToSharedPtr();
		bool					bResult = FHelperAssetImporter::Reimport( assetHandle, errorMessage );

		// If failed reimport asset - add error message to array
		if ( !bResult )
		{
			errorAssets.push_back( QString::fromStdWString( assetRef->GetAssetName() ) + " : " + QString::fromStdWString( errorMessage ) );
		}
	}

	// If we have error assets - show message box
	if ( !errorAssets.empty() )
	{
		ShowMessageBoxWithList( this, "Error", "Failed reimport next assets", "Assets", errorAssets, true );
	}
}

void WeContentBrowserWidget::on_listView_packageBrowser_ReimportWithNewFile()
{
	// Getting selected items and current package
	FPackageRef				package = ui->listView_packageBrowser->GetPackage();
	QModelIndexList			modelIndexList = ui->listView_packageBrowser->selectionModel()->selectedRows();
	if ( !package )
	{
		return;
	}

	// Getting asset info
	check( modelIndexList.length() == 1 );		// Support rename only first asset
	FAssetInfo		assetInfo;
	package->GetAssetInfo( modelIndexList[ 0 ].row(), assetInfo );

	// Make flags for getting supported extensions
	uint32		filterFlags = 0;
	switch ( assetInfo.type )
	{
	case AT_Texture2D:
		filterFlags = FHelperAssetImporter::ET_Texture2D;
		break;

	case AT_AudioBank:
		filterFlags = FHelperAssetImporter::ET_AudioBank;
		break;

	default:		return;
	}

	// Select new source file
	QString			newSourceFile	= QFileDialog::getOpenFileName( this, "Select New Source File", QString(), FHelperAssetImporter::MakeFilterOfSupportedExtensions( filterFlags ) );
	if ( newSourceFile.isEmpty() )
	{
		return;
	}

	// Reimport asset
	std::wstring			errorMessage;
	TAssetHandle<FAsset>	assetHandle = package->Find( assetInfo.name );
	TSharedPtr<FAsset>		assetRef = assetHandle.ToSharedPtr();
	assetRef->SetAssetSourceFile( appQtAbsolutePathToEngine( newSourceFile ) );
	bool			bResult = FHelperAssetImporter::Reimport( assetHandle, errorMessage );
	
	// If reimport asset is failed - show error message box
	if ( !bResult )
	{
		QMessageBox::critical( this, "Error", QString::fromStdWString( FString::Format( TEXT( "Failed reimport asset <b>'%s'</b> from new source file <b>'%s'</b>.<br><br>Error: %s" ), assetRef->GetAssetName().c_str(), newSourceFile.toStdWString().c_str(), errorMessage.c_str() ) ) );
	}
}

void WeContentBrowserWidget::on_listView_packageBrowser_CreateMaterial()
{
	// Getting current package
	FPackageRef				package = ui->listView_packageBrowser->GetPackage();
	if ( !package )
	{
		return;
	}

	// Get asset name and check on exist other asset with this name
	bool			bIsOk = false;
	QString			assetName;
	while ( !bIsOk )
	{
		// Get asset name. If we not press 'ok' nothing apply and exit from method
		assetName = QInputDialog::getText( this, "Enter", "Asset Name", QLineEdit::Normal, "NewAsset", &bIsOk );
		if ( !bIsOk )
		{
			return;
		}

		// If asset with name already exist - try enter other name 
		if ( package->IsExist( assetName.toStdWString() ) )
		{
			QMessageBox::critical( this, "Error", QString::fromStdWString( FString::Format( TEXT( "Name <b>'%s'</b> already exist in package" ), assetName.toStdWString().c_str() ) ), QMessageBox::Ok );
			bIsOk = false;
		}
	}

	// Create material with asset name and add to package
	TSharedPtr<FMaterial>		materialRef = MakeSharedPtr<FMaterial>();
	materialRef->SetAssetName( assetName.toStdWString() );
	package->Add( TAssetHandle<FMaterial>( materialRef, MakeSharedPtr<FAssetReference>( AT_Material, materialRef->GetGUID() ) ) );
	ui->listView_packageBrowser->Refresh();
}

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
	std::vector< std::wstring >		assetInfosToDelete;
	std::vector< QString >			usedAssets;
	for ( uint32 index = 0, count = modelIndexList.length(); index < count; ++index )
	{
		uint32			idAsset = modelIndexList[ index ].row();
		FAssetInfo		assetInfo;
		
		package->GetAssetInfo( idAsset, assetInfo );
		assetInfosToDelete.push_back( assetInfo.name );
	}

	for ( uint32 index = 0, count = assetInfosToDelete.size(); index < count; ++index )
	{
		if ( !package->Remove( assetInfosToDelete[ index ], false, true ) )
		{
			usedAssets.push_back( QString::fromStdWString( assetInfosToDelete[ index ] ) );
		}
	}

	// If package is dirty - refresh package browser
	if ( package->IsDirty() )
	{
		ui->listView_packageBrowser->Refresh();
	}

	// If we have used assets - print message
	if ( !usedAssets.empty() )
	{
		ShowMessageBoxWithList( this, "Warning", "The following assets in using and cannot be delete. Close all assets from this package will allow them to be moved or deleted", "Assets", usedAssets );
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
			QMessageBox::critical( this, "Error", QString::fromStdWString( FString::Format( TEXT( "Name <b>'%s'</b> already exist in package" ), newAssetName.toStdWString().c_str() ) ), QMessageBox::Ok );
			bIsOk = false;
		}
	}

	// Find asset in package
	TSharedPtr<FAsset>		assetRef = package->Find( assetInfo.name ).ToSharedPtr();
	if ( !assetRef )
	{
		return;
	}

	// Add change asset name
	assetRef->SetAssetName( newAssetName.toStdWString() );
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
	
	std::vector< QString >		dirtyPackages;
	for ( int index = 0, count = modelIndexList.length(); index < count; ++index )
	{
		QFileInfo		fileInfo	= fileSystemModel->fileInfo( modelIndexList[ index ] );
		std::wstring	pathPackage = appQtAbsolutePathToEngine( fileInfo.absoluteFilePath() );
		
		// If package already not loaded - skip him
		if ( !GPackageManager->IsPackageLoaded( pathPackage ) )
		{
			continue;
		}

		// If package is dirty - we not unload him
		FPackageRef		package = GPackageManager->LoadPackage( pathPackage );
		if ( package->IsDirty() )
		{
			dirtyPackages.push_back( fileInfo.baseName() );
			continue;
		}

		bool	bSeccussed = GPackageManager->UnloadPackage( pathPackage );

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

	// If we have dirty package - print message
	if ( !dirtyPackages.empty() )
	{
		ShowMessageBoxWithList( this, "Warning", "The following packages have been modified and cannot be unloaded. Saving these packages will allow them to be unloaded", "Packages", dirtyPackages );
	}
}

void WeContentBrowserWidget::on_treeView_contentBrowser_contextMenu_delete()
{
	QModelIndexList					modelIndexList		= ui->treeView_contentBrowser->selectionModel()->selectedRows();
	QFileSystemModel*				fileSystemModel		= ui->treeView_contentBrowser->GetFileSystemModel();
	FPackageRef						currentPackage		= ui->listView_packageBrowser->GetPackage();
	QString							filesForDelete;
	bool							bDirtyTOC			= false;

	// If we not press 'ok' in message box - exit from method
	if ( ShowDeleteFileMessageBox( this, "Warning", "Is need delete selected files?", modelIndexList, fileSystemModel ) != QMessageBox::Ok )
	{
		return;
	}

	// Delete all selected files
	std::vector< QString >		usedPackages;
	for ( uint32 index = 0, count = modelIndexList.length(); index < count; ++index )
	{
		QFileInfo		fileInfo = fileSystemModel->fileInfo( modelIndexList[ index ] );
		if ( fileInfo.isDir() )
		{
			DeleteDir( fileInfo.absoluteFilePath(), bDirtyTOC, usedPackages );
		}
		else
		{
			// If this package, we remove entry from TOC file
			if ( fileInfo.suffix() == FILE_PACKAGE_EXTENSION )
			{
				std::wstring	enginePath = appQtAbsolutePathToEngine( fileInfo.absoluteFilePath() );
				
				// We try unload package for remove unused assets.
				// If package still used, we skip him
				if ( !GPackageManager->UnloadPackage( enginePath ) )
				{
					usedPackages.push_back( fileInfo.baseName() );
					continue;
				}
				
				// Else we remove entry from TOC file
				GTableOfContents.RemoveEntry( enginePath );
				bDirtyTOC = true;

				// If this is  package and him opened in package browser, we close it
				if ( currentPackage && enginePath == currentPackage->GetFileName() )
				{
					ui->listView_packageBrowser->SetPackage( nullptr );
				}
			}

			QFile( fileInfo.absoluteFilePath() ).remove();
		}
	}

	// If TOC file is dirty, we serialize him to cache
	if ( bDirtyTOC )
	{
		GAssetDataBase.SerializeTOC( true );
	}

	// If we have used package - print message
	if ( !usedPackages.empty() )
	{
		ShowMessageBoxWithList( this, "Warning", "The following packages in using and cannot be delete. Close all assets from this package will allow them to be deleted", "Packages", usedPackages );
	}
}

void WeContentBrowserWidget::on_treeView_contentBrowser_contextMenu_rename()
{
	bool				isRename			= false;
	QFileSystemModel*	fileSystemModel		= ui->treeView_contentBrowser->GetFileSystemModel();
	QFileInfo			fileInfo			= fileSystemModel->fileInfo( ui->treeView_contentBrowser->currentIndex() );
	QString				newFileName;
	bool				bDirtyTOC			= false;

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
		QString			newAbsolutePath = fileInfo.absolutePath() + "/" + newFileName + "." + fileInfo.suffix();

		// If this is package need call SetName in him
		if ( fileInfo.suffix() == FILE_PACKAGE_EXTENSION )
		{
			std::wstring	pathPackage				= appQtAbsolutePathToEngine( fileInfo.absoluteFilePath() );
			
			// We try unload package for remove unused assets.
			// If package still used, we skip him
			if ( !GPackageManager->UnloadPackage( pathPackage ) )
			{
				QMessageBox::warning( this, "Warning", QString::fromStdWString( FString::Format( TEXT( "The package <b>'%s'</b> in using and cannot be delete. Close all assets from this package will allow them to be deleted" ), fileInfo.baseName().toStdWString().c_str() ) ), QMessageBox::Ok );
				return;
			}

			bool			bIsNeedUnloadPackage	= !GPackageManager->IsPackageLoaded( pathPackage );
			FPackageRef		package					= GPackageManager->LoadPackage( pathPackage );

			// If package failed loaded - we not change him name
			if ( !package )
			{
				QMessageBox::critical( this, "Error", QString::fromStdWString( FString::Format( TEXT( "File <b>'%s'</b> not renamed because failed loading package for change him name" ), pathPackage.c_str() ) ), QMessageBox::Ok );
				return;
			}

			// And if package is dirty - we not change him name
			if ( package->IsDirty() )
			{
				QMessageBox::critical( this, "Error", QString::fromStdWString( FString::Format( TEXT( "File <b>'%s'</b> not renamed because this package is modified. Saving this package will allow to be rename him" ), pathPackage.c_str() ) ) );
				return;
			}

			// Else we change name and resave package
			package->SetName( newFileName.toStdWString() );
			package->Save( package->GetFileName() );

			// Update TOC file
			GTableOfContents.RemoveEntry( package->GetGUID() );
			GTableOfContents.AddEntry( package->GetGUID(), package->GetName(), appQtAbsolutePathToEngine( newAbsolutePath ) );
			bDirtyTOC = true;

			// If need unload package - do it
			if ( bIsNeedUnloadPackage )
			{
				GPackageManager->UnloadPackage( pathPackage );
			}
		}

		file.rename( fileInfo.absoluteFilePath(), newAbsolutePath );
	}

	// If TOC file is dirty, we serialize him to cache
	if ( bDirtyTOC )
	{
		GAssetDataBase.SerializeTOC( true );
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

	// Update TOC file and serialize him
	GTableOfContents.AddEntry( package->GetGUID(), package->GetName(), package->GetFileName() );
	GAssetDataBase.SerializeTOC( true );
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

void WeContentBrowserWidget::on_listView_packageBrowser_doubleClicked( QModelIndex InModelIndex )
{
	// If selected item is not valid - exit from method
	if ( !InModelIndex.isValid() )
	{
		return;
	}

	// Get info about asset
	FPackageRef			package = ui->listView_packageBrowser->GetPackage();
	FAssetInfo			assetInfo;
	package->GetAssetInfo( InModelIndex.row(), assetInfo );
	TSharedPtr<FAsset>	assetRef = package->Find( assetInfo.name ).ToSharedPtr();

	// Open editor for each asset type
	switch ( assetInfo.type )
	{
	case AT_Texture2D:
	{
		assetRef->ReloadDependentAssets();

		WeTextureEditorWindow*		textureEditorWindow = new WeTextureEditorWindow( assetRef, this );
		GEditorEngine->GetMainWindow()->CreateFloatingDockWidget( QString::fromStdWString( FString::Format( TEXT( "%s - %s" ), textureEditorWindow->windowTitle().toStdWString().c_str(), assetRef->GetAssetName().c_str() ) ), textureEditorWindow, true );
		connect( textureEditorWindow, SIGNAL( OnChangedAsset( const TSharedPtr<FAsset>& ) ), this, SLOT( OnPackageBrowserChangedAsset( const TSharedPtr<FAsset>& ) ) );
		break;
	}

	case AT_Material:
	{
		assetRef->ReloadDependentAssets();

		WeMaterialEditorWindow*		materialEditorWindow = new WeMaterialEditorWindow( assetRef, this );
		GEditorEngine->GetMainWindow()->CreateFloatingDockWidget( QString::fromStdWString( FString::Format( TEXT( "%s - %s" ), materialEditorWindow->windowTitle().toStdWString().c_str(), assetRef->GetAssetName().c_str() ) ), materialEditorWindow, true );
		connect( materialEditorWindow, SIGNAL( OnChangedAsset( const TSharedPtr<FAsset>& ) ), this, SLOT( OnPackageBrowserChangedAsset( const TSharedPtr<FAsset>& ) ) );
		break;
	}
	}
}

void WeContentBrowserWidget::OnPackageBrowserChangedAsset( const TSharedPtr<FAsset>& InAsset )
{
	ui->treeView_contentBrowser->repaint();
	ui->listView_packageBrowser->repaint();
}

void WeContentBrowserWidget::on_listView_packageBrowser_CopyReferenceToAsset()
{
	// Getting selected items and current package
	FPackageRef				package = ui->listView_packageBrowser->GetPackage();
	QModelIndexList			modelIndexList = ui->listView_packageBrowser->selectionModel()->selectedRows();
	if ( !package )
	{
		return;
	}

	check( modelIndexList.length() == 1 );		// Support copy reference only first asset
	FAssetInfo		assetInfo;
	package->GetAssetInfo( modelIndexList[ 0 ].row(), assetInfo );

	// Make reference to asset
	std::wstring		assetReference;
	bool	bResult = MakeReferenceToAsset( package->GetName(), assetInfo.name, assetInfo.type, assetReference );
	if ( !bResult )
	{
		return;
	}

	QApplication::clipboard()->setText( QString::fromStdWString( assetReference ) );
}

void WeContentBrowserWidget::OnListViewPackageBrowserSelectedAsset( const std::wstring& InAssetReference )
{
	selectedAssetReference = InAssetReference;
}

void WeContentBrowserWidget::on_listView_packageBrowser_ReloadAsset()
{
	// Getting selected items and current package
	FPackageRef				package = ui->listView_packageBrowser->GetPackage();
	QModelIndexList			modelIndexList = ui->listView_packageBrowser->selectionModel()->selectedRows();
	if ( !package )
	{
		return;
	}

	// Reload selected assets
	for ( uint32 index = 0, count = modelIndexList.length(); index < count; ++index )
	{
		uint32			idAsset = modelIndexList[ index ].row();
		FAssetInfo		assetInfo;
		FGuid			guidAsset;
		package->GetAssetInfo( idAsset, assetInfo, &guidAsset );
		package->ReloadAsset( guidAsset );
	}
}

void WeContentBrowserWidget::on_treeView_contentBrowser_contextMenu_ReloadPackage()
{
	// Getting selected items, file system model and current package in package browser
	QModelIndexList			modelIndexList = ui->treeView_contentBrowser->selectionModel()->selectedRows();
	QFileSystemModel*		fileSystemModel = ui->treeView_contentBrowser->GetFileSystemModel();
	FPackageRef				currentPackage = ui->listView_packageBrowser->GetPackage();

	// Reload packages
	bool		bIsNeedRepaint = false;
	for ( int index = 0, count = modelIndexList.length(); index < count; ++index )
	{
		QFileInfo		fileInfo = fileSystemModel->fileInfo( modelIndexList[ index ] );
		bIsNeedRepaint |= GPackageManager->ReloadPackage( appQtAbsolutePathToEngine( fileInfo.absoluteFilePath() ) );
	}

	// Repaint content browser is need
	if ( bIsNeedRepaint )
	{
		ui->treeView_contentBrowser->repaint();
		ui->listView_packageBrowser->Refresh();
	}
}