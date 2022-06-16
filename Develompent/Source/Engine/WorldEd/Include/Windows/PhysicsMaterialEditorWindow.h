/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef PHYSICSMATERIALEDITORWINDOW_H
#define PHYSICSMATERIALEDITORWINDOW_H

#include <QWidget>

#include "System/PhysicsMaterial.h"
#include "WorldEd.h"

namespace Ui
{
	class WePhysicsMaterialEditorWindow;
}

/**
 * @ingroup WorldEd
 * Class window of physics material editor
 */
class WePhysicsMaterialEditorWindow : public QWidget
{
	Q_OBJECT

public:
	/**
	 * Constructor
	 * 
	 * @param InPhysMaterial	Physics material to edit
	 * @param InParent			Parent widget
	 */
	WePhysicsMaterialEditorWindow( const TSharedPtr<CPhysicsMaterial>& InPhysMaterial, QWidget* InParent = nullptr );

	/**
	 * Destructor
	 */
	virtual ~WePhysicsMaterialEditorWindow();

private slots:
	//
	// Events
	//
	void on_doubleSpinBox_staticFriction_valueChanged( double InValue );
	void on_doubleSpinBox_dynamicFriction_valueChanged( double InValue );
	void on_doubleSpinBox_restitution_valueChanged( double InValue );
	void on_doubleSpinBox_density_valueChanged( double InValue );
	void on_comboBox_surfaceType_currentIndexChanged( int InIndex );

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
	 * Called event when asset is reloaded
	 * @param InAssets	Array of reloaded assets
	 */
	void OnAssetsReloaded( const std::vector< TSharedPtr<class CAsset> >& InAssets );

	bool													bInit;								/**< Is inited window */
	Ui::WePhysicsMaterialEditorWindow*						ui;									/**< Qt UI */
	TSharedPtr<CPhysicsMaterial>							physMaterial;						/**< Asset to edit */
	SEditorDelegates::COnAssetsReloaded::DelegateType_t*	assetsReloadedHandle;				/**< Handle delegate of reloaded assets */
};

#endif // PHYSICSMATERIALEDITORWINDOW_H