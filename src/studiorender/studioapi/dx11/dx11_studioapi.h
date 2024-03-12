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

#ifndef DX11_STUDIOAPI_H
#define DX11_STUDIOAPI_H

#include "studiorender/studioapi/istudioapi.h"

/**
 * @ingroup studioapi_dx11
 * @brief DirectX 11 Studio API 
 */
class CStudioAPIDx11 : public IStudioAPI
{
public:
	/**
	 * @brief Constructor
	 */
	CStudioAPIDx11();

	/**
	 * @brief Initialize Studio API
	 *
	 * @param pFactory	Pointer to interface factory
	 * @return Return TRUE if Studio API is initialized, otherwise returns FALSE
	 */
	virtual bool Init( CreateInterfaceFn_t pFactory ) override;

	/**
	 * @brief Shutdown Studio API
	 */
	virtual void Shutdown() override;

	/**
	 * @brief Create viewport
	 *
	 * @param windowHandle	OS window handle
	 * @param width			Viewport width
	 * @param height		Viewport height
	 * @return Return pointer to created viewport
	 */
	virtual TRefPtr<IStudioViewport> CreateViewport( windowHandle_t windowHandle, uint32 width, uint32 height ) const override;

	/**
	 * @brief Create viewport
	 *
	 * @param pTargetSurface	Target surface to render
	 * @param width				Viewport width
	 * @param height			Viewport height
	 * @return Return pointer tn created viewport
	 */
	virtual TRefPtr<IStudioViewport> CreateViewport( IStudioSurface* pTargetSurface, uint32 width, uint32 height ) const override;

	/**
	 * @brief Begin drawing viewport
	 * @param pViewport		Viewport
	 */
	virtual void BeginDrawingViewport( IStudioViewport* pViewport ) override;

	/**
	 * @brief End drawing viewport
	 *
	 * @param pViewport		Viewport
	 * @param bPresent		Whether to display the frame on the screen
	 * @param bLockToVsync	Is it necessary to block for Vsync
	 */
	virtual void EndDrawingViewport( IStudioViewport* pViewport, bool bPresent, bool bLockToVsync ) override;

	/**
	 * @brief Set render target
	 *
	 * @param pNewRenderTarget			New render target
	 * @param pNewDepthStencilTarget	New depth stencil target
	 */
	virtual void SetRenderTarget( IStudioSurface* pNewRenderTarget, IStudioSurface* pNewDepthStencilTarget ) override;

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
	virtual void SetViewport( uint32 minX, uint32 minY, float minZ, uint32 maxX, uint32 maxY, float maxZ ) override;

	/**
	 * @brief Get DirectX 11 device
	 * @return Return DirectX 11 device
	 */
	FORCEINLINE ID3D11Device* GetDx11Device() const
	{
		return pDx11Device;
	}

	/**
	 * @brief Get DirectX 11 device context
	 * @return Return DirectX 11 device context
	 */
	FORCEINLINE ID3D11DeviceContext* GetDx11DeviceContext() const
	{
		return pDx11DeviceContext;
	}

	/**
	 * @brief Get DXGI factory
	 * @return Return DXGI factory
	 */
	FORCEINLINE IDXGIFactory* GetDXGIFactory() const
	{
		return pDxgiFactory;
	}

	/**
	 * @brief Get DXGI adapter
	 * @return Return DXGI adapter
	 */
	FORCEINLINE IDXGIAdapter* GetDXGIAdapter() const
	{
		return pDxgiAdapter;
	}

private:
	bool				bInited;				/**< Is initialized Studio API */
	D3D_FEATURE_LEVEL		dxFeatureLevel;		/**< DirectX feature level */
	ID3D11Device*			pDx11Device;		/**< DirectX 11 Device */
	ID3D11DeviceContext*	pDx11DeviceContext;	/**< DirectX 11 device context */
	IDXGIFactory*			pDxgiFactory;		/**< DXGI factory */
	IDXGIAdapter*			pDxgiAdapter;		/**< DXGI adapter */
};

/**
 * @ingroup studioapi_dx11
 * @brief DirectX 11 Studio API
 */
extern CStudioAPIDx11		g_StudioAPIDx11;

/**
 * @ingroup studioapi_dx11
 * @brief Set debug name for a DirectX 11 resource
 *
 * @param pObject	DirectX 11 object
 * @param pName		Resource debug name
 */
void DX11_SetDebugName( ID3D11DeviceChild* pObject, const achar* pName );

/**
 * @ingroup studioapi_dx11
 * @brief Set debug name for a DirectX 11 resource
 *
 * @param pObject	DirectX 11 object
 * @param pName		Resource debug name
 */
void DX11_SetDebugName( IDXGIObject* pObject, const achar* pName );

#endif // !DX11_STUDIOAPI_H