/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef MATERIALEDITORWINDOW_H
#define MATERIALEDITORWINDOW_H

#include <QWidget>
#include "Render/Material.h"

namespace Ui
{
	class WeMaterialEditorWindow;
}

/**
 * @ingroup WorldEd
 * Class window of material editor
 */
class WeMaterialEditorWindow : public QWidget
{
	Q_OBJECT

public:
	/**
	 * Constructor
	 * 
	 * @param InMaterial	Material to edit
	 * @param InParent		Parent widget
	 */
	WeMaterialEditorWindow( FMaterial* InMaterial, QWidget* InParent = nullptr );

	/**
	 * Destructor
	 */
	virtual ~WeMaterialEditorWindow();

signals:
	/**
	 * Event when material is changed
	 * 
	 * @param InAsset		Changed asset
	 */
	void OnChangedAsset( FAsset* InAsset );

private:
	/**
	 * Init UI
	 */
	void InitUI();

	bool						bInit;						/**< Is inited window */
	Ui::WeMaterialEditorWindow*	ui;							/**< Qt UI */
	FMaterialRef				material;					/**< Editable asset */
	class QCheckBox*			checkBox_isTwoSided;		/**< Check box of is two sided */
	class QCheckBox*			checkBox_isWireframe;		/**< Check box of is wireframe */
	class QCheckBox*			checkBox_staticMesh;		/**< Check box of static mesh */
	class QCheckBox*			checkBox_sprite;			/**< Check box of sprite */
	class QComboBox*			comboBox_vertexShader;		/**< Combo box of vertex shader */
	class QComboBox*			comboBox_hullShader;		/**< Combo box of hull shader */
	class QComboBox*			comboBox_domainShader;		/**< Combo box of domain shader */
	class QComboBox*			comboBox_pixelShader;		/**< Combo box of pixel shader */
	class QComboBox*			comboBox_geometryShader;	/**< Combo box of geometry shader */
	class QComboBox*			comboBox_diffuseTexture;	/**< Combo box for select diffuse texture */
	class QToolButton*			toolButton_removeDiffuse;	/**< Tool button for remove diffuse texture */
};

#endif // MATERIALEDITORWINDOW_H