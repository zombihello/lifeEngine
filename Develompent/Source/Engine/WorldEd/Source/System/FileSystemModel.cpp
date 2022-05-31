#include <vector>
#include <qmimedata.h>
#include <qurl.h>
#include <qdebug.h>
#include <qapplication.h>

#include "Logger/LoggerMacros.h"
#include "Misc/Misc.h"
#include "Misc/CoreGlobals.h"
#include "Misc/WorldEdGlobals.h"
#include "Misc/TableOfContents.h"
#include "System/Package.h"
#include "System/BaseFileSystem.h"
#include "System/AssetDataBase.h"
#include "Containers/String.h"
#include "Containers/StringConv.h"
#include "System/FileSystemModel.h"
#include "WorldEd.h"

WeFileSystemModel::WeFileSystemModel( QObject* InParent ) 
	: QFileSystemModel( InParent )
	, applayMode( QMessageBox::No )
{	
}

Qt::ItemFlags WeFileSystemModel::flags( const QModelIndex& InIndex ) const
{
	Qt::ItemFlags defaultFlags = QFileSystemModel::flags( InIndex );

	if ( !InIndex.isValid() )
	{
		return defaultFlags;
	}

	const QFileInfo& fileInfo = this->fileInfo( InIndex );
	if ( fileInfo.isDir() )
	{
		return Qt::ItemIsDropEnabled | defaultFlags;
	}

	else if ( fileInfo.isFile() )
	{
		return Qt::ItemIsDragEnabled | defaultFlags;
	}

	return defaultFlags;
}

bool WeFileSystemModel::dropMimeData( const QMimeData* InData, Qt::DropAction InAction, int InRow, int InColumn, const QModelIndex& InParent )
{
	QList<QUrl>			urls = InData->urls();
	QFileInfo			dropFileInfo;
	int					countFiles = 0;

	// Counting the total number of files to copy/move
	for ( auto it = urls.begin(), itEnd = urls.end(); it != itEnd; ++it )
	{
		dropFileInfo.setFile( it->toLocalFile() );

		if ( dropFileInfo.isDir() )
		{
			CountFilesInDir( dropFileInfo.absoluteFilePath(), countFiles, InAction == Qt::MoveAction );
		}
		else
		{
			++countFiles;
		}
	}

	switch ( InAction )
	{
		case Qt::MoveAction:
			dropMimeData_moveFiles( InData, InAction, InParent, countFiles );			
			break;

		case Qt::CopyAction:
			dropMimeData_copyFiles( InData, InAction, InParent, countFiles );
			break;

		default:
			return false;
	}

	return true;
}

