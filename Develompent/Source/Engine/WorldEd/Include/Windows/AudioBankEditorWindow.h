/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef AUDIOBANKEDITORWINDOW_H
#define AUDIOBANKEDITORWINDOW_H

#include <QWidget>

#include "System/AudioBank.h"
#include "WorldEd.h"

namespace Ui
{
	class WeAudioBankEditorWindow;
}

/**
 * @ingroup WorldEd
 * Class window of audio bank editor
 */
class WeAudioBankEditorWindow : public QWidget
{
	Q_OBJECT

public:
	/**
	 * Constructor
	 * 
	 * @param InAudioBank	Audio bank to preview
	 * @param InParent		Parent widget
	 */
	WeAudioBankEditorWindow( const TSharedPtr<CAudioBank>& InAudioBank, QWidget* InParent = nullptr );

	/**
	 * Destructor
	 */
	virtual ~WeAudioBankEditorWindow();

private slots:
	//
	// Events
	//
	void on_toolButton_sourceFile_clicked();
	void on_toolButton_sourceFileRemove_clicked();
	void on_actionReimport_triggered();
	void OnAudioPlay();
	void OnAudioPause();
	void OnAudioStop();

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

	bool													bInit;								/**< Is inited window */
	Ui::WeAudioBankEditorWindow*							ui;									/**< Qt UI */
	TSharedPtr<CAudioBank>									audioBank;							/**< Preview asset */
	class QLabel*											label_sourceFileValue;				/**< Label for show path to source file */
	class QToolButton*										toolButton_sourceFile;				/**< Tool button for select new path to source file */
	class QToolButton*										toolButton_sourceFileRemove;		/**< Tool button for remove path to source file */
	class QPushButton*										pushButton_play;					/**< Push button for play audio bank */
	class QPushButton*										pushButton_pause;					/**< Push button for pause audio bank */
	class QPushButton*										pushButton_stop;					/**< Push button for stop audio bank */
	class CAudioComponent*									audioComponent;						/**< Audio component */
	SEditorDelegates::COnAssetsReloaded::DelegateType_t*	assetsReloadedHandle;				/**< Handle delegate of reloaded assets */
};

#endif // AUDIOBANKEDITORWINDOW_H