/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef EXPLORERLEVELWINDOW_H
#define EXPLORERLEVELWINDOW_H

#include "ImGUI/ImGUIEngine.h"

 /**
  * @ingroup WorldEd
  * @brief Window explorer level
  */
class CExplorerLevelWindow : public CImGUILayer
{
public:
	/**
	 * @brief Constructor
	 *
	 * @param InName	Window name
	 */
	CExplorerLevelWindow( const std::wstring& InName );

protected:
	/**
	 * @brief Method tick interface of a layer
	 */
	virtual void OnTick() override;

	/**
	 * @brief Draw popup menu
	 */
	void DrawPopupMenu();

	/**
	 * @brief Process table's item events
	 * 
	 * @param InIndex	Item index
	 * @param InActor	Actor, him is owner events
	 */
	void ProcessItemEvents( uint32 InIndex, ActorRef_t InActor );
};

#endif // !EXPLORERLEVELWINDOW_H