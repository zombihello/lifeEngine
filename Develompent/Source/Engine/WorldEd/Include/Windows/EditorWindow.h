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

protected:
	/**
	 * @brief Method tick interface of a layer
	 */
	virtual void OnTick() override;

	/**
	 * @brief Save level
	 * 
	 * @param InIsSaveAs		Is save level as...
	 * @param InEventResponse	Event used when opened popup. This is need for waiting his response
	 */
	void SaveLevel( bool InIsSaveAs = false, CEvent* InEventResponse = nullptr );

private:
	/**
	 * @brief Update window title
	 */
	void UpdateWindowTitle();

	ImGuiDockNodeFlags												dockspaceFlags;					/**< ImGui dockspace flags */
	SEditorDelegates::COnEditorCreatedNewMap::DelegateType_t*		editorCreatedNewMapDelegate;	/**< Delegate of created new map */
	SEditorDelegates::COnEditorLoadedMap::DelegateType_t*			editorLoadedMapDelegate;		/**< Delegate of loaded map */
	SEditorDelegates::COnEditorSavedMap::DelegateType_t*			editorSavedMapDelegate;			/**< Delegate of saved map */
	SEditorDelegates::COnEditorMapMarkedDirty::DelegateType_t*		editorMapMarkedDirtyDelegate;	/**< Delegate of map marked dirty */
};

#endif // !EDITORWINDOW_H