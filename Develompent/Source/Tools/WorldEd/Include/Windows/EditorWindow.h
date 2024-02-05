/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * ************************************************************
 *                  This file is part of:
 *                      LIFEENGINE
 *          https://github.com/zombihello/lifeEngine
 * ************************************************************
 * Copyright (C) 2024 Yehor Pohuliaka.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef EDITORWINDOW_H
#define EDITORWINDOW_H

#include "UI/ImGUI/ImGUIEngine.h"
#include "Editor/Editor.h"

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