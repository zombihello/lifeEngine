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

#include "UI/Misc/UIGlobals.h"
#include "UI/ImGUI/ImGUIEngine.h"
#include "Misc/WorldEdGlobals.h"
#include "System/WorldEdEngine.h"
#include "Widgets/ViewportWidget.h"

/*
==================
CViewportWidget::CViewportWidget
==================
*/
CViewportWidget::CViewportWidget( bool InIsEnabled /* = true */, CViewportClient* InViewportClient /* = nullptr */, bool InDeleteViewportClient /* = false */ )
	: bInit( false )
	, bEnabled( InIsEnabled )
	, bDeleteViewportClient( InDeleteViewportClient )
	, bViewportOnDrawing( false )
	, bHovered( false )
	, viewport( new CViewport() )
	, renderTarget( new CRenderTarget() )
	, viewportClient( InViewportClient )
	, size( 200, 200 )
{
	viewport->SetViewportClient( InViewportClient );
	SetEnabled( bEnabled );
	SetSize( size );
}

/*
==================
CViewportWidget::~CViewportWidget
==================
*/
CViewportWidget::~CViewportWidget()
{
	SetEnabled( false );
}

/*
==================
CViewportWidget::Init
==================
*/
void CViewportWidget::Init()
{
	bInit = true;
	if ( IsEnabled() )
	{
		g_WorldEdEngine->AddViewport( viewport );
		bViewportOnDrawing = true;
	}
}

/*
==================
CViewportWidget::Tick
==================
*/
void CViewportWidget::Tick()
{
	Assert( bInit );
	if ( renderTarget->IsValid() )
	{
		ImGui::Image( g_ImGUIEngine->LockTexture( renderTarget->GetTexture2DRHI() ), ImVec2{ ( float )size.x, ( float )size.y } );
		bHovered = ImGui::IsItemHovered();
	}
}

/*
==================
CViewportWidget::ProcessEvent
==================
*/
void CViewportWidget::ProcessEvent( struct WindowEvent& InWindowEvent )
{
	// Process ImGUI events
	if ( InWindowEvent.bImGUIEvent )
	{
		switch ( InWindowEvent.type )
		{
		case WindowEvent::T_WindowResize:
			SetSize( Vector2D( InWindowEvent.events.windowResize.width, InWindowEvent.events.windowResize.height ) );
			break;
		}
	}
}

/*
==================
CViewportWidget::SetEnabled
==================
*/
void CViewportWidget::SetEnabled( bool InIsEnabled )
{
	bEnabled = InIsEnabled;
	if ( IsInit() )
	{
		if ( InIsEnabled && !bViewportOnDrawing )
		{
			g_WorldEdEngine->AddViewport( viewport );
			bViewportOnDrawing = true;
		}
		else if ( !InIsEnabled && bViewportOnDrawing )
		{
			g_WorldEdEngine->RemoveViewport( viewport );
			bViewportOnDrawing = false;
		}
	}
}