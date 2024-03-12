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

#ifndef ISTUDIOVIEWPORT_H
#define ISTUDIOVIEWPORT_H

#include "studiorender/studioapi/istudiosurface.h"

/**
 * @ingroup studioapi
 * @brief Studio viewport interface
 */
class IStudioViewport : public IRefCounted
{
public:
	/**
	 * @brief Resize viewport
	 *
	 * @param width			New width
	 * @param height		New height
	 */
	virtual void Resize( uint32 width, uint32 height ) = 0;

	/**
	 * @brief Set surface of viewport
	 * @note Viewport who created with window handle must be ignore this method
	 *
	 * @param pSurface		Studio surface
	 */
	virtual void SetSurface( IStudioSurface* pSurface ) = 0;

	/**
	 * @brief Get width
	 * @return Return viewport width
	 */
	virtual uint32 GetWidth() const = 0;

	/**
	 * @brief Get height
	 * @return Return viewport height
	 */
	virtual uint32 GetHeight() const = 0;

	/**
	 * @breif Get surface of viewport
	 * @return Return pointer to surface of viewport. Returns NULL if this viewport was created with window handle
	 */
	virtual TRefPtr<IStudioSurface> GetSurface() const = 0;

	/**
	 * @breif Get window handle
	 * @return Return pointer to window handle. Returns NULL if this viewport was created with surface
	 */
	virtual windowHandle_t GetWindowHandle() const = 0;
};

#endif // !ISTUDIOVIEWPORT_H