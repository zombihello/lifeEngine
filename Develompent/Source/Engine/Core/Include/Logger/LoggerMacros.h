/**
 * @file
 * @addtogroup Core Core
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef LOGGERMACROS_H
#define LOGGERMACROS_H

#include "LEBuild.h"
#include "Misc/CoreGlobals.h"
#include "Logger/BaseLogger.h"

// If configuration is not shipping - we using logs for debug
#if !NO_LOGGING || PLATFORM_DOXYGEN
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
	 
	 /**
	   * @ingroup Core
	   * @brief Macro for print message to log with custom color
	   * @warning In shipping this macro is empty and logging disabled
	   *
	   * @param[in] InColor Color message
	   * @param[in] InType Type message
	   * @param[in] InCategory Category of message
	   * @param[in] InMessage Message
	   * @param[in] ... Other arguments of message
	   */
	#define LE_LOG_COLOR( InColor, InType, InCategory, InMessage, ...  ) \
		{ \
			GLog->SetTextColor( InColor ); \
			GLog->Logf( InType, InCategory, InMessage, __VA_ARGS__ ); \
			GLog->ResetTextColor(); \
		}
#else
	#define LE_LOG( InType, InCategory, InMessage, ... )
	#define LE_LOG_COLOR( InColor, InType, InCategory, InMessage, ...  )
#endif // !NO_LOGGING || PLATFORM_DOXYGEN

#endif // !LOGGERMACROS_H
