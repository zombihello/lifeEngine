/**
 * @file
 * @addtogroup Engine Engine
 *
 * ************************************************************
 *                  This file is part of:
 *                      LIFEENGINE
 *          https://github.com/zombihello/lifeEngine
 * ************************************************************
 * Copyright (C) 2024 Yehor Pohuliaka.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef BASEDEVICECONTEXTRHI_H
#define BASEDEVICECONTEXTRHI_H

#include "RHI/BaseRHI.h"

/**
 * @ingroup Engine
 * @brief Base class for work with device context RHI
 */
class CBaseDeviceContextRHI : public CBaseResourceRHI
{
public:
	/**
	 * @brief Destructor
	 */
	virtual ~CBaseDeviceContextRHI() {}

	/**
	 * @brief Clear surface
	 * 
	 * @param[in] InSurface Surface for rendering
	 * @param[in] InColor Color for clearing render target
	 */
	virtual void ClearSurface( SurfaceRHIParamRef_t InSurface, const class CColor& InColor ) {}

	/**
	 * Clear depth stencil
	 *
	 * @param[in] InSurface Surface for clear
	 * @param[in] InIsClearDepth Is need clear depth buffer
	 * @param[in] InIsClearStencil Is need clear stencil buffer
	 * @param[in] InDepthValue Clear the depth buffer with this value
	 * @param[in] InStencilValue Clear the stencil buffer with this value
	 */
	virtual void ClearDepthStencil( SurfaceRHIParamRef_t InSurface, bool InIsClearDepth = true, bool InIsClearStencil = true, float InDepthValue = 1.f, uint8 InStencilValue = 0 ) {}
};

#endif // !BASEDEVICECONTEXTRHI_H
