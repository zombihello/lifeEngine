/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef BASEDEVICECONTEXTRHI_H
#define BASEDEVICECONTEXTRHI_H

#include "RHI/BaseRHI.h"

/**
 * @ingroup Engine
 * @brief Base class for work with device context RHI
 */
class FBaseDeviceContextRHI : public FBaseResourceRHI
{
public:
	/**
	 * @brief Destructor
	 */
	virtual						~FBaseDeviceContextRHI() {}

	/**
	 * @brief Clear surface
	 * 
	 * @param[in] InSurface Surface for rendering
	 * @param[in] InColor Color for clearing render target
	 */
	virtual void				ClearSurface( FSurfaceRHIParamRef InSurface, const class FColor& InColor ) {}

	/**
	 * Clear depth stencil
	 *
	 * @param[in] InSurface Surface for clear
	 * @param[in] InIsClearDepth Is need clear depth buffer
	 * @param[in] InIsClearStencil Is need clear stencil buffer
	 * @param[in] InDepthValue Clear the depth buffer with this value
	 * @param[in] InStencilValue Clear the stencil buffer with this value
	 */
	virtual void				ClearDepthStencil( FSurfaceRHIParamRef InSurface, bool InIsClearDepth = true, bool InIsClearStencil = true, float InDepthValue = 1.f, uint8 InStencilValue = 0 ) {}
};

#endif // !BASEDEVICECONTEXTRHI_H
