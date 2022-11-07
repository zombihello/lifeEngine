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

	/**
	 * @brief Method tick interface of a layer
	 */
	virtual void OnTick() override;
};

#endif // !EXPLORERLEVELWINDOW_H