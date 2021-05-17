/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef BASEVIEWPORTRHI_H
#define BASEVIEWPORTRHI_H

#include "Misc/Types.h"

 /**
  * @ingroup Engine
  * @brief Base class for work with viewport RHI
  */
class FBaseViewportRHI
{
public:
	/**
	 * @brief Destructor
	 */
	virtual								~FBaseViewportRHI() {}

	/**
	 * @brief Presents the swap chain
	 * 
	 * @param[in] InLockToVsync Is it necessary to block for Vsync
	 */
	virtual void						Present( bool InLockToVsync ) {}

	/**
	 * @brief Get width
	 * @return Width of viewport
	 */
	virtual uint32						GetWidth() const { return 0; }

	/**
	 * @brief Get height
	 * @return Height of viewport
	 */
	virtual uint32						GetHeight() const { return 0; }

	/**
	 * @breif Get surface of viewport
	 * @return Pointer to surface of viewport
	 */
	virtual class FBaseSurfaceRHI*		GetSurface() const { return nullptr; }
};

#endif // !BASEVIEWPORTRHI_H
