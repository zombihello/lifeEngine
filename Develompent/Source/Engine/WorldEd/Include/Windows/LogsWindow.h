/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef LOGSWINDOW_H
#define LOGSWINDOW_H

#include "ImGUI/ImGUIEngine.h"

 /**
  * @ingroup WorldEd
  * @brief Window logs
  */
class CLogsWindow : public CImGUILayer
{
public:
	/**
	 * @brief Constructor
	 *
	 * @param InName	Window name
	 */
	CLogsWindow( const std::wstring& InName );

	/**
	 * @brief Method tick interface of a layer
	 */
	virtual void OnTick() override;
};

#endif // !LOGSWINDOW_H