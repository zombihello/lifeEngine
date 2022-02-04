/**
 * @file
 * @addtogroup WorldEd WorldEd
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef EXPLOERFOLDERS_H
#define EXPLOERFOLDERS_H

#include <qtreeview.h>
#include <qfilesystemmodel.h>
#include <qproxystyle.h>

#include "FileSystemModel.h"
#include "System/Package.h"

/**
 * @ingroup WorldEd
 * Qt content browser
 */
class WeContentBrowser : public QTreeView
{
	Q_OBJECT

public:
	/**
	 * Class of style this content browser
	 */
	class WeStyle : public QProxyStyle
	{
	public:
		/**
		 * Event draw primitive
		 */
		void drawPrimitive( PrimitiveElement InElement, const QStyleOption* InOption, QPainter* InPainter, const QWidget* InWidget ) const;
	};

    /**
     * Constructor
     */
	WeContentBrowser( QWidget* InParent = nullptr );

    /**
     * Destructor
     */
    virtual ~WeContentBrowser();

	/**
	 * Set root dir for content browser
	 */
    virtual void SetRootDir( const QDir& InRootDir );

	/**
	 * Get file system model
	 * @return Return pointer to file system model
	 */
	virtual QFileSystemModel* GetFileSystemModel() const;

	/**
	 * Get root dir of content browser
	 * @return Return root dir
	 */
	virtual  const QDir& GetRootDir() const;

protected:
	/**
	 * Event of press mouse
	 * 
	 * @param InEvent Mouse event of press button
	 */
	void mousePressEvent( QMouseEvent* InEvent );

	/**
	 * Event drag enter in content browser
	 * 
	 * @param InEvent Event of drag enter
	 */
	void dragEnterEvent( QDragEnterEvent* InEvent );

	/**
	 * Event draw move in content browser
	 * 
	 * @param InEvent Event of drag move
	 */
	void dragMoveEvent( QDragMoveEvent* InEvent );

	/**
	 * Event 'drop' item in widget
	 * 
	 * @param InEvent Event of drop item
	 */
	void dropEvent( QDropEvent* Event );

private slots:
	/**
	 * Event of double click on item
	 */
	void on_treeView_contentBrowser_doubleClicked( const QModelIndex& InIndex );

signals:
	/**
	 * Event on open package
	 * @param InPackage Package
	 */
	void OnOpenPackage( FPackage* InPackage );

private:
	QDir					rootDir;			/**< Root dir for content browser */
	WeFileSystemModel*		fileSystemModel;	/**< File system model */
	WeStyle*				style;				/**< Style of content browser */
};

//-------------------------------------------------------------------------//

#endif // EXPLOERFOLDERS_H
