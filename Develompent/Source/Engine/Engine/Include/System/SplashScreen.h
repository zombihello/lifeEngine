/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef SPLASHSCREEN_H
#define SPLASHSCREEN_H

#include "Core.h"

/**
 * @ingroup Engine
 * SplashTextType defines the types of text on the splash screen
 */
enum ESplashTextType
{
	STT_StartupProgress = 0,		/**< Startup progress text */
	STT_VersionInfo1,				/**< Version information text line 1 */
	STT_CopyrightInfo,				/**< Copyright information text */
	STT_GameName,					/**< Game name */
	STT_NumTextTypes				/**< Number of text types (must be final enum value) */
};

/**
 * @ingroup Engine
 * Displays a splash window with the specified image. The function does not use wxWidgets. The splash
 * screen variables are stored in static global variables (SplashScreen*). If the image file doesn't exist,
 * the function does nothing.
 *
 * @param[in] InSplashName Name of splash screen file (and relative path if needed)
 */
extern void Sys_ShowSplash( const tchar* InSplashName );

/**
 * @ingroup Engine
 * Destroys the splash window that was previously shown by Sys_ShowSplash(). If the splash screen isn't active,
 * it will do nothing.
 */
extern void Sys_HideSplash();

/**
 * @ingroup Engine
 * Sets the text displayed on the splash screen (for startup/loading progress)
 *
 * @param[in] InType Type of text to change
 * @param[in] InText Text to display
 */
extern void Sys_SetSplashText( const ESplashTextType InType, const tchar* InText );

#endif // !SPLASHSCREEN_H