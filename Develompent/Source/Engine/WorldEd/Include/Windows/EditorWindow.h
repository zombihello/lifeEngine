/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef EDITORWINDOW_H
#define EDITORWINDOW_H

#include "ImGUI/ImGUIEngine.h"

/**
  * @ingroup WorldEd
  * @brief Main editor window
  */
class CEditorWindow : public CImGUILayer
{
public:
	/**
	 * @brief Constructor
	 */
	CEditorWindow();

	/**
	 * @brief Init
	 */
	virtual void Init() override;

protected:
	/**
	 * @brief Method tick interface of a layer
	 */
	virtual void OnTick() override;

private:
	ImGuiDockNodeFlags		dockspaceFlags;		/**< ImGui dockspace flags */
};

#endif // !EDITORWINDOW_H