/**
 * @file
 * @addtogroup WindowsPlatform Windows platform
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef WINDOWSWINDOW_H
#define WINDOWSWINDOW_H

#include "Core.h"
#include "System/BaseWindow.h"

struct SDL_Window;
struct SDL_SysWMinfo;

/**
 * @ingroup WindowsPlatform
 * @brief Convert scancode to engine keycode
 * 
 * @param[in] InScancode SDL scancode
 * @return Return engine keycode
 */
EButtonCode appScanCodeToButtonCode( uint32 InScancode );

/**
 * @ingroup WindowsPlatform
 * @brief Convert engine keycode scancode
 * 
 * @param[in] InButtonCode Engine keycode
 * @return Return SDL scancode
 */
uint32 appButtonCodeToScanCode( EButtonCode InButtonCode );

/**
 * @ingroup WindowsPlatform
 * @brief The class for work with window on Windows
 */
class CWindowsWindow : public CBaseWindow
{
public:
	/**
	 * @brief Constructor
	 */
									CWindowsWindow();

	/**
	 * @brief Destructor
	 */
									~CWindowsWindow();

	/**
	 * @brief Create window
	 *
	 * @param[in] InTitle Title
	 * @param[in] InWidth Width
	 * @param[in] InHeight Height
	 * @param[in] InFlags Combinations flags of EStyleWindow for set style of window
	 * @return True if window created successed, else false
	 */
	virtual void					Create( const tchar* InTitle, uint32 InWidth, uint32 InHeight, uint32 InFlags = SW_Default ) override;

	/**
	 * @brief Close window
	 */
	virtual void					Close() override;

	/**
	 * @brief Show cursor
	 */
	virtual void					ShowCursor() override;

	/**
	 * @brief Hide cursor 
	 */
	virtual void					HideCursor() override;

	/**
	 * @brief Show window
	 */
	virtual void					Show() override;

	/**
	 * @brief Hide window
	 */
	virtual void					Hide() override;

	/**
	 * @brief Handle window event
	 *
	 * @param[out] OutWindowEvent Occurred window event
	 * @return True if queue of events not empty, else false
	 */
	virtual bool					PollEvent( SWindowEvent& OutWindowEvent ) override;

	/**
	 * @brief Set title of window
	 * 
	 * @param[in] InTitle New title
	 */
	virtual void					SetTitle( const tchar* InTitle ) override;

	/**
	 * @brief Set size of window
	 * 
	 * @param[in] InWidth Width
	 * @param[in] InHeight Height
	 */
	virtual void					SetSize( uint32 InWidth, uint32 InHeight ) override;

	/**
	 * @brief Set fullscreen mode
	 * 
	 * @param[in] InIsFullscreen Enable fullscreen mode or disable
	 */
	virtual void					SetFullscreen( bool InIsFullscreen ) override;

	/**
	 * @brief Is window open
	 * @return True if window oppened, else false
	 */
	virtual bool					IsOpen() const override;

	/**
	 * @brief Is showing cursor
	 * @return True cursor is showing, else false
	 */
	virtual bool					IsShowingCursor() const override;

	/**
	 * @brief Is enabled fullscreen mode
	 * @return True fullscreen is enabled, else false
	 */
	virtual bool					IsFullscreen() const override;

	/**
	 * @brief Get size window
	 * 
	 * @param[out] OutWidth Width
	 * @oaram[out] OutHeight Height
	 */
	virtual void					GetSize( uint32& OutWidth, uint32& OutHeight ) const override;

	/**
	 * @brief Get OS handle
	 * @return Return OS handle
	 */
	virtual void*					GetHandle() const override;

	/**
	 * @brief Get ID window
	 * @return Return ID window. If window not created return ( uint32 )-1
	 */
	virtual uint32					GetID() const override;

	/**
	 * @brief Get SDL window handle
	 * @return Return SDL window handle
	 */
	FORCEINLINE SDL_Window*			GetSDLWindow() const		{ return sdlWindow; }

private:
	bool					isShowCursor;			/**< Is showed cursor */
	bool					isFullscreen;			/**< Is enabled fullscreen mode */

	uint32					id;						/**< Id window */
	SDL_Window*				sdlWindow;				/**< Pointer on SDL window */
	SDL_SysWMinfo*			sdlWindowInfo;			/**< Pointer on system info from SDL */
	HANDLE					handle;					/**< OS handle on window */
};

#endif // !WINDOWSWINDOW_H
