/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef TEXTUREEDITORWINDOW_H
#define TEXTUREEDITORWINDOW_H

#include <QWidget>
#include "Render/Texture.h"
#include "WorldEd.h"

namespace Ui
{
	class WeTextureEditorWindow;
}

/**
 * @ingroup WorldEd
 * Class window of texture editor
 */
class WeTextureEditorWindow : public QWidget
{
	Q_OBJECT

public:
	/**
	 * Constructor
	 * 
	 * @param InTexture2D	Texture 2D to edit
	 * @param InParent		Parent widget
	 */
	WeTextureEditorWindow( const TSharedPtr<CTexture2D>& InTexture2D, QWidget* InParent = nullptr );

	/**
	 * Destructor
	 */
	virtual ~WeTextureEditorWindow();

private slots:
	//
	// Events
	//
	void on_comboBox_addressU_currentIndexChanged( int InIndex );
	void on_comboBox_addressV_currentIndexChanged( int InIndex );
	void on_comboBox_filter_currentIndexChanged( int InIndex );
	void on_toolButton_sourceFile_clicked();
	void on_toolButton_sourceFileRemove_clicked();
	void on_actionReimport_triggered();
	void on_actionR_toggled( bool InValue );
	void on_actionG_toggled( bool InValue );
	void on_actionB_toggled( bool InValue );
	void on_actionA_toggled( bool InValue );

signals:
	/**
	 * Event when texture is changed
	 * 
	 * @param InAsset		Changed asset
	 */
	void OnChangedAsset( const TSharedPtr<CAsset>& InAsset );

private:
	/**
	 * Init UI
	 */
	void InitUI();

	/**
	 * Update UI
	 */
	void UpdateUI();

	/**
	 * Called when source file changed
	 * 
	 * @param InNewSourceFile		New path to source file
	 */
	void OnSourceFileChanged( QString InNewSourceFile );

	/**
	 * Crop source file text in UI
	 */
	void UICropSourceFileText();

	/**
	 * Event called when resize widget
	 *
	 * @param[in] InEvent Event of resize widget
	 */
	virtual void resizeEvent( QResizeEvent* InEvent ) override;

	/**
	 * Called event when asset is reloaded
	 * @param InAssets	Array of reloaded assets
	 */
	void OnAssetsReloaded( const std::vector< TSharedPtr<class CAsset> >& InAssets );

	bool												bInit;								/**< Is inited window */
	Ui::WeTextureEditorWindow*							ui;									/**< Qt UI */
	TSharedPtr<CTexture2D>								texture2D;							/**< Editable asset */
	class CTexturePreviewViewportClient*				viewportClient;						/**< Viewport client */
	class QComboBox*									comboBox_addressU;					/**< Combobox for select address mode by U */
	class QComboBox*									comboBox_addressV;					/**< Combobox for select address mode by V */
	class QComboBox*									comboBox_filter;					/**< Combobox for select sampler filter */
	class QLabel*										label_sourceFileValue;				/**< Label for show path to source file */
	class QToolButton*									toolButton_sourceFile;				/**< Tool button for select new path to source file */
	class QToolButton*									toolButton_sourceFileRemove;		/**< Tool button for remove path to source file */
	SEditorDelegates::COnAssetsReloaded::DelegateType_t* assetsReloadedHandle;				/**< Handle delegate of reloaded assets */
};

#endif // MAINWINDOW_H