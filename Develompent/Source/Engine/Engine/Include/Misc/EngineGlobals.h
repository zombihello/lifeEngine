/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef ENGINEGLOBALS_H
#define ENGINEGLOBALS_H

#include <string>
#include <vector>

#include "Misc/PhysicsTypes.h"

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
