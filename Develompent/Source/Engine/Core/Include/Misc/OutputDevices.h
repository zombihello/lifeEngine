/**
 * @file
 * @addtogroup Core Core
 *
 * @authors: Yehor Pohuliaka
 * @date: 3/25/2021
 */

#ifndef OUTPUTDEVICE_H
#define OUTPUTDEVICE_H

#include "Core.h"

/**
 * @ingroup Core
 * @brief Base class of output device
 */
class OutputDevice
{
public:

    /**
    * @ingroup Core
    * @brief Enumerating of event types
    */
    enum class EEventType : uint8
    {
        Log,            /**< Normal message display event */
        Warning,        /**< Displaying a warning */
        Critical,       /**< Critical error output */
        Init            /**< Displaying the initialization message */
    };

    OutputDevice() {}
    virtual ~OutputDevice() {}

    /**
     * @ingroup Core
     * @brief Serialize message
     *
     * @param[in] InMessage Message
     * @param[in] InEvent Type event of message
     */
    virtual void Serialize( const tchar* InMessage, EEventType InEvent ) {};

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
     * @param[in] InMessage Message
     * @param[in] ... Other arguments of message
     */
    void Logf( const tchar* InMessage, ... )
    {
        Logf( EEventType::Log, InMessage );
    }

    /**
     * @ingroup Core
     * @brief Print message to output device
     *
     * @param[in] InEvent Type event of message
     * @param[in] InMessage Message
     * @param[in] ... Other arguments of message
     */
    void Logf( EEventType InEvent, const tchar* InMessage, ... );
};

#endif // !OUTPUTDEVICE_H
