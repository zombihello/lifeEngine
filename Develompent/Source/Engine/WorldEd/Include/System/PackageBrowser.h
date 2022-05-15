/**
 * @file
 * @addtogroup WorldEd WorldEd
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef PACKAGEBROWSER_H
#define PACKAGEBROWSER_H

#include <qlistview.h>
#include <qproxystyle.h>

#include "System/PackageModel.h"

/**
 * @ingroup WorldEd
 * Qt package browser
 */
class WePackageBrowser : public QListView
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
	WePackageBrowser( QWidget* InParent = nullptr );

    /**
     * Destructor
     */
    virtual ~WePackageBrowser();

	/**
	 * Refresh browser
	 */
	FORCEINLINE void Refresh()
	{
		packageModel->Refresh();
		repaint();
	}

	/**
	 * Set selected package
	 * @param InPackage Package
	 */
	FORCEINLINE void SetPackage( FPackage* InPackage )
	{
		check( packageModel );
		packageModel->SetPackage( InPackage );
	}

	/**
	 * Get selected package
	 * @return Return selected package. If not selected returning nullptr
	 */
	FORCEINLINE FPackageRef GetPackage() const
	{
		check( packageModel );
		return packageModel->GetPackage();
	}

private slots:
	//
	// Events
	//
	void OnClicked( QModelIndex InIndex );

signals:
	/**
	 * Event of selection asset
	 * @param InAssetReference	Asset reference
	 */
	void OnSelectedAsset( const std::wstring& InAssetReference );

private:
	/**
	 * Event of press mouse
	 *
	 * @param InEvent		Mouse event of press button
	 */
	void mousePressEvent( QMouseEvent* InEvent );

	WeStyle*				style;				/**< Style of content browser */
	WePackageModel*			packageModel;		/**< Package model */
};

//-------------------------------------------------------------------------//

#endif // PACKAGEBROWSER_H
