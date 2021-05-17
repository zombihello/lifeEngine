/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef BASERHI_H
#define BASERHI_H

#include "Misc/Types.h"

/**
 * @ingroup Engine
 * @brief Base class of RHI
 */
class FBaseRHI
{
public:
	/**
	 * @brief Destructor
	 */
	virtual										~FBaseRHI() {}

	/**
	 * @brief Initialize RHI
	 * 
	 * @param[in] InIsEditor Is current application editor
	 */
	virtual void								Init( bool InIsEditor )	{}

	/**
	 * @brief Destroy RHI
	 */
	virtual void								Destroy() {}

	/**
	 * @brief Create viewport
	 * 
	 * @param[in] InWindowHandle OS handle on window
	 * @param[in] InWidth Width of viewport
	 * @param[in] InHeight Height of viewport
	 * @return Pointer on viewport
	 */
	virtual class FBaseViewportRHI*				CreateViewport( void* InWindowHandle, uint32 InWidth, uint32 InHeight )	{ return nullptr; }

	/**
	 * @brief Begin drawing viewport
	 * 
	 * @param[in] InDeviceContext Device context
	 * @param[in] InViewport Viewport
	 */
	virtual void								BeginDrawingViewport( class FBaseDeviceContextRHI* InDeviceContext, class FBaseViewportRHI* InViewport ) {}

	/**
	 * @brief End drawing viewport
	 * 
	 * @param[in] InDeviceContext Device context
	 * @param[in] InViewport Viewport
	 * @param[in] InIsPresent Whether to display the frame on the screen
	 * @param[in] InLockToVsync Is it necessary to block for Vsync
	 */
	virtual void								EndDrawingViewport( class FBaseDeviceContextRHI* InDeviceContext, class FBaseViewportRHI* InViewport, bool InIsPresent, bool InLockToVsync ) {}

	/**
	 * @brief Set viewport
	 * 
	 * @param[in] InDeviceContext Device context
	 * @param[in] InMinX Min x
	 * @param[in] InMinY Min y
	 * @param[in] InMinZ Min z
	 * @param[in] InMaxX Max x
	 * @param[in] InMaxY Max y
	 * @param[in] InMaxZ Max z
	 */
	virtual void								SetViewport( class FBaseDeviceContextRHI* InDeviceContext, uint32 InMinX, uint32 InMinY, float InMinZ, uint32 InMaxX, uint32 InMaxY, float InMaxZ ) {}

	/**
	 * @brief Is initialized RHI
	 * @return Return true if RHI is initialized, else false
	 */
	virtual bool								IsInitialize() const { return false; }

	/**
	 * @brief Get device context
	 * @return Pointer to device context
	 */
	virtual class FBaseDeviceContextRHI*		GetImmediateContext() const	{ return nullptr; }
};

#endif // !BASERHI_H
