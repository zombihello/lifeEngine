/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef ACTORCLASSESWINDOW_H
#define ACTORCLASSESWINDOW_H

#include "ImGUI/ImGUIEngine.h"

/**
 * @ingroup WorldEd
 * @brief Window of actor classes
 */
class CActorClassesWindow : public CImGUILayer
{
public:
	/**
	 * @brief Constructor
	 *
	 * @param InName	Window name
	 */
	CActorClassesWindow( const std::wstring& InName );

protected:
	/**
	 * @brief Method tick interface of a layer
	 */
	virtual void OnTick() override;
};

#endif // !ACTORCLASSESWINDOW_H