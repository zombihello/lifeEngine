/**
 * @file
 * @addtogroup Core Core
 *
 * @authors: Yehor Pohuliaka
 * @date: 3/27/2021
 */

#ifndef GLOBALS_H
#define GLOBALS_H

/**
 * @ingroup Core
 * @brief Regular logging
 */
extern class FOutputDevice*       GLog;

/**
 * @ingroup Core
 * @brief User interaction and non critical warnings
 */
extern class FOutputDevice*       GWarn;

/**
 * @ingroup Core
 * @brief Critical errors
 */
extern class FOutputDevice*       GError;

#endif // !GLOBALS_H
