#include <vector>
#include <qmimedata.h>
#include <qurl.h>
#include <qdebug.h>
#include <qapplication.h>

#include "Misc/Misc.h"
#include "Misc/CoreGlobals.h"
#include "System/Package.h"
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

	QProgressDialog				progressDialog( "Moving files", "Cancel", 0, InCountFiles, ( QWidget* )this->parent() );
	progressDialog.setWindowTitle( "Progress moving files" );
	progressDialog.resize( 300, 50 );
	progressDialog.show();
	applayMode = QMessageBox::No;

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

			MoveDir( oldFile, newFile, &progressDialog, compliteFiles );
		}
		else
		{
			if ( QFile::exists( newFile ) )
			{
				if ( applayMode == QMessageBox::NoToAll ||
					 applayMode != QMessageBox::YesToAll && QMessageBox::warning( ( QWidget* ) this->parent(),
					 "Warning",
					 TCHAR_TO_ANSI( FString::Format( TEXT( "Move file exists %s %s" ), dropFileInfo.fileName().toStdWString().c_str(), this->fileInfo( InParent ).fileName().toStdWString().c_str() ).c_str() ),
					 QMessageBox::Ok | QMessageBox::Cancel ) != QMessageBox::Ok )
				{
					continue;
				}

				QFile::remove( newFile );
			}

			QFile::rename( oldFile, newFile );
		}
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

	QProgressDialog				progressDialog( "Progress copy files", "Cancel", 0, InCountFiles, ( QWidget* )this->parent() );
	progressDialog.setWindowTitle( "Progress copy files" );
	progressDialog.resize( 300, 50 );
	progressDialog.show();
	applayMode = QMessageBox::No;

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

			CopyDir( oldFile, newFile, &progressDialog, compliteFiles );
		}
		else
		{
			if ( QFile::exists( newFile ) )
			{
				if ( applayMode == QMessageBox::NoToAll ||
					 applayMode != QMessageBox::YesToAll && QMessageBox::warning( ( QWidget* ) this->parent(),
					 "Warning",
					 TCHAR_TO_ANSI( FString::Format( TEXT( "Copy file exists %s %s" ), dropFileInfo.fileName().toStdWString().c_str(), this->fileInfo( InParent ).fileName().toStdWString().c_str() ).c_str() ),
					 QMessageBox::Ok | QMessageBox::Cancel ) != QMessageBox::Ok )
				{
					continue;
				}

				QFile::remove( newFile );
			}

			QFile::copy( oldFile, newFile );
		}
	}
}

void WeFileSystemModel::MoveDir( const QString& InOldDir, const QString& InNewDir, QProgressDialog* InProgressDialog, int& OutValueProgress )
{
	QDir				oldDir( InOldDir );
	
	if ( !oldDir.mkpath( InNewDir ) ) return;

	QFileInfoList						entryInfoList = oldDir.entryInfoList( QDir::AllEntries | QDir::NoDotAndDotDot | QDir::NoSymLinks | QDir::Filter::Hidden );
	QString								newFile;

	for ( auto it = entryInfoList.begin(), itEnd = entryInfoList.end(); it != itEnd; ++it )
	{
		if ( it->isDir() )
		{
			MoveDir( it->absoluteFilePath(), InNewDir + "/" + it->fileName(), InProgressDialog, OutValueProgress );
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
														   TCHAR_TO_ANSI( FString::Format( TEXT( "Move file exists %s %s" ), it->fileName().toStdWString().c_str(), InNewDir.toStdWString().c_str() ).c_str() ),
														   QMessageBox::YesAll | QMessageBox::Yes | QMessageBox::NoAll | QMessageBox::No );

						if ( applayMode == QMessageBox::No || applayMode == QMessageBox::NoToAll )
						{
							continue;
						}
					}
				}

				QFile::remove( newFile );
			}

			QFile::rename( it->absoluteFilePath(), newFile );
		}
	}

	if ( oldDir.isEmpty() )
	{
		oldDir.rmdir( InOldDir );
	}
}

void WeFileSystemModel::CopyDir( const QString& InOldDir, const QString& InNewDir, QProgressDialog* InProgressDialog, int& OutValueProgress )
{
	QDir				oldDir( InOldDir );

	if ( !oldDir.mkpath( InNewDir ) ) return;

	QFileInfoList						entryInfoList = oldDir.entryInfoList( QDir::AllEntries | QDir::NoDotAndDotDot | QDir::NoSymLinks );
	QString								newFile;

	for ( auto it = entryInfoList.begin(), itEnd = entryInfoList.end(); it != itEnd; ++it )
	{
		if ( it->isDir() )
		{
			CopyDir( it->absoluteFilePath(), InNewDir + "/" + it->fileName(), InProgressDialog, OutValueProgress );
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
														   TCHAR_TO_ANSI( FString::Format( TEXT( "Copy file exists %s %s" ), it->fileName().toStdWString().c_str(), InNewDir.toStdWString().c_str() ).c_str() ),
														   QMessageBox::YesAll | QMessageBox::Yes | QMessageBox::NoAll | QMessageBox::No );

						if ( applayMode == QMessageBox::No || applayMode == QMessageBox::NoToAll )
						{
							continue;
						}
					}
				}

				QFile::remove( newFile );
			}

			QFile::copy( it->absoluteFilePath(), newFile );
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
	if ( InRole == Qt::DecorationRole )
	{
		QFileInfo		fileInfo = WeFileSystemModel::fileInfo( InIndex );
		if ( fileInfo.isFile() )
		{
			// Icon for packages
			if ( fileInfo.suffix() == FILE_PACKAGE_EXTENSION )
			{
				if ( GPackageManager->IsPackageLoaded( appQtAbsolutePathToEngine( fileInfo.absoluteFilePath() ) ) )
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

	return QFileSystemModel::data( InIndex, InRole );
}