void WeFileSystemModel::dropMimeData_moveFiles( const QMimeData* InData, Qt::DropAction InAction, const QModelIndex& InParent, int InCountFiles )
{
	QList<QUrl>			urls = InData->urls();
	QDir				targetDir;
	QFileInfo			dropFileInfo;
	QString				oldFile;
	QString				newFile;
	int					compliteFiles = 0;
	bool				bDirtyTOC = false;

	QProgressDialog				progressDialog( "Moving files", "Cancel", 0, InCountFiles, ( QWidget* )this->parent() );
	progressDialog.setWindowTitle( "Progress moving files" );
	progressDialog.resize( 300, 50 );
	progressDialog.show();
	applayMode = QMessageBox::No;

	std::vector< QString >		usedPackages;
	for ( auto it = urls.begin(), itEnd = urls.end(); it != itEnd; ++it, ++compliteFiles )
	{
		dropFileInfo.setFile( it->toLocalFile() );
	
		progressDialog.setValue( compliteFiles );
		QCoreApplication::processEvents();
		if ( progressDialog.wasCanceled() )
		{
			break;
		}

		oldFile = dropFileInfo.absoluteFilePath();
		newFile = fileInfo( InParent ).absoluteFilePath() + "/" + dropFileInfo.baseName();
			
		if ( !dropFileInfo.suffix().isEmpty() )
			newFile += "." + dropFileInfo.suffix();

		if ( oldFile == newFile )
		{
			continue;
		}
		else if ( dropFileInfo.isDir() )
		{
			targetDir.setPath( fileInfo( InParent ).absoluteFilePath() );
			if ( targetDir.exists( dropFileInfo.baseName() ) )
			{
				continue;
			}

			MoveDir( oldFile, newFile, &progressDialog, compliteFiles, bDirtyTOC, usedPackages );
		}
		else
		{
			bool		bPackage = dropFileInfo.suffix() == FILE_PACKAGE_EXTENSION;
			if ( QFile::exists( newFile ) )
			{
				if ( applayMode == QMessageBox::NoToAll ||
					 applayMode != QMessageBox::YesToAll && QMessageBox::warning( ( QWidget* ) this->parent(),
					 "Warning",
					 TCHAR_TO_ANSI( ÑString::Format( TEXT( "Move file exists %s %s" ), dropFileInfo.fileName().toStdWString().c_str(), this->fileInfo( InParent ).fileName().toStdWString().c_str() ).c_str() ),
					 QMessageBox::Ok | QMessageBox::Cancel ) != QMessageBox::Ok )
				{
					continue;
				}

				// If this file is package, we remove him from TOC
				if ( bPackage )
				{
					std::wstring		enginePath = appQtAbsolutePathToEngine( newFile );
					
					// We try unload package for remove unused assets.
					// If package still used, we skip him
					if ( !GPackageManager->UnloadPackage( enginePath ) )
					{
						usedPackages.push_back( fileInfo( InParent ).baseName() + "/" + dropFileInfo.baseName() );
						continue;
					}

					// Else we remove entry from TOC file
					GTableOfContents.RemoveEntry( enginePath );
					bDirtyTOC		= true;
				}

				QFile::remove( newFile );
			}

			// If this is package, we update TOC file
			if ( bPackage )
			{
				std::wstring		enginePath = appQtAbsolutePathToEngine( oldFile );
				
				// We try unload package for remove unused assets.
				// If package still used, we skip him
				if ( !GPackageManager->UnloadPackage( enginePath ) )
				{
					usedPackages.push_back( dropFileInfo.baseName() );
					continue;
				}

				// Else we remove entry from TOC file
				GTableOfContents.RemoveEntry( enginePath );
				bDirtyTOC		= true;
			}	

			QFile::rename( oldFile, newFile );

			if ( bPackage )
			{
				GTableOfContents.AddEntry( appQtAbsolutePathToEngine( newFile ) );
				bDirtyTOC		= true;
			}

			
		}
	}

	// If TOC file is dirty, we serialize him
	if ( bDirtyTOC )
	{
		GAssetDataBase.SerializeTOC( true );
	}

	// If we have used package - print message
	if ( !usedPackages.empty() )
	{
		ShowMessageBoxWithList( ( QWidget* )parent(), "Warning", "The following packages in using and cannot be delete. Close all assets from this package will allow them to be moved or deleted", "Packages", usedPackages );
	}
}

