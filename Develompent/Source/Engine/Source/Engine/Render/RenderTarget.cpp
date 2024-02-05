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

#include "Engine/Misc/EngineGlobals.h"
#include "Engine/Render/RenderTarget.h"
#include "RHI/BaseRHI.h"

/*
==================
CRenderTarget::CRenderTarget
==================
*/
CRenderTarget::CRenderTarget()
	: bDirty( false )
	, flags( 0 )
	, sizeX( 0 )
	, sizeY( 0 )
	, pixelFormat( PF_Unknown )
{}

/*
==================
CRenderTarget::InitRHI
==================
*/
void CRenderTarget::InitRHI()
{
#if !SHIPPING_BUILD
	const tchar* cDebugName = debugName.c_str();
#else
	const tchar* cDebugName = TEXT( "" );
#endif // !SHIPPING_BUILD

	Assert( sizeX > 0 && sizeY > 0 && pixelFormat != PF_Unknown );
	texture		= g_RHI->CreateTexture2D( cDebugName, sizeX, sizeY, pixelFormat, 1, flags );
	surface		= g_RHI->CreateTargetableSurface( cDebugName, sizeX, sizeY, pixelFormat, texture, flags );
	bDirty		= false;
}

/*
==================
CRenderTarget::UpdateRHI
==================
*/
void CRenderTarget::UpdateRHI()
{
	InitRHI();
}

/*
==================
CRenderTarget::ReleaseRHI
==================
*/
void CRenderTarget::ReleaseRHI()
{
	surface.SafeRelease();
	texture.SafeRelease();
	bDirty = false;
}

/*
==================
CRenderTarget::Update
==================
*/
void CRenderTarget::Update( bool InIsDestroyed, uint32 InNewSizeX, uint32 InNewSizeY, EPixelFormat InPixelFormat, uint32 InFlags /* = 0 */, const std::wstring& InName /* = TEXT( "" ) */ )
{
	bool			bNeedUpdateRHI	= pixelFormat != InPixelFormat || sizeX != InNewSizeX || sizeY != InNewSizeY || flags != (TCF_ResolveTargetable | InFlags);
	bool			bIsValidSize	= ( int32 )InNewSizeX > 0 && ( int32 )InNewSizeY > 0;

	// Do nothing if attributes is equal or not valid size and don't need destroy him
	if ( !bNeedUpdateRHI && !InIsDestroyed && !bIsValidSize )
	{
		return;
	}

	// Update the render target attributes
	flags			= TCF_ResolveTargetable | InFlags;
	pixelFormat		= InPixelFormat;
	sizeX			= InNewSizeX;
	sizeY			= InNewSizeY;
	bDirty			= InIsDestroyed || bNeedUpdateRHI;
#if !SHIPPING_BUILD
	if ( !InName.empty() )
	{
		debugName = InName;
	}
#endif // !SHIPPING_BUILD

	// Don't reinitialize the render target RHI if him has been destroyed
	if ( InIsDestroyed )
	{
		BeginReleaseResource( this );
		if ( InIsDestroyed )
		{
			return;
		}
	}

	// Initialize the render resource if need
	if ( bNeedUpdateRHI )
	{
		BeginUpdateResource( this );
	}
}