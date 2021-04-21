/**
 * @file
 * @addtogroup Engine Engine
 *
 * @authors: Yehor Pohuliaka
 * @date: 4/19/2021
 */

#ifndef BASEVIEWPORTRHI_H
#define BASEVIEWPORTRHI_H

#include "Misc/Types.h"

 /**
  * @ingroup Engine
  * @brief Base class for work with viewport RHI
  */
class BaseViewportRHI
{
public:
	/**
	 * @brief Destructor
	 */
	virtual				~BaseViewportRHI() {}

	/**
	 * @brief Presents the swap chain
	 * 
	 * @param[in] InLockToVsync Is it necessary to block for Vsync
	 */
	virtual void		Present( bool InLockToVsync ) {}

	/**
	 * @brief Get width
	 * @return Width of viewport
	 */
	virtual uint32		GetWidth() const { return 0; }

	/**
	 * @brief Get height
	 * @return Height of viewport
	 */
	virtual uint32		GetHeight() const { return 0; }
};

#endif // !BASEVIEWPORTRHI_H
