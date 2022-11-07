/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef VIEWPORTWINDOW_H
#define VIEWPORTWINDOW_H

#include "ImGUI/ImGUIEngine.h"

 /**
  * @ingroup WorldEd
  * @brief Viewport window
  */
class CViewportWindow : public CImGUILayer
{
public:
	/**
	 * @brief Constructor
	 *
	 * @param InName	Window name
	 */
	CViewportWindow( const std::wstring& InName );

	/**
	 * @brief Method tick interface of a layer
	 */
	virtual void OnTick() override;
};

#endif // !VIEWPORTWINDOW_H