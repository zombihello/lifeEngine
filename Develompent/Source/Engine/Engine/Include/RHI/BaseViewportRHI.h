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
#include "RHI/BaseRHI.h"

 /**
  * @ingroup Engine
  * @brief Base class for work with viewport RHI
  */
class CBaseViewportRHI : public CBaseResourceRHI
{
public:
	/**
	 * @brief Destructor
	 */
	virtual								~CBaseViewportRHI() {}

	/**
	 * @brief Presents the swap chain
	 * 
	 * @param[in] InLockToVsync Is it necessary to block for Vsync
	 */
	virtual void						Present( bool InLockToVsync ) {}

	/**
	 * Resize viewport
	 * 
	 * @param[in] InWidth New width
	 * @param[in] InHeight New height
	 * @param[in] InIsFullscreen Is fullscreen
	 */
	virtual void						Resize( uint32 InWidth, uint32 InHeight ) {}

	/**
	 * @brief Set surface of viewport
	 * @note Viewport who created with window handle must be ignore this method
	 * 
	 * @param InSurfaceRHI		Surface RHI
	 */
	virtual void SetSurface( SurfaceRHIParamRef_t InSurfaceRHI ) {}

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
	virtual SurfaceRHIRef_t				GetSurface() const { return nullptr; }

	/**
	 * @breif Get window handle
	 * @return Return pointer to window handle
	 */
	virtual WindowHandle_t GetWindowHandle() const { return nullptr; }
};

#endif // !BASEVIEWPORTRHI_H