void WeFileSystemModel::dropMimeData_copyFiles( const QMimeData* InData, Qt::DropAction InAction, const QModelIndex& InParent, int InCountFiles )
{
	QList<QUrl>			urls = InData->urls();
	QDir				targetDir;
	QFileInfo			dropFileInfo;
	QString				oldFile;
	QString				newFile;
	int					compliteFiles = 0;
	bool				bDirtyTOC = false;

	QProgressDialog				progressDialog( "Progress copy files", "Cancel", 0, InCountFiles, ( QWidget* )this->parent() );
	progressDialog.setWindowTitle( "Progress copy files" );
	progressDialog.resize( 300, 50 );
	progressDialog.show();
	applayMode = QMessageBox::No;

	std::vector< QString >		usedPackages;
	for ( auto it = urls.begin(), itEnd = urls.end(); it != itEnd; ++it, ++compliteFiles )
	{
		dropFileInfo.setFile( it->toLocalFile() );
		
		progressDialog.setValue( compliteFiles );
		QCoreApplication::processEvents();
		if ( progressDialog.wasCanceled() )
		{
			break;
		}

		oldFile = dropFileInfo.absoluteFilePath();
		newFile = fileInfo( InParent ).absoluteFilePath() + "/" + dropFileInfo.baseName();

		if ( !dropFileInfo.suffix().isEmpty() )
			newFile += "." + dropFileInfo.suffix();

		if ( oldFile == newFile )
		{
			continue;
		}
		else if ( dropFileInfo.isDir() )
		{
			targetDir.setPath( fileInfo( InParent ).absoluteFilePath() );
			if ( targetDir.exists( dropFileInfo.baseName() ) )
			{
				continue;
			}

			CopyDir( oldFile, newFile, &progressDialog, compliteFiles, bDirtyTOC, usedPackages );
		}
		else
		{
			bool		bPackage = dropFileInfo.suffix() == FILE_PACKAGE_EXTENSION;

			if ( QFile::exists( newFile ) )
			{
				if ( applayMode == QMessageBox::NoToAll ||
					 applayMode != QMessageBox::YesToAll && QMessageBox::warning( ( QWidget* ) this->parent(),
					 "Warning",
					 TCHAR_TO_ANSI( ÑString::Format( TEXT( "Copy file exists %s %s" ), dropFileInfo.fileName().toStdWString().c_str(), this->fileInfo( InParent ).fileName().toStdWString().c_str() ).c_str() ),
					 QMessageBox::Ok | QMessageBox::Cancel ) != QMessageBox::Ok )
				{
					continue;
				}

				// If this file is package, we remove him from TOC
				if ( bPackage )
				{
					std::wstring		enginePath = appQtAbsolutePathToEngine( newFile );
					
					// We try unload package for remove unused assets.
					// If package still used, we skip him
					if ( !GPackageManager->UnloadPackage( enginePath ) )
					{
						usedPackages.push_back( fileInfo( InParent ).baseName() + "/" + dropFileInfo.baseName() );
						continue;
					}

					// Else remove entry from TOC file
					GTableOfContents.RemoveEntry( enginePath );
					bDirtyTOC = true;
				}

				QFile::remove( newFile );
			}

			QFile::copy( oldFile, newFile );

			// If this is package, we update TOC file
			if ( bPackage )
			{
				GTableOfContents.AddEntry( appQtAbsolutePathToEngine( newFile ) );
				bDirtyTOC = true;
			}
		}
	}

	// If TOC file is dirty, we serialize him
	if ( bDirtyTOC )
	{
		GAssetDataBase.SerializeTOC( true );
	}

	// If we have used package - print message
	if ( !usedPackages.empty() )
	{
		ShowMessageBoxWithList( ( QWidget* )parent(), "Warning", "The following packages in using and cannot be delete. Close all assets from this package will allow them to be deleted", "Packages", usedPackages );
	}
}

