/**
 * @file
 * @addtogroup Core Core
 *
 * @authors: Yehor Pohuliaka
 * @date: 4/15/2021
 */

#ifndef LOGGERMACROS_H
#define LOGGERMACROS_H

#include "BaseLogger.h"

// If configuration is not shipping - we using logs for debug
#if !SHIPPING_BUILD || PLATFORM_DOXYGEN
	/**
	 * @ingroup Core
	 * @brief Macro for print message to log
	 * @warning In shipping this macro is empty and logging disabled
	 * 
	 * @param[in] InType Type message
	 * @param[in] InCategory Category of message
	 * @param[in] InMessage Message
	 * @param[in] ... Other arguments of message
	 */
	#define LE_LOG( InType, InCategory, InMessage, ... )				GLog->Logf( InType, InCategory, InMessage, __VA_ARGS__ )
#else
	#define LE_LOG( InType, InCategory, InMessage, ... )
#endif // !SHIPPING_BUILD || PLATFORM_DOXYGEN

#endif // !LOGGERMACROS_H
