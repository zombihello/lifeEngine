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
#include "Engine/Scripts/ScriptEngine.h"
#include "Engine/Render/Shaders/ShaderManager.h"
#include "Engine/System/InputSystem.h"
#include "Engine/System/World.h"
#include "Engine/System/CameraManager.h"
#include "Engine/System/ConsoleSystem.h"

// -------------
// GLOBALS
// -------------

CScriptEngine*												g_ScriptEngine = new CScriptEngine();
CShaderManager*												g_ShaderManager = new CShaderManager();
std::wstring												g_CookedDir = TEXT( "" );
class CBaseEngine*											g_Engine = nullptr;
CInputSystem*												g_InputSystem = new CInputSystem();
CWorld*														g_World = new CWorld();
bool														g_UseMaxTickRate = true;
CCameraManager*												g_CameraManager = new CCameraManager();

#if WITH_EDITOR
std::vector< std::pair< std::wstring, ESurfaceType > >		g_SurfaceTypeNames( 1, std::make_pair( TEXT( "Default" ), ST_Default ) );
bool														g_IsCookEditorContent = false;
bool														g_AllowDebugShaderDump = false;
#endif // WITH_EDITOR

class CFullScreenMovieSupport*								g_FullScreenMovie = nullptr;
CConsoleSystem												g_ConsoleSystem;
