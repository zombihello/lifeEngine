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

#include "Core/Misc/CoreGlobals.h"
#include "Core/System/BaseWindow.h"
#include "Core/System/WindowEvent.h"
#include "Engine/System/CameraManager.h"

/*
==================
CCameraManager::BeginPlay
==================
*/
void CCameraManager::BeginPlay()
{
	// If camera is active - we update aspect ratio and width/height
	if ( activeCamera && activeCamera->IsAutoViewData() )
	{
		uint32		windowWidth;
		uint32		windowHeight;
		g_Window->GetSize( windowWidth, windowHeight );

		activeCamera->SetAspectRatio( ( float )windowWidth / windowHeight );
		activeCamera->SetOrthoWidth( windowWidth );
		activeCamera->SetOrthoHeight( windowHeight );
	}
}

/*
==================
CCameraManager::EndPlay
==================
*/
void CCameraManager::EndPlay()
{
	activeCamera = nullptr;
}

/*
==================
CCameraManager::ProcessEvent
==================
*/
void CCameraManager::ProcessEvent( struct WindowEvent &InWindowEvent )
{
	if ( !activeCamera )
	{
		return;
	}

	switch ( InWindowEvent.type )
	{
	case WindowEvent::T_WindowResize:
		if ( activeCamera && activeCamera->IsAutoViewData() )
		{
			activeCamera->SetAspectRatio( ( float )InWindowEvent.events.windowResize.width / InWindowEvent.events.windowResize.height );
			activeCamera->SetOrthoWidth( InWindowEvent.events.windowResize.width );
			activeCamera->SetOrthoHeight( InWindowEvent.events.windowResize.height );
		}
		break;
	}
}
