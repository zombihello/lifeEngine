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
#include "studiorender/studioapi/dx11/dx11_studiosurface.h"

/*
==================
CStudioSurfaceDx11::CStudioSurfaceDx11
==================
*/
CStudioSurfaceDx11::CStudioSurfaceDx11( ID3D11RenderTargetView* pDx11RenderTargetView )
	: pDx11RenderTargetView( pDx11RenderTargetView )
	, pDx11DepthStencilView( nullptr )
{}

/*
==================
CStudioSurfaceDx11::ClearSurface
==================
*/
void CStudioSurfaceDx11::ClearSurface( const class CColor& color )
{
	ID3D11DeviceContext*		pDx11DeviceContext	= g_StudioAPIDx11.GetDx11DeviceContext();
	float						clearColor[4]		= { color.r / 255.f, color.g / 255.f, color.b / 255.f, color.a / 255.f };
	pDx11DeviceContext->ClearRenderTargetView( pDx11RenderTargetView, clearColor );
}

/*
==================
CStudioSurfaceDx11::ClearDepthStencil
==================
*/
void CStudioSurfaceDx11::ClearDepthStencil( bool bClearDepth /* = true */, bool bClearStencil /* = true */, float depthValue /* = 1.f */, uint8 stencilValue /* = 0 */ )
{
	ID3D11DeviceContext*	pDx11DeviceContext	= g_StudioAPIDx11.GetDx11DeviceContext();
	Assert( pDx11DepthStencilView );

	uint32 clearFlags = 0;
	if ( bClearDepth )
	{
		clearFlags |= D3D11_CLEAR_DEPTH;
	}

	if ( bClearStencil )
	{
		clearFlags |= D3D11_CLEAR_STENCIL;
	}

	pDx11DeviceContext->ClearDepthStencilView( pDx11DepthStencilView, clearFlags, depthValue, stencilValue );
}

/*
==================
CStudioSurfaceDx11::~CStudioSurfaceDx11
==================
*/
CStudioSurfaceDx11::~CStudioSurfaceDx11()
{
	if ( pDx11RenderTargetView )
	{
		pDx11RenderTargetView->Release();
	}

	if ( pDx11DepthStencilView )
	{
		pDx11DepthStencilView->Release();
	}
}