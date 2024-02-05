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

#include "RHI/BaseRHI.h"
#include "RHI/BaseDeviceContextRHI.h"
#include "RHI/BaseViewportRHI.h"
#include "Engine/Misc/EngineGlobals.h"
#include "Engine/Render/RenderingThread.h"
#include "Engine/Render/Viewport.h"
#include "Engine/Render/SceneRenderTargets.h"
#include "Engine/Render/Scene.h"

/*
==================
CViewport::CViewport
==================
*/
CViewport::CViewport() 
	: windowHandle( nullptr )
	, viewportClient( nullptr )
	, sizeX( 0 )
	, sizeY( 0 )
{}

/*
==================
CViewport::~CViewport
==================
*/
CViewport::~CViewport()
{}

/*
==================
CViewport::InitRHI
==================
*/
void CViewport::InitRHI()
{
	// If viewport already created - we need resize
	if ( viewportRHI )
	{
		viewportRHI->Resize( sizeX, sizeY );
		if ( renderTarget )
		{
			Assert( renderTarget->GetSurfaceRHI() );
			viewportRHI->SetSurface( renderTarget->GetSurfaceRHI() );
		}
	}
	// Else we need create it
	else
	{
		if ( windowHandle )
		{
			viewportRHI = g_RHI->CreateViewport( windowHandle, sizeX, sizeY );
		}
		else if ( renderTarget )
		{
			Assert( renderTarget->GetSurfaceRHI() );
			viewportRHI = g_RHI->CreateViewport( renderTarget->GetSurfaceRHI(), sizeX, sizeY );
		}
	}
}

/*
==================
CViewport::UpdateRHI
==================
*/
void CViewport::UpdateRHI()
{
	InitRHI();
}

/*
==================
CViewport::ReleaseRHI
==================
*/
void CViewport::ReleaseRHI()
{
	// Release RHI if him is not have parent
	viewportRHI.SafeRelease();
}

/*
==================
CViewport::Update
==================
*/
void CViewport::Update( bool InIsDestroyed, uint32 InNewSizeX, uint32 InNewSizeY, CRenderTarget* InNewRenderTarget )
{
	bool			bNewRenderTarget = renderTarget != InNewRenderTarget;

	// Update the viewport attributes
	windowHandle	= nullptr;
	renderTarget	= InNewRenderTarget;

	UpdateInternal( InIsDestroyed, InNewSizeX, InNewSizeY, bNewRenderTarget );
}

/*
==================
CViewport::Update
==================
*/
void CViewport::Update( bool InIsDestroyed, uint32 InNewSizeX, uint32 InNewSizeY, WindowHandle_t InNewWindowHandle )
{
	bool		bNewWindowHandle = windowHandle != InNewWindowHandle;

	// Update the viewport attributes
	renderTarget.SafeRelease();
	windowHandle = InNewWindowHandle;

	UpdateInternal( InIsDestroyed, InNewSizeX, InNewSizeY, bNewWindowHandle );
}

/*
==================
CViewport::UpdateInternal
==================
*/
void CViewport::UpdateInternal( bool InIsDestroyed, uint32 InNewSizeX, uint32 InNewSizeY, bool InIsNewHandle )
{
	AssertMsg( InIsDestroyed || ( windowHandle || renderTarget ) && !( windowHandle && renderTarget ), TEXT( "Must be stated only one" ) );
	bool		bNeedUpdateViewportRHI = InIsNewHandle || sizeX != InNewSizeX || sizeY != InNewSizeY;

	// Update the viewport attributes
	sizeX = InNewSizeX;
	sizeY = InNewSizeY;

	// Don't reinitialize the viewport RHI if the viewport has been destroyed
	if ( InIsDestroyed )
	{
		BeginReleaseResource( this );
		if ( InIsDestroyed )
		{
			return;
		}
	}

	// Update the viewport's resources if it need
	if ( bNeedUpdateViewportRHI )
	{
		BeginUpdateResource( this );
	}
}

/*
==================
CViewport::Tick
==================
*/
void CViewport::Tick( float InDeltaSeconds )
{
	if ( viewportClient )
	{
		viewportClient->Tick( InDeltaSeconds );
	}
}

/*
==================
CViewport::Draw
==================
*/
void CViewport::Draw( bool InIsShouldPresent /* = true */ )
{
	if ( !IsInitialized() || !IsValid() )
	{
		return;
	}

	// Begin drawing viewport
	UNIQUE_RENDER_COMMAND_ONEPARAMETER( CBeginRenderCommand,
										ViewportRHIRef_t, viewportRHI, viewportRHI,
										{
											CBaseDeviceContextRHI*		immediateContext = g_RHI->GetImmediateContext();
											g_RHI->BeginDrawingViewport( immediateContext, viewportRHI );
										} );

	// Draw viewport
	if ( viewportClient )
	{
		viewportClient->Draw( this );
	}

	// End drawing viewport
	UNIQUE_RENDER_COMMAND_TWOPARAMETER( CEndRenderCommand,
										ViewportRHIRef_t, viewportRHI, viewportRHI,
										bool, isShouldPresent, InIsShouldPresent,
										{
											CBaseDeviceContextRHI*		immediateContext = g_RHI->GetImmediateContext();
											g_RHI->EndDrawingViewport( immediateContext, viewportRHI, isShouldPresent, false );
										} );
}