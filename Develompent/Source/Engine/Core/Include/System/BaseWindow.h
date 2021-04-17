/**
 * @file
 * @addtogroup Core Core
 *
 * @authors: Yehor Pohuliaka
 * @date: 4/17/2021
 */

#ifndef BASEWINDOW_H
#define BASEWINDOW_H

#include "Misc/Types.h"

/**
 * @ingroup Core
 * @brief Enumeration of types window styles
 */
enum EStyleWindow
{
	SW_None				= 0,								/**< No styles */
	SW_Resizable		= 1 << 0,							/**< Resizable window */
	SW_Decorated		= 1 << 1,							/**< Decorated window  */
	SW_Floating			= 1 << 2,							/**< Floating window  */
	SW_Fullscreen		= 1 << 3,							/**< Fullscreen window  */

	SW_Default			= SW_Decorated | SW_Resizable		/**< Default style - combined decorated and resizable style  */
};

/**
 * @ingroup Core
 * @brief The base class for work with window
 */
class BaseWindow
{
public:
	/**
	 * @brief Destructor
	 */
	virtual					~BaseWindow() {}

	/**
	 * @brief Create window
	 * 
	 * @param[in] InTitle Title
	 * @param[in] InWidth Width
	 * @param[in] InHeight Height
	 * @param[in] InFlags Combinations flags of EStyleWindow for set style of window
	 */
	virtual void			Create( const tchar* InTitle, uint32 InWidth, uint32 InHeight, uint32 InFlags = SW_Default ) {}

	/**
	 * @brief Close window
	 */
	virtual void			Close() {}
};

#endif // !BASEWINDOW_H
