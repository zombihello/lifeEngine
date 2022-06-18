/**
 * @file
 * @addtogroup WorldEd WorldEd
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef FILESYSTEMMODEL_H
#define FILESYSTEMMODEL_H

#include <qfilesystemmodel.h>
#include <qprogressdialog.h>
#include <qmessagebox.h>

/**
 * @ingroup WorldEd
 * Package extension
 */
#define FILE_PACKAGE_EXTENSION		"pak"

/**
 * @ingroup WorldEd
 * Map extension
 */
#define  FILE_MAP_EXTENSION			"map"

/**
 * @ingroup WorldEd
 * Qt file system model
 */
class WeFileSystemModel : public QFileSystemModel
{
public:
	/**
	 * Constructor
	 * 
	 * @param InParent Parent
	 */
	WeFileSystemModel( QObject* InParent = nullptr );

	/**
	 * Flags for files being dragged
	 */
	Qt::ItemFlags flags( const QModelIndex& InIndex ) const;

	/**
	 * File move event on widget
	 */
	bool dropMimeData( const QMimeData* InData, Qt::DropAction InAction, int InRow, int InColumn, const QModelIndex& InParent );

	/**
	 * Returns the data stored under the given role for the item referred to by the index
	 */
	QVariant data( const QModelIndex& InIndex, int InRole = Qt::DisplayRole ) const override;

private:
	/**
	 * File move event in explorer
	 */
	void dropMimeData_moveFiles( const QMimeData* InData, Qt::DropAction InAction, const QModelIndex& InParent, int InCountFiles );

	/**
	 * File copy event in explorer
	 */
	void dropMimeData_copyFiles( const QMimeData* InData, Qt::DropAction InAction, const QModelIndex& InParent, int InCountFiles );

	/**
	 * Move Directory
	 */
	void MoveDir( const QString& InOldDir, const QString& InNewDir, QProgressDialog* InProgressDialog, int& OutValueProgress, bool& OutIsDirtyTOC, std::vector< QString >& OutUsedPackage );

	/**
	 * Copy Directory
	 */
	void CopyDir( const QString& InOldDir, const QString& InNewDir, QProgressDialog* InProgressDialog, int& OutValueProgress, bool& OutIsDirtyTOC, std::vector< QString >& OutUsedPackage );

	/**
	 * Count the number of files in a directory
	 */
	int CountFilesInDir( const QString& InDir, int& OutCountFiles, bool InIsLinkHidden = false );

	QMessageBox::StandardButton			applayMode;		/**< Applay mode */
	QModelIndex*						dropIndex;		/**< Drop index */
};

#endif // WeFileSystemModel
