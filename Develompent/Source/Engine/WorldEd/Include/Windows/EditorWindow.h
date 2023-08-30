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
#include "WorldEd.h"

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
	 * @brief Destructor
	 */
	~CEditorWindow();

	/**
	 * @brief Init
	 */
	virtual void Init() override;

	/**
	 * @brief Save level
	 *
	 * @param InIsSaveAs		Is save level as...
	 */
	void SaveLevel( bool InIsSaveAs = false );

protected:
	/**
	 * @brief Method tick interface of a layer
	 */
	virtual void OnTick() override;

	/**
	 * @brief Open level
	 */
	void OpenLevel();

private:
	/**
	 * @brief Update window title
	 */
	void UpdateWindowTitle();

	ImGuiDockNodeFlags												dockspaceFlags;					/**< ImGui dockspace flags */
	EditorDelegates::COnEditorCreatedNewMap::DelegateType_t*		editorCreatedNewMapDelegate;	/**< Delegate of created new map */
	EditorDelegates::COnEditorLoadedMap::DelegateType_t*			editorLoadedMapDelegate;		/**< Delegate of loaded map */
	EditorDelegates::COnEditorSavedMap::DelegateType_t*			editorSavedMapDelegate;			/**< Delegate of saved map */
	EditorDelegates::COnEditorMapMarkedDirty::DelegateType_t*		editorMapMarkedDirtyDelegate;	/**< Delegate of map marked dirty */
};

#endif // !EDITORWINDOW_H