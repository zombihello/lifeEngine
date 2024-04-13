/**
 * @file
 * @addtogroup Core Core
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef COREGLOBALS_H
#define COREGLOBALS_H

#include <string>

/**
 * @ingroup Core
 * @brief Logger
 */
extern class CBaseLogger*			g_Log;

/**
 * @ingroup Core
 * @brief File system
 */
extern class CBaseFileSystem*		g_FileSystem;

/**
 * @ingroup Core
 * @brief Window
 */
extern class CBaseWindow*			g_Window;

/**
 * @ingroup Core
 * @brief Indicates that MainLoop() should be exited at the end of the current iteration
 */
extern bool							g_IsRequestingExit;

/**
 * @ingroup Core
 * @brief Indicated that execution is happening within MainLoop()
 */
extern bool							g_IsRunning;

/**
 * @ingroup Core
 * @brief Whether we are still in the initial loading process
 */
extern bool							g_IsInitialLoad;

#if WITH_EDITOR
/**
 * @ingroup Core
 * @brief Whether engine was launched for game
 */
extern bool							g_IsGame;

/**
 * @ingroup Core
 * Whether engine was launched for editing
 */
extern bool							g_IsEditor;

/**
 * @ingroup Core
 * Whether engine was launched for commandlet
 */
extern bool							g_IsCommandlet;

/**
 * @ingroup Core
 * Whether engine was launched for cooked packages
 */
extern bool							g_IsCooker;

/**
 * @ingroup Core
 * Whether we should pause before exiting. Used by commandlets
 */
extern bool							g_ShouldPauseBeforeExit;
#else
#define								g_IsGame 1
#define								g_IsEditor 0
#define								g_IsCommandlet 0
#define								g_IsCooker 0
#endif // WITH_EDITOR

/**
 * @ingroup Core
 * Seconds per CPU cycle for this PC 
 */
extern double						g_SecondsPerCycle;

/**
* @ingroup Core
* Time at which Sys_Seconds() was first initialized (very early on)
*/
extern double						g_StartTime;

/**
 * @ingroup Core
 * Current delta time in seconds
 */
extern double						g_DeltaTime;

/**
 * @ingroup Core
 * Current time in seconds
 */
extern double						g_CurrentTime;

/**
 * @ingroup Core
 * Last time in seconds
 */
extern double						g_LastTime;

/**
 * @ingroup Core
 * Package manager
 */
extern class CPackageManager*		g_PackageManager;

/**
 * @ingroup Core
 * Table of contents
 */
extern class CTableOfContets		g_TableOfContents;

/**
 * @ingroup Core
 * Game name
 */
extern std::wstring					g_GameName;

/**
 * @ingroup Core
 * Platform type
 */
extern enum EPlatformType			g_Platform;

/**
 * @ingroup Core
 * @brief Command line
 */
extern class CCommandLine			g_CommandLine;

/**
 * @ingroup Core
 * @brief Asset factory
 */
extern class CAssetFactory			g_AssetFactory;

#endif // !COREGLOBALS_H
