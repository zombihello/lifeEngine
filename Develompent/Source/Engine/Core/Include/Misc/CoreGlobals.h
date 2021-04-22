/**
 * @file
 * @addtogroup Core Core
 *
 * @authors: Yehor Pohuliaka
 * @date: 3/27/2021
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
 * @brief Indicates that MainLoop() should be exited at the end of the current iteration
 */
extern bool							GIsRequestingExit;

#endif // !COREGLOBALS_H
