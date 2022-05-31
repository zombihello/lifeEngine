/**
 * @file
 * @addtogroup UI User interface
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef UIGLOBALS_H
#define UIGLOBALS_H

#include "LEBuild.h"

#if WITH_IMGUI
	 /**
	  * @ingroup UI
	  * @brief ImGUI engine
	  */
	extern class ĐImGUIEngine*				GImGUIEngine;
#endif // WITH_IMGUI

extern class ĐUIEngine*						GUIEngine;

#endif // !UIGLOBALS_H
