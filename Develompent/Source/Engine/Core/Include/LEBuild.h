/**
 * @file
 * @addtogroup Core Core
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef LEBUILD_H
#define LEBUILD_H

// Current build is shipping?
#ifndef SHIPPING_BUILD
	#define SHIPPING_BUILD			SHIPPING
#endif // !SHIPPING_BUILD

// If current build not shipping - use  ImGui for debug
#ifndef WITH_IMGUI
	#if !SHIPPING_BUILD
		#define WITH_IMGUI 1
	#else
		#define WITH_IMGUI 0
	#endif // !SHIPPING_BUILD
#endif // !WITH_IMGUI

// Enable or disable checks in build
#ifndef DO_CHECK
	#define DO_CHECK				!SHIPPING_BUILD
#endif // !DO_CHECK

// Enable or disable in build editor
#ifndef WITH_EDITOR
	#define  WITH_EDITOR			!SHIPPING_BUILD
#endif // !WITH_EDITOR

// Enable or disable logging in build
#ifndef NO_LOGGING
	#define NO_LOGGING				SHIPPING_BUILD
#endif // !NO_LOGGING

// Enable or disable frame capture markers
#ifndef FRAME_CAPTURE_MARKERS
	#define FRAME_CAPTURE_MARKERS	!SHIPPING_BUILD
#endif // !FRAME_CAPTURE_MARKERS

/**
 * @ingroup Core
 * Checks to see if pure virtual has actually been implemented
 * 
 * @see Core.h
 */
#ifndef CHECK_PUREVIRTUALS
	#define CHECK_PUREVIRTUALS 0
#endif

#endif // !LEBUILD_H
