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

#include "Core/Core.h"
#include "Core/Math/Color.h"
#include "RHI/D3D11/D3D11DeviceContext.h"
#include "RHI/D3D11/D3D11Surface.h"

/*
==================
CD3D11DeviceContext::CD3D11DeviceContext
==================
*/
CD3D11DeviceContext::CD3D11DeviceContext( ID3D11DeviceContext* InD3D11DeviceContext ) :
	d3d11DeviceContext( InD3D11DeviceContext )
{}

/*
==================
CD3D11DeviceContext::~CD3D11DeviceContext
==================
*/
CD3D11DeviceContext::~CD3D11DeviceContext()
{
	d3d11DeviceContext->Release();
	d3d11DeviceContext = nullptr;
}

/*
==================
CD3D11DeviceContext::ClearSurface
==================
*/
void CD3D11DeviceContext::ClearSurface( SurfaceRHIParamRef_t InSurface, const class CColor& InColor )
{
	Assert( d3d11DeviceContext && InSurface );
	CD3D11Surface*			d3d11Surface = ( CD3D11Surface* )InSurface;

	float		clearColor[ 4 ] = { InColor.r / 255.f, InColor.g / 255.f, InColor.b / 255.f, InColor.a / 255.f };
	d3d11DeviceContext->ClearRenderTargetView( d3d11Surface->GetRenderTargetView(), clearColor );
}

/*
==================
CD3D11DeviceContext::ClearDepthStencil
==================
*/
void CD3D11DeviceContext::ClearDepthStencil( SurfaceRHIParamRef_t InSurface, bool InIsClearDepth /* = true */, bool InIsClearStencil /* = true */, float InDepthValue /* = 1.f */, uint8 InStencilValue /* = 0 */ )
{
	Assert( d3d11DeviceContext && InSurface );
	ID3D11DepthStencilView*			d3d11DepthStencilView = ( ( CD3D11Surface* )InSurface )->GetDepthStencilView();
	Assert( d3d11DepthStencilView );

	uint32			clearFlags = 0;
	if ( InIsClearDepth )
	{
		clearFlags |= D3D11_CLEAR_DEPTH;
	}
	
	if ( InIsClearStencil )
	{
		clearFlags |= D3D11_CLEAR_STENCIL;
	}

	d3d11DeviceContext->ClearDepthStencilView( d3d11DepthStencilView, clearFlags, InDepthValue, InStencilValue );
}