/**
 * @file
 * @addtogroup Core Core
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef COREGLOBALS_H
#define COREGLOBALS_H

/**
 * @ingroup Core
 * @brief Logger
 */
extern class FBaseLogger*			GLog;

/**
 * @ingroup Core
 * @brief File system
 */
extern class FBaseFileSystem*		GFileSystem;

/**
 * @ingroup Core
 * @brief Window
 */
extern class FBaseWindow*			GWindow;

/**
 * @ingroup Core
 * @brief Config of engine
 */
extern class FConfig				GEngineConfig;

/**
 * @ingroup Core
 * @brief Config of game
 */
extern class FConfig				GGameConfig;

/**
 * @ingroup Core
 * @brief Config of input system
 */
extern class FConfig				GInputConfig;

#if WITH_EDITOR
/**
 * @ingroup Core
 * @brief Config of editor
 */
extern class FConfig				GEditorConfig;
#endif // WITH_EDITOR

/**
 * @ingroup Core
 * @brief Indicates that MainLoop() should be exited at the end of the current iteration
 */
extern bool							GIsRequestingExit;

#if WITH_EDITOR
/**
 * @ingroup Core
 * Whether engine was launched for editing
 */
extern bool							GIsEditor;

/**
 * @ingroup Core
 * Whether engine was launched for commandlet
 */
extern bool							GIsCommandlet;
#else
#define								GIsEditor 0
#define								GIsCommandlet 0
#endif // WITH_EDITOR

/**
 * @ingroup Core
 * Seconds per CPU cycle for this PC 
 */
extern double						GSecondsPerCycle;

/**
* @ingroup Core
* Time at which appSeconds() was first initialized (very early on)
*/
extern double						GStartTime;

/**
 * @ingroup Core
 * Current delta time in seconds
 */
extern double						GDeltaTime;

/**
 * @ingroup Core
 * Current time in seconds
 */
extern double						GCurrentTime;

/**
 * @ingroup Core
 * Last time in seconds
 */
extern double						GLastTime;

/**
 * @ingroup Core
 * Package manager
 */
extern class FPackageManager*		GPackageManager;

#endif // !COREGLOBALS_H