void WeFileSystemModel::MoveDir( const QString& InOldDir, const QString& InNewDir, QProgressDialog* InProgressDialog, int& OutValueProgress, bool& OutIsDirtyTOC, std::vector< QString >& OutUsedPackage )
{
	QDir				oldDir( InOldDir );
	
	if ( !oldDir.mkpath( InNewDir ) ) return;

	QFileInfoList						entryInfoList = oldDir.entryInfoList( QDir::AllEntries | QDir::NoDotAndDotDot | QDir::NoSymLinks | QDir::Filter::Hidden );
	QString								newFile;

	for ( auto it = entryInfoList.begin(), itEnd = entryInfoList.end(); it != itEnd; ++it )
	{
		if ( it->isDir() )
		{
			MoveDir( it->absoluteFilePath(), InNewDir + "/" + it->fileName(), InProgressDialog, OutValueProgress, OutIsDirtyTOC, OutUsedPackage );
		}
		else
		{
			newFile = InNewDir + "/" + it->fileName();

			InProgressDialog->setValue( OutValueProgress );
			++OutValueProgress;

			QCoreApplication::processEvents();
			if ( InProgressDialog->wasCanceled() )
			{
				break;
			}

			bool		bPackage = it->suffix() == FILE_PACKAGE_EXTENSION;

			if ( QFile::exists( newFile ) )
			{
				switch ( applayMode )
				{
					case QMessageBox::NoToAll:
						continue;

					case QMessageBox::YesToAll:
						break;

					case QMessageBox::No:
					case QMessageBox::Yes:
					{
						applayMode = QMessageBox::warning( ( QWidget* ) this->parent(),
														   "Warning",
														   TCHAR_TO_ANSI( ÑString::Format( TEXT( "Move file exists %s %s" ), it->fileName().toStdWString().c_str(), InNewDir.toStdWString().c_str() ).c_str() ),
														   QMessageBox::YesAll | QMessageBox::Yes | QMessageBox::NoAll | QMessageBox::No );

						if ( applayMode == QMessageBox::No || applayMode == QMessageBox::NoToAll )
						{
							continue;
						}
					}
				}

				// If this file is package, we remove him from TOC
				if ( bPackage )
				{
					std::wstring		enginePath = appQtAbsolutePathToEngine( newFile );
					
					// We try unload package for remove unused assets.
					// If package still used, we skip him
					if ( !GPackageManager->UnloadPackage( enginePath ) )
					{
						OutUsedPackage.push_back( InNewDir + "/" + it->baseName() );
						continue;
					}

					// Else remove entry from TOC file
					GTableOfContents.RemoveEntry( enginePath );
					OutIsDirtyTOC	= true;
				}

				QFile::remove( newFile );
			}

			// If this is package, we update TOC file
			if ( bPackage )
			{
				std::wstring		enginePath = appQtAbsolutePathToEngine( it->absoluteFilePath() );
				
				// We try unload package for remove unused assets.
				// If package still used, we skip him
				if ( !GPackageManager->UnloadPackage( enginePath ) )
				{
					OutUsedPackage.push_back( it->baseName() );
					continue;
				}

				// Else remove entry from TOC file
				GTableOfContents.RemoveEntry( enginePath );
				OutIsDirtyTOC = true;
			}

			QFile::rename( it->absoluteFilePath(), newFile );

			if ( bPackage )
			{
				GTableOfContents.AddEntry( appQtAbsolutePathToEngine( newFile ) );
				OutIsDirtyTOC = true;
			}
		}
	}

	if ( oldDir.isEmpty() )
	{
		oldDir.rmdir( InOldDir );
	}
}

void WeFileSystemModel::CopyDir( const QString& InOldDir, const QString& InNewDir, QProgressDialog* InProgressDialog, int& OutValueProgress, bool& OutIsDirtyTOC, std::vector< QString >& OutUsedPackage )
{
	QDir				oldDir( InOldDir );

	if ( !oldDir.mkpath( InNewDir ) ) return;

	QFileInfoList						entryInfoList = oldDir.entryInfoList( QDir::AllEntries | QDir::NoDotAndDotDot | QDir::NoSymLinks );
	QString								newFile;

	for ( auto it = entryInfoList.begin(), itEnd = entryInfoList.end(); it != itEnd; ++it )
	{
		if ( it->isDir() )
		{
			CopyDir( it->absoluteFilePath(), InNewDir + "/" + it->fileName(), InProgressDialog, OutValueProgress, OutIsDirtyTOC, OutUsedPackage );
		}
		else
		{
			newFile = InNewDir + "/" + it->fileName();

			InProgressDialog->setValue( OutValueProgress );
			++OutValueProgress;

			QCoreApplication::processEvents();
			if ( InProgressDialog->wasCanceled() )
			{
				break;
			}

			bool		bPackage = it->suffix() == FILE_PACKAGE_EXTENSION;

			if ( QFile::exists( newFile ) )
			{
				switch ( applayMode )
				{
					case QMessageBox::NoToAll:
						continue;

					case QMessageBox::YesToAll:
						break;

					case QMessageBox::No:
					case QMessageBox::Yes:
					{
						applayMode = QMessageBox::warning( ( QWidget* ) this->parent(),
														   "Warning",
														   TCHAR_TO_ANSI( ÑString::Format( TEXT( "Copy file exists %s %s" ), it->fileName().toStdWString().c_str(), InNewDir.toStdWString().c_str() ).c_str() ),
														   QMessageBox::YesAll | QMessageBox::Yes | QMessageBox::NoAll | QMessageBox::No );

						if ( applayMode == QMessageBox::No || applayMode == QMessageBox::NoToAll )
						{
							continue;
						}
					}
				}

				// If this file is package, we remove him from TOC
				if ( bPackage )
				{
					std::wstring		enginePath = appQtAbsolutePathToEngine( newFile );
					
					// We try unload package for remove unused assets.
					// If package still used, we skip him
					if ( !GPackageManager->UnloadPackage( enginePath ) )
					{
						OutUsedPackage.push_back( InNewDir + "/" + it->baseName() );
						continue;
					}

					// Else remove entry from TOC file
					GTableOfContents.RemoveEntry( enginePath );
					OutIsDirtyTOC = true;
				}

				QFile::remove( newFile );
			}

			QFile::copy( it->absoluteFilePath(), newFile );

			// If this is package, we update TOC file
			if ( bPackage )
			{
				GTableOfContents.AddEntry( appQtAbsolutePathToEngine( newFile ) );
				OutIsDirtyTOC = true;
			}
		}
	}

	if ( oldDir.isEmpty() )
	{
		oldDir.rmdir( InOldDir );
	}
}

