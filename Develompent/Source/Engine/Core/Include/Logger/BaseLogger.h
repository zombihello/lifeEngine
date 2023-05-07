/**
 * @file
 * @addtogroup Core Core
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef BASELOGGER_H
#define BASELOGGER_H

#include "Core.h"
#include "Scripts/ScriptEngine.h"

/**
 * @ingroup Core
 * @brief Enumerating of log types
 */
enum ELogType
{
	LT_Log,                 /**< Normal message display */
	LT_Warning,             /**< Displaying a warning */
	LT_Error                /**< Error message */
};

/**
 * @ingroup Core
 * @brief Enumerating of log color
 */
enum ELogColor
{
    LC_Default,         /**< Default color */
    LC_Red,             /**< Red */
    LC_Yellow,          /**< Yellow */
    LC_Green            /**< Green */
};

/**
 * @ingroup Core
 * @brief Base class of logging
 */
class CBaseLogger
{
    DECLARE_SCRIPT_API()

public:
    /**
     * @brief Constructor
     */
    CBaseLogger() {}

    /**
     * @brief Destructor
     */
    virtual ~CBaseLogger() {}

    /**
     * @brief Initialize logger
     */
    virtual void Init() {}

    /**
     * @ingroup Core
     * @brief Serialize message
     *
     * @param[in] InMessage Message
     * @param[in] InLogType Type of message
     */
    virtual void Serialize( const tchar* InMessage, ELogType InLogType ) {};

    /**
     * @ingroup Core
     * @brief Flush of output device
     */
    virtual void Flush() {};

    /**
     * @ingroup Core
     * @brief Closes output device and cleans up
     *
     * Closes output device and cleans up. This can't happen in the destructor
	 * as we might have to call "delete" which cannot be done for static/ global
	 * objects
     */
    virtual void TearDown() {};

    /**
     * @ingroup Core
     * @brief Print message to output device
     *
     * @param[in] InLogType Type of message
     * @param[in] InMessage Message
     * @param[in] ... Other arguments of message
     */
    void Printf( ELogType InLogType, const tchar* InMessage, ... );

    /**
     * @brief Set color for text in log
     * 
     * @param InLogColor Log color
     */
    virtual void SetTextColor( ELogColor InLogColor ) {}

    /**
     * @brief Reset color text to default
     */
    virtual void ResetTextColor() {}
};

#endif // !BASELOGGER_H
