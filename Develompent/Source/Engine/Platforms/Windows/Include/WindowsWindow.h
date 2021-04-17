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
#include "System/BaseWindow.h"

struct SDL_Window;
struct SDL_SysWMinfo;

/**
 * @ingroup WindowsPlatform
 * @brief The class for work with window on Windows
 */
class WindowsWindow : public BaseWindow
{
public:
	/**
	 * @brief Constructor
	 */
					WindowsWindow();

	/**
	 * @brief Destructor
	 */
					~WindowsWindow();

	/**
	 * @brief Create window
	 *
	 * @param[in] InTitle Title
	 * @param[in] InWidth Width
	 * @param[in] InHeight Height
	 * @param[in] InFlags Combinations flags of EStyleWindow for set style of window
	 * @return True if window created successed, else false
	 */
	void			Create( const tchar* InTitle, uint32 InWidth, uint32 InHeight, uint32 InFlags = SW_Default ) override;

	/**
	 * @brief Close window
	 */
	void			Close() override;

private:
	bool					isFullscreen;

	SDL_Window*				sdlWindow;
	SDL_SysWMinfo*			sdlWindowInfo;
	HANDLE					handle;
};

#endif // !WINDOWSWINDOW_H
