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
extern class BaseLogger*			GLog;

/**
 * @ingroup Core
 * @brief File system
 */
extern class BaseFileSystem*		GFileSystem;

/**
 * @ingroup Core
 * @brief Window
 */
extern class BaseWindow*			GWindow;

/**
 * @ingroup Core
 * @brief Config of engine
 */
extern class Config					GEngineConfig;

/**
 * @ingroup Core
 * @brief Config of game
 */
extern class Config					GGameConfig;

/**
 * @ingroup Core
 * @brief Indicates that MainLoop() should be exited at the end of the current iteration
 */
extern bool							GIsRequestingExit;

#endif // !COREGLOBALS_H
