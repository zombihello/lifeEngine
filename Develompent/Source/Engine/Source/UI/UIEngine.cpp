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
#include "UI/UIEngine.h"

#if WITH_IMGUI
#include "UI/ImGUI/ImGUIEngine.h"
#endif // WITH_IMGUI

/*
==================
CUIEngine::Init
==================
*/
void CUIEngine::Init()
{
#if WITH_IMGUI
	g_ImGUIEngine->Init();
#endif // WITH_IMGUI
}

/*
==================
CUIEngine::Tick
==================
*/
void CUIEngine::Tick( float InDeltaSeconds )
{
#if WITH_IMGUI
	g_ImGUIEngine->Tick( InDeltaSeconds );
#endif // WITH_IMGUI
}

/*
==================
CUIEngine::Shutdown
==================
*/
void CUIEngine::Shutdown()
{
#if WITH_IMGUI
	g_ImGUIEngine->Shutdown();
#endif // WITH_IMGUI
}

/*
==================
CUIEngine::ProcessEvent
==================
*/
void CUIEngine::ProcessEvent( struct WindowEvent& InWindowEvent )
{
#if WITH_IMGUI
	g_ImGUIEngine->ProcessEvent( InWindowEvent );
#endif // WITH_IMGUI
}

/*
==================
CUIEngine::BeginDraw
==================
*/
void CUIEngine::BeginDraw()
{
#if WITH_IMGUI
	g_ImGUIEngine->BeginDraw();
#endif // WITH_IMGUI
}

/*
==================
CUIEngine::EndDraw
==================
*/
void CUIEngine::EndDraw()
{
#if WITH_IMGUI
	g_ImGUIEngine->EndDraw();
#endif // WITH_IMGUI
}