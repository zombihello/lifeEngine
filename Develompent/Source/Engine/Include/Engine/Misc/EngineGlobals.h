/**
 * @file
 * @addtogroup Engine Engine
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

#ifndef ENGINEGLOBALS_H
#define ENGINEGLOBALS_H

#include <string>
#include <vector>

#include "Physics/Misc/PhysicsTypes.h"

 /**
  * @ingroup Engine
  * @brief RHI
  */
extern class CBaseRHI*					g_RHI;

/**
 * @ingroup Engine
 * @brief Script engine
 */
extern class CScriptEngine*				g_ScriptEngine;

/**
 * @ingroup Engine
 * @brief Shader manager
 */
extern class CShaderManager*			g_ShaderManager;

/**
 * @ingroup Engine
 * Cooked directory
 */
extern std::wstring						g_CookedDir;

/**
 * @ingroup Engine
 * Engine
 */
extern class CBaseEngine*				g_Engine;

/**
 * @ingroup Engine
 * Input system
 */
extern class CInputSystem*				g_InputSystem;

/**
 * @ingroup Engine
 * World
 */
extern class CWorld*					g_World;

/**
 * @ingroup Engine
 * Is need use max tick rate
 */
extern bool								g_UseMaxTickRate;

/**
 * @ingroup Engine
 * @brief Camera manager
 */
extern class CCameraManager*			g_CameraManager;

#if WITH_EDITOR
/**
 * @ingroup Engine
 * @brief Table for convert from text to ESurfaceType
 */
extern std::vector< std::pair< std::wstring, ESurfaceType > >		g_SurfaceTypeNames;

/**
 * @ingroup Engine
 * @brief Is need cook editor content
 */
extern bool						g_IsCookEditorContent;

/**
 * @ingroup Engine
 * @brief Allow debug shader dump
 */
extern bool						g_AllowDebugShaderDump;
#endif // WITH_EDITOR

/**
 * @ingroup Engine
 * @brief Full screen movie player
 */
extern class CFullScreenMovieSupport*		g_FullScreenMovie;

/**
 * @ingroup Engine
 * @brief Console system
 */
extern class CConsoleSystem					g_ConsoleSystem;

#endif // !ENGINEGLOBALS_H
