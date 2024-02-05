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

#ifndef ACTORPROPERTIESWINDOW_H
#define ACTORPROPERTIESWINDOW_H

#include <list>
#include <unordered_map>
#include <unordered_set>
#include <string>

#include "UI/ImGUI/ImGUIEngine.h"
#include "Editor/Editor.h"
#include "System/ObjectPropertyNode.h"

/**
 * @ingroup WorldEd
 * @brief Window of actor properties
 */
class CActorPropertiesWindow : public CImGUILayer
{
public:
	/**
	 * @brief Constructor
	 *
	 * @param InName	Window name
	 */
	CActorPropertiesWindow( const std::wstring& InName );

	/**
	 * @brief Destructor
	 */
	~CActorPropertiesWindow();

protected:
	/**
	 * @brief Method tick interface of a layer
	 */
	virtual void OnTick() override;

private:
	/**
	 * @brief Event called when actors selected or unselected
	 * 
	 * @param InActors	Selected or unselected actors
	 */
	void OnActorsUnSelected( const std::vector<ActorRef_t>& InActors );

	/**
	 * @brief Event called when map changed (created or loaded)
	 */
	void OnMapChanged();

	/**
	 * @brief Destroy property tree
	 */
	void DestroyPropertyTree();
	
	CObjectPropertyNode*													rootPropertyNode;			/**< Root property node */
	EditorDelegates::COnActorsSelected::DelegateType_t*						actorsSelectedDelegate;		/**< Actors selected delegate */
	EditorDelegates::COnActorsUnselected::DelegateType_t*					actorsUnselectedDelegate;	/**< Actors unselected delegate */
	EditorDelegates::COnEditorCreatedNewMap::DelegateType_t*				createdNewMapDelegate;		/**< Created new map delegate */
	EditorDelegates::COnEditorLoadedMap::DelegateType_t*					loadedMapDelegate;			/**< Loaded map delegate */
};

#endif // !ACTORPROPERTIESWINDOW_H