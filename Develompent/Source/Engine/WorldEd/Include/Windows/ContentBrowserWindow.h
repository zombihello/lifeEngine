/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef CONTENTBROWSERWINDOW_H
#define CONTENTBROWSERWINDOW_H

#include "ImGUI/ImGUIEngine.h"

 /**
  * @ingroup WorldEd
  * @brief Window content browser
  */
class CContentBrowserWindow : public CImGUILayer
{
public:
	/**
	 * @brief Constructor
	 *
	 * @param InName	Window name
	 */
	CContentBrowserWindow( const std::wstring& InName );

	/**
	 * @brief Method tick interface of a layer
	 */
	virtual void OnTick() override;
};

#endif // !CONTENTBROWSERWINDOW_H