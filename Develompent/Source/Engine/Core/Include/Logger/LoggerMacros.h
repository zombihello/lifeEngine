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
	 * @param[in] InMessage Message
	 * @param[in] ... Other arguments of message
	 */
	#define Logf( InMessage, ... )				g_Log->Printf( LT_Log, InMessage, __VA_ARGS__ )
	 
	 /**
	  * @ingroup Core
	  * @brief Macro for print warning to log
	  * @warning In shipping this macro is empty and logging disabled
	  *
	  * @param[in] InMessage Message
	  * @param[in] ... Other arguments of message
	  */
	#define Warnf( InMessage, ... )				g_Log->Printf( LT_Warning, InMessage, __VA_ARGS__ )

	/**
	 * @ingroup Core
	 * @brief Macro for print error to log
	 * @warning In shipping this macro is empty and logging disabled
	 *
	 * @param[in] InMessage Message
	 * @param[in] ... Other arguments of message
	 */
	#define Errorf( InMessage, ... )			g_Log->Printf( LT_Error, InMessage, __VA_ARGS__ )
#else
	#define Logf( InMessage, ... )
	#define Warnf( InMessage, ... )
	#define Errorf( InMessage, ... )
#endif // !NO_LOGGING || PLATFORM_DOXYGEN

#endif // !LOGGERMACROS_H
