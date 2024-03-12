/**
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

#include "pch_studioapi.h"
#include "studiorender/studioapi/dx11/dx11_studioviewport.h"

/*
==================
DX11_GetSwapChainSurface
==================
*/
static CStudioSurfaceDx11* DX11_GetSwapChainSurface( IDXGISwapChain* pSwapChain )
{
	Assert( pSwapChain );

	// Grab the back buffer
	ID3D11Texture2D*	pBackBuffer = nullptr;
	HRESULT				result		= pSwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), ( void** )&pBackBuffer );
	if ( result != S_OK )
	{
		Assert( false );
		return nullptr;
	}

	// Create the render target view
	ID3D11RenderTargetView*			pBackBufferRenderTargetView = nullptr;
	D3D11_RENDER_TARGET_VIEW_DESC	rtvDesc;
	rtvDesc.Format					= DXGI_FORMAT_R8G8B8A8_UNORM;
	rtvDesc.ViewDimension			= D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice		= 0;

	result = g_StudioAPIDx11.GetDx11Device()->CreateRenderTargetView( pBackBuffer, &rtvDesc, &pBackBufferRenderTargetView );
	if ( result != S_OK )
	{
		Assert( false );
		return nullptr;
	}

	// Set debug name for back buffer and return our surface
	DX11_SetDebugName( pBackBuffer, "SwapChainBackBuffer" );
	DX11_SetDebugName( pBackBufferRenderTargetView, "BackBuffer" );
	pBackBuffer->Release();
	return new CStudioSurfaceDx11( pBackBufferRenderTargetView );
}


/*
==================
CStudioViewportDx11::CStudioViewportDx11
==================
*/
CStudioViewportDx11::CStudioViewportDx11( windowHandle_t windowHandle, uint32 width, uint32 height )
	: windowHandle( windowHandle )
	, pBackBuffer( nullptr )
	, pDxgiSwapChain( nullptr )
	, width( width )
	, height( height )
{
	IDXGIFactory*	pDxgiFactory = g_StudioAPIDx11.GetDXGIFactory();

	// Create the swapchain
	DXGI_SWAP_CHAIN_DESC			swapChainDesc;
	Mem_Memzero( &swapChainDesc, sizeof( DXGI_SWAP_CHAIN_DESC ) );
	swapChainDesc.BufferCount							= 1;
	swapChainDesc.BufferDesc.Width						= width;
	swapChainDesc.BufferDesc.Height						= height;
	swapChainDesc.BufferDesc.Format						= DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.RefreshRate.Numerator		= 75;
	swapChainDesc.BufferDesc.RefreshRate.Denominator	= 1;
	swapChainDesc.BufferUsage							= DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow							= ( HWND )windowHandle;
	swapChainDesc.SampleDesc.Count						= 1;
	swapChainDesc.SampleDesc.Quality					= 0;
	swapChainDesc.Windowed								= true;

	HRESULT		result = pDxgiFactory->CreateSwapChain( g_StudioAPIDx11.GetDx11Device(), &swapChainDesc, &pDxgiSwapChain );
	if ( result != S_OK )
	{
		Sys_Error( "CStudioAPIDx11: Failed to create studio viewport" );
	}

	// Make window association with out swap chain
	DX11_SetDebugName( pDxgiSwapChain, "SwapChain" );
	result = pDxgiFactory->MakeWindowAssociation( ( HWND )windowHandle, DXGI_MWA_NO_WINDOW_CHANGES );
	if ( result != S_OK )
	{
		Sys_Error( "CStudioAPIDx11: Failed to make window association with studio viewport" );
	}

	// Create a studio surface to represent the viewport's back buffer
	pBackBuffer = DX11_GetSwapChainSurface( pDxgiSwapChain );
}

/*
==================
CStudioViewportDx11::CStudioViewportDx11
==================
*/
CStudioViewportDx11::CStudioViewportDx11( CStudioSurfaceDx11* pTargetSurface, uint32 width, uint32 height )
	: windowHandle( nullptr )
	, pBackBuffer( pTargetSurface )
	, pDxgiSwapChain( nullptr )
	, width( width )
	, height( height )
{}

/*
==================
CStudioViewportDx11::~CStudioViewportDx11
==================
*/
CStudioViewportDx11::~CStudioViewportDx11()
{
	pBackBuffer.SafeRelease();
	if ( pDxgiSwapChain )
	{
		pDxgiSwapChain->Release();
	}
}

/*
==================
CStudioViewportDx11::Present
==================
*/
void CStudioViewportDx11::Present( bool bLockToVsync )
{
	if ( pDxgiSwapChain )
	{
		pDxgiSwapChain->Present( bLockToVsync ? 1 : 0, 0 );
	}
}

/*
==================
CStudioViewportDx11::Resize
==================
*/
void CStudioViewportDx11::Resize( uint32 width, uint32 height )
{
	if ( CStudioViewportDx11::width == width && CStudioViewportDx11::height == height )
	{
		return;
	}
	CStudioViewportDx11::width	= width;
	CStudioViewportDx11::height	= height;

	if ( pDxgiSwapChain )
	{
		// Release our backbuffer reference, as required by DXGI before calling ResizeBuffers
		pBackBuffer.SafeRelease();
		HRESULT			result = pDxgiSwapChain->ResizeBuffers( 1, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH );
		if ( result != S_OK )
		{
			Sys_Error( "CStudioAPIDx11: Failed to resize studio viewport to %ix%i", width, height );
		}

		// Get a new swapchain surface
		pBackBuffer = DX11_GetSwapChainSurface( pDxgiSwapChain );
	}
}

/*
==================
CStudioViewportDx11::SetSurface
==================
*/
void CStudioViewportDx11::SetSurface( IStudioSurface* pSurface )
{
	if ( !pDxgiSwapChain )
	{
		pBackBuffer = ( CStudioSurfaceDx11* )pSurface;
	}
}

/*
==================
CStudioViewportDx11::GetWidth
==================
*/
uint32 CStudioViewportDx11::GetWidth() const
{
	return width;
}

/*
==================
CStudioViewportDx11::GetHeight
==================
*/
uint32 CStudioViewportDx11::GetHeight() const
{
	return height;
}

/*
==================
CStudioViewportDx11::GetSurface
==================
*/
TRefPtr<IStudioSurface> CStudioViewportDx11::GetSurface() const
{
	return pBackBuffer;
}

/*
==================
CStudioViewportDx11::GetWindowHandle
==================
*/
windowHandle_t CStudioViewportDx11::GetWindowHandle() const
{
	return windowHandle;
}