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

#endif // !LEBUILD_H