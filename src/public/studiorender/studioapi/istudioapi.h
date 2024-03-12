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

#ifndef ISTUDIOAPI_H
#define ISTUDIOAPI_H

#include "stdlib/interface.h"
#include "studiorender/studioapi/istudioviewport.h"

/**
 * @ingroup studioapi
 * @brief Studio API interface version
 */
#define STUDIOAPI_INTERFACE_VERSION "LStudioAPI001"

/**
 * @ingroup studioapi
 * @brief Studio API interface
 */
class IStudioAPI
{
public:
	/**
	 * @brief Initialize Studio API
	 *
	 * @param pFactory	Pointer to interface factory
	 * @return Return TRUE if Studio API is initialized, otherwise returns FALSE
	 */
	virtual bool Init( CreateInterfaceFn_t pFactory ) = 0;

	/**
	 * @brief Shutdown Studio API
	 */
	virtual void Shutdown() = 0;

	/**
	 * @brief Create viewport
	 *
	 * @param windowHandle	OS window handle 
	 * @param width			Viewport width
	 * @param height		Viewport height
	 * @return Return pointer to created viewport
	 */
	virtual TRefPtr<IStudioViewport> CreateViewport( windowHandle_t windowHandle, uint32 width, uint32 height ) const = 0;

	/**
	 * @brief Create viewport
	 *
	 * @param pTargetSurface	Target surface to render
	 * @param width				Viewport width
	 * @param height			Viewport height
	 * @return Return pointer tn created viewport
	 */
	virtual TRefPtr<IStudioViewport> CreateViewport( IStudioSurface* pTargetSurface, uint32 width, uint32 height ) const = 0;

	/**
	 * @brief Begin drawing viewport
	 * @param pViewport		Viewport
	 */
	virtual void BeginDrawingViewport( IStudioViewport* pViewport ) = 0;

	/**
	 * @brief End drawing viewport
	 *
	 * @param pViewport		Viewport
	 * @param bPresent		Whether to display the frame on the screen
	 * @param bLockToVsync	Is it necessary to block for Vsync
	 */
	virtual void EndDrawingViewport( IStudioViewport* pViewport, bool bPresent, bool bLockToVsync ) = 0;

	/**
	 * @brief Set render target
	 *
	 * @param pNewRenderTarget			New render target
	 * @param pNewDepthStencilTarget	New depth stencil target
	 */
	virtual void SetRenderTarget( IStudioSurface* pNewRenderTarget, IStudioSurface* pNewDepthStencilTarget ) = 0;

	/**
	 * @brief Set viewport
	 *
	 * @param minX	Min x
	 * @param minY	Min y
	 * @param minZ	Min z
	 * @param maxX	Max x
	 * @param maxY	Max y
	 * @param maxZ	Max z
	 */
	virtual void SetViewport( uint32 minX, uint32 minY, float minZ, uint32 maxX, uint32 maxY, float maxZ ) = 0;
};

#endif // !ISTUDIOAPI_H