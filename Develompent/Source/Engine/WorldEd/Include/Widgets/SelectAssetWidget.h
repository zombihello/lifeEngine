/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef SELECTASSETWIDGET_H
#define SELECTASSETWIDGET_H

#include <QWidget>

/**
 * @ingroup WorldEd
 * Widget for select asset from content browser
 */
class WeSelectAssetWidget : public QWidget
{
	Q_OBJECT

public:
	/**
	 * Constructor
	 * @param InParent		Parent
	 */
	WeSelectAssetWidget( QWidget* InParent = nullptr );

	/**
	 * Destructor
	 */
	virtual ~WeSelectAssetWidget();

	/**
	 * Clear asset reference
	 * @param InIsEmitEvent		Is need emit event of change asset reference
	 */
	void ClearAssetReference( bool InIsEmitEvent = true );

	/**
	 * Set asset reference
	 * 
	 * @param InAssetReference	Asset reference in format <PackageName>:<AssetName>
	 * @param InIsEmitEvent		Is need emit event of change asset reference
	 */
	void SetAssetReference( const std::wstring& InAssetReference, bool InIsEmitEvent = true );

private slots:
	//
	// Events
	//
	void OnLineEditAssetReferenceReturnPressed();
	void OnRemoveAsset();
	void OnPasteAssetFromContentBrowser();
	void OnShowAssetInContentBrowser();

signals:
	/**
	 * Event of changed asset reference
	 */
	void OnAssetReferenceChanged( const std::wstring& InNewAssetReference );

private:
	/**
	 * Init UI
	 */
	void InitUI();

	class QHBoxLayout*		horizontalLayout;			/**< Horizontal layout */
	class QLineEdit*		lineEdit_assetReference;	/**< Reference to asset */
	class QToolButton*		toolButton_pasteAsset;		/**< Paste selected asset in content browser */
	class QToolButton*		toolButton_browseAsset;		/**< Show asset in content browser */
	class QToolButton*		toolButton_removeAsset;		/**< Remove reference to asset */
};

#endif // !LOGWIDGET_H