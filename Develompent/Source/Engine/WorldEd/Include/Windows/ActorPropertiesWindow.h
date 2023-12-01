/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef ACTORPROPERTIESWINDOW_H
#define ACTORPROPERTIESWINDOW_H

#include <list>
#include <unordered_map>
#include <unordered_set>
#include <string>

#include "ImGUI/ImGUIEngine.h"
#include "System/ObjectPropertyNode.h"
#include "WorldEd.h"

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
	void OnActorsUnSelected( const std::vector<AActor*>& InActors );

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