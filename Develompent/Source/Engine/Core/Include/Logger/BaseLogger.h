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
 * @brief Enumerating of log categories
 */
enum ELogCategory
{
    LC_None,                /**< No category */
    LC_General,             /**< General category */
    LC_Init,                /**< Initialize category  */
    LC_Script,              /**< Log from script */
    LC_Dev,                 /**< Development category */
    LC_Shader,              /**< Shader category */
    LC_Input,               /**< Input category */
    LC_Package,             /**< Package category */
    LC_Audio,               /**< Audio category */
    LC_Physics,             /**< Physics category */

#if WITH_EDITOR
    LC_Editor,              /**< Editor category */
    LC_Commandlet           /**< Commandlet category */
#endif // WITH_EDITOR
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
class ÑBaseLogger
{
    DECLARE_SCRIPT_API()

public:
    /**
     * @brief Constructor
     */
                        ÑBaseLogger() {}

    /**
     * @brief Destructor
     */
    virtual             ~ÑBaseLogger() {}

    /**
     * @brief Initialize logger
     */
    virtual void        Init() {}

    /**
     * @ingroup Core
     * @brief Serialize message
     *
     * @param[in] InMessage Message
     * @param[in] InLogType Type of message
     * @param[in] InLogCategory Log category
     */
    virtual void        Serialize( const tchar* InMessage, ELogType InLogType, ELogCategory InLogCategory ) {};

    /**
     * @ingroup Core
     * @brief Flush of output device
     */
    virtual void        Flush() {};

    /**
     * @ingroup Core
     * @brief Closes output device and cleans up
     *
     * Closes output device and cleans up. This can't happen in the destructor
	 * as we might have to call "delete" which cannot be done for static/ global
	 * objects
     */
    virtual void        TearDown() {};

    /**
     * @ingroup Core
     * @brief Print message to output device
     *
     * @param[in] InLogType Type of message
     * @param[in] InLogCategory Log category 
     * @param[in] InMessage Message
     * @param[in] ... Other arguments of message
     */
    void                Logf( ELogType InLogType, ELogCategory InLogCategory, const tchar* InMessage, ... );

    /**
     * @brief Set color for text in log
     * 
     * @param InLogColor Log color
     */
    virtual void        SetTextColor( ELogColor InLogColor ) {}

    /**
     * @brief Reset color text to default
     */
    virtual void        ResetTextColor() {}
};

#endif // !BASELOGGER_H
