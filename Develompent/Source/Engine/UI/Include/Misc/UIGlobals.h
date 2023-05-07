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
	extern class CImGUIEngine*				g_ImGUIEngine;
#endif // WITH_IMGUI

extern class CUIEngine*						g_UIEngine;

#endif // !UIGLOBALS_H
