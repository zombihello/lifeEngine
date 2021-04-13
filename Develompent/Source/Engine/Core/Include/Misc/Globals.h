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
extern class OutputDevice*       GLog;

/**
 * @ingroup Core
 * @brief User interaction and non critical warnings
 */
extern class OutputDevice*       GWarn;

/**
 * @ingroup Core
 * @brief Critical errors
 */
extern class OutputDevice*       GError;

#endif // !GLOBALS_H
