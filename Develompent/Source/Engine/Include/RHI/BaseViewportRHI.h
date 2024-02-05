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

#ifndef BASEVIEWPORTRHI_H
#define BASEVIEWPORTRHI_H

#include "Core/Misc/Types.h"
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
	virtual ~CBaseViewportRHI() {}

	/**
	 * @brief Presents the swap chain
	 * 
	 * @param[in] InLockToVsync Is it necessary to block for Vsync
	 */
	virtual void Present( bool InLockToVsync ) {}

	/**
	 * Resize viewport
	 * 
	 * @param[in] InWidth New width
	 * @param[in] InHeight New height
	 * @param[in] InIsFullscreen Is fullscreen
	 */
	virtual void Resize( uint32 InWidth, uint32 InHeight ) {}

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
	virtual uint32 GetWidth() const { return 0; }

	/**
	 * @brief Get height
	 * @return Height of viewport
	 */
	virtual uint32 GetHeight() const { return 0; }

	/**
	 * @breif Get surface of viewport
	 * @return Pointer to surface of viewport
	 */
	virtual SurfaceRHIRef_t GetSurface() const { return nullptr; }

	/**
	 * @breif Get window handle
	 * @return Return pointer to window handle
	 */
	virtual WindowHandle_t GetWindowHandle() const { return nullptr; }
};

#endif // !BASEVIEWPORTRHI_H
