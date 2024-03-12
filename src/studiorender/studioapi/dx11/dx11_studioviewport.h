/**
 * @file
 * @addtogroup studioapi_dx11 studioapi_dx11
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

#ifndef DX11_STUDIOVIEWPORT_H
#define DX11_STUDIOVIEWPORT_H

#include "studiorender/studioapi/istudioviewport.h"
#include "studiorender/studioapi/dx11/dx11_studiosurface.h"

/**
 * @ingroup studioapi_dx11
 * @brief DirectX 11 viewport
 */
class CStudioViewportDx11 : public TRefCounted<IStudioViewport>
{
public:
	/**
	 * @brief Constructor
	 * 
	 * @param windowHandle	OS window handle
	 * @param width			Width of viewport
	 * @param height		Height of viewport
	 */
	CStudioViewportDx11( windowHandle_t windowHandle, uint32 width, uint32 height );

	/**
	 * @brief Constructor
	 *
	 * @param pTargetSurface	Target surface to render
	 * @param width				Width of viewport
	 * @param height			Height of viewport
	 */
	CStudioViewportDx11( CStudioSurfaceDx11* pTargetSurface, uint32 width, uint32 height );

	/**
	 * @brief Destructor
	 */
	~CStudioViewportDx11();

	/**
	 * @brief Presents the swap chain
	 * @param bLockToVsync	Is it necessary to block for Vsync
	 */
	void Present( bool bLockToVsync );

	/**
	 * @brief Resize viewport
	 *
	 * @param width		New width
	 * @param height	New height
	 */
	virtual void Resize( uint32 width, uint32 height ) override;

	/**
	 * @brief Set surface of viewport
	 * @note Viewport who created with window handle must be ignore this method
	 *
	 * @param pSurface		Studio surface
	 */
	virtual void SetSurface( IStudioSurface* pSurface ) override;

	/**
	 * @brief Get width
	 * @return Return viewport width
	 */
	virtual uint32 GetWidth() const override;

	/**
	 * @brief Get height
	 * @return Return viewport height
	 */
	virtual uint32 GetHeight() const override;

	/**
	 * @breif Get surface of viewport
	 * @return Return pointer to surface of viewport. Returns NULL if this viewport was created with window handle
	 */
	virtual TRefPtr<IStudioSurface> GetSurface() const override;

	/**
	 * @breif Get window handle
	 * @return Return pointer to window handle. Returns NULL if this viewport was created with surface
	 */
	virtual windowHandle_t GetWindowHandle() const override;

private:
	windowHandle_t				windowHandle;				/**< Pointer to window handle */
	TRefPtr<CStudioSurfaceDx11>	pBackBuffer;				/**< Pointer to back buffer */
	IDXGISwapChain*				pDxgiSwapChain;				/**< DXGI swap chain */
	uint32						width;						/**< Width of viewport */
	uint32						height;						/**< Height of viewport */
};

#endif // !DX11_STUDIOVIEWPORT_H