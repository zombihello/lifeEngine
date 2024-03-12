/**
 * @file
 * @addtogroup studioapi studioapi
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

#ifndef ISTUDIOSURFACE_H
#define ISTUDIOSURFACE_H

#include "stdlib/refcount.h"
#include "stdlib/math/color.h"

/**
 * @ingroup studioapi
 * @brief Studio surface interface
 */
class IStudioSurface : public IRefCounted
{
public:
	/**
	 * @brief Clear surface
	 * @param color		Color for clearing render target
	 */
	virtual void ClearSurface( const CColor& color ) = 0;

	/**
	 * @brief Clear depth stencil
	 * This method clear depth stencil surface. If it isn't the depth stencil must be ignored
	 *
	 * @param bClearDepth		Is need clear depth buffer
	 * @param bClearStencil		Is need clear stencil buffer
	 * @param depthValue		Clear the depth buffer with this value
	 * @param stencilValue		Clear the stencil buffer with this value
	 */
	virtual void ClearDepthStencil( bool bClearDepth = true, bool bClearStencil = true, float depthValue = 1.f, uint8 stencilValue = 0 ) = 0;
};

#endif // !ISTUDIOSURFACE_H