int WeFileSystemModel::CountFilesInDir( const QString& InDir, int& OutCountFiles, bool InIsLinkHidden /* = false */ )
{
	QDir						dir( InDir );
	QFlags<QDir::Filter>		filters;

	filters.setFlag( QDir::AllEntries );
	filters.setFlag( QDir::NoDotAndDotDot );
	filters.setFlag( QDir::NoSymLinks );

	if ( InIsLinkHidden )
	{
		filters.setFlag( QDir::Filter::Hidden );
	}

	QFileInfoList				entryInfoList = dir.entryInfoList( filters );
	for ( auto it = entryInfoList.begin(), itEnd = entryInfoList.end(); it != itEnd; ++it )
	{
		if ( it->isDir() )
		{
			OutCountFiles = CountFilesInDir( InDir + "/" + it->baseName(), OutCountFiles );
		}
		else
		{
			++OutCountFiles;
		}
	}

	return OutCountFiles;
}

QVariant WeFileSystemModel::data( const QModelIndex& InIndex, int InRole /* = Qt::DisplayRole */ ) const
{
	QFileInfo		fileInfo = WeFileSystemModel::fileInfo( InIndex );
	std::wstring	path	 = appQtAbsolutePathToEngine( fileInfo.absoluteFilePath() );

	if ( InRole == Qt::DecorationRole )
	{
		if ( fileInfo.isFile() )
		{
			// Icon for packages
			if ( fileInfo.suffix() == FILE_PACKAGE_EXTENSION )
			{
				if ( GPackageManager->IsPackageLoaded( path ) )
				{
					return QPixmap( TCHAR_TO_ANSI( ( appBaseDir() + TEXT( "Engine/Editor/Icons/CB_PackageOpen.png" ) ).c_str() ) );
				}
				else
				{
					return QPixmap( TCHAR_TO_ANSI( ( appBaseDir() + TEXT( "Engine/Editor/Icons/CB_Package.png" ) ).c_str() ) );
				}
			}

			// Icon for maps
			else if ( fileInfo.suffix() == FILE_MAP_EXTENSION )
			{
				return QPixmap( TCHAR_TO_ANSI( ( appBaseDir() + TEXT( "Engine/Editor/Icons/CB_Map.png" ) ).c_str() ) );
			}
		}
		else if ( fileInfo.isDir() )
		{
			return QPixmap( TCHAR_TO_ANSI( ( appBaseDir() + TEXT( "Engine/Editor/Icons/CB_FolderClosed.png" ) ).c_str() ) );
		}
	}
	else if ( InRole == Qt::DisplayRole || InRole == Qt::EditRole )
	{
		if ( fileInfo.isFile() && fileInfo.suffix() == FILE_PACKAGE_EXTENSION && GPackageManager->IsPackageLoaded( path ) )		// If package loaded we check is changed in memory. In successed case mark this package by star
		{
			PackageRef_t		package = GPackageManager->LoadPackage( path );
			QString			result = fileInfo.baseName() + ( package->IsDirty() ? "[MODIFIED]" : "" );
			return result;
		}
		else
		{
			return fileInfo.baseName();		// Show all files without extensions
		}
	}

	return QFileSystemModel::data( InIndex, InRole );
}