/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef IMPORTSETTINGSMESHDIALOG_H
#define IMPORTSETTINGSMESHDIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QPushButton>

#include "Math/Axis.h"
#include "Core.h"

/**
 * @ingroup WorldEd
 * Dialog window of settings import mesh
 */
class WeImportSettingsMeshDialog : public QDialog
{
	Q_OBJECT

public:
	/**
	 * Enumeration of exis up
	 */
	enum EAxis
	{
		A_PlusX,					/**< +X */
		A_MinusX,					/**< -X */
		A_PlusY,					/**< +Y */
		A_MinusY,					/**< -Y */
		A_PlusZ,					/**< +Z */
		A_MinusZ,					/**< -Z */

		A_Num,						/**< Number of axis ups */
		A_Default = A_PlusY,		/**< Default axis up */
	};

	/**
	 * Enumeration of dialog result
	 */
	enum EDialogResult
	{
		DR_ImportAll,		/**< Import all meshes with current settings */
		DR_Import,			/**< Import only one mesh with current settings */
		DR_Cancel			/**< Cancel from import meshes */
	};

	/**
	 * Struct of import settings
	 */
	struct SImportSettings
	{
		EAxis		axisUp;		/**< Axis up */
	};

	/**
	 * Event of close window
	 *
	 * @param InEvent Event of close event
	 */
	virtual void closeEvent( QCloseEvent* InEvent ) override;

	/**
	 * Get import settings
	 * 
	 * @param OutImportSettings	Output import settings
	 * @param InParent			Parent widget
	 * @return Return dialog result (see WeImportSettingsMeshDialog::EDialogResult)
	 */
	static EDialogResult GetImportSettings( SImportSettings& OutImportSettings, QWidget* InParent = nullptr );

private slots:
	//
	// Events
	//
	void OnComboBoxAxisUp_CurrentIndexChanged( int InIndex );
	void OnPushButtonImportAll_Clicked();
	void OnPushButtonImport_Clicked();
	void OnPushButtonCancel_Clicked();

private:
	/**
	 * Constructor
	 *
	 * @param InParent		Parent widget
	 */
	WeImportSettingsMeshDialog( QWidget* InParent = nullptr );

	/**
	 * Destructor
	 */
	virtual ~WeImportSettingsMeshDialog();

	/**
	 * Init UI
	 */
	void InitUI();

	/**
	 * Update UI
	 */
	void UpdateUI();

	SImportSettings	importSettings;
	QComboBox*		comboBox_AxisUp;		/**< Combo box of axis up in mesh */
	QPushButton*	pushButton_importAll;	/**< Import all meshes with current settings */
	QPushButton*	pushButton_import;		/**< Import one mesh with current settings */
	QPushButton*	pushButton_cancel;		/**< Cancel from import meshes */
};

#endif // !IMPORTSETTINGSMESHDIALOG_H