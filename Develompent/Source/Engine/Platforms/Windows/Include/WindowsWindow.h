/**
 * @file
 * @addtogroup WindowsPlatform Windows platform
 *
 * @authors: Yehor Pohuliaka
 * @date: 3/24/2021
 */

#ifndef WINDOWSWINDOW_H
#define WINDOWSWINDOW_H

#include "Core.h"

/**
 * @ingroup WindowsPlatform
 * @brief Class for working with the application window
 */
class FWindowsWindow
{
public:

    /**
    * @ingroup WindowsPlatform
    * @brief Enumerating window styles
    */
    enum class EStyle : uint8
    {
        None			= 0,                           /**< Without style */
        Resizable 	    = 1 << 0,                      /**< Resizable style */
        Decorated	    = 1 << 1,                      /**< Decorated style */
        Floating		= 1 << 2,                      /**< Floating style */
        Fullscreen	    = 1 << 3,                      /**< Fullscreen style */

        Default         = Decorated | Resizable        /**< The default style is a combination of Decorated and Resizable */
    };

    FWindowsWindow();
    ~FWindowsWindow();

    /**
     * @ingroup WindowsPlatform
     * @brief Handle an event from the queue
     *
     * @return True if queue not empty else false
     */
    bool PollEvent();

    /**
     * @ingroup WindowsPlatform
     * @brief Create window
     *
     * @param InTitle Title of window
     * @param InWidth Width window
     * @param InHeight Height window
     * @param InStyle Style of window
     * @return True if window is opened else false
     */
    bool Create( const tchar* InTitle, int32 InWidth, int32 InHeight, EStyle InStyle = EStyle::Default );

    /**
     * @ingroup WindowsPlatform
     * @brief Close window
     */
    void Close();
};

#endif // !WINDOWSWINDOW_H
