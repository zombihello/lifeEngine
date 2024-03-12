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

#ifndef DX11_STUDIOSURFACE_H
#define DX11_STUDIOSURFACE_H

#include <d3d11.h>
#include "studiorender/studioapi/istudiosurface.h"

/**
 * @ingroup studioapi_dx11
 * @brief DirectX 11 surface
 */
class CStudioSurfaceDx11 : public TRefCounted<IStudioSurface>
{
public:
	/**
	 * @brief Constructor
	 * @param pDx11RenderTargetView		DirectX 11 Render target view
	 */
	CStudioSurfaceDx11( ID3D11RenderTargetView* pDx11RenderTargetView );

	/**
	 * @brief Destructor
	 */
	~CStudioSurfaceDx11();

	/**
	 * @brief Clear surface
	 * @param color		Color for clearing render target
	 */
	virtual void ClearSurface( const class CColor& color ) override;

	/**
	 * @brief Clear depth stencil
	 * This method clear depth stencil surface. If it isn't the depth stencil must be ignored
	 *
	 * @param bClearDepth		Is need clear depth buffer
	 * @param bClearStencil		Is need clear stencil buffer
	 * @param depthValue		Clear the depth buffer with this value
	 * @param stencilValue		Clear the stencil buffer with this value
	 */
	virtual void ClearDepthStencil( bool bClearDepth = true, bool bClearStencil = true, float depthValue = 1.f, uint8 stencilValue = 0 ) override;

	/**
	 * @brief Get render target view
	 * @return Return DirectX 11 render target view
	 */
	FORCEINLINE ID3D11RenderTargetView*	GetDx11RenderTargetView() const
	{
		return pDx11RenderTargetView;
	}

	/**
	 * @brief Get depth stencil view
	 * @return Return DirectX 11 depth stencil view
	 */
	FORCEINLINE ID3D11DepthStencilView* GetDx11DepthStencilView() const
	{
		return pDx11DepthStencilView;
	}

private:
	ID3D11RenderTargetView*		pDx11RenderTargetView;		/**< The view that is used to render to the texture */
	ID3D11DepthStencilView*		pDx11DepthStencilView;		/**< The view that is used to render to the texture for depth format textures */
};

#endif // !DX11_STUDIOSURFACE_H