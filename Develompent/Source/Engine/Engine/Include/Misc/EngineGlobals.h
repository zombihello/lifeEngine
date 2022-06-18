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
extern class CBaseRHI*					GRHI;

/**
 * @ingroup Engine
 * @brief Script engine
 */
extern class CScriptEngine*				GScriptEngine;

/**
 * @ingroup Engine
 * @brief Shader manager
 */
extern class CShaderManager*			GShaderManager;

/**
 * @ingroup Engine
 * Cooked directory
 */
extern std::wstring						GCookedDir;

/**
 * @ingroup Engine
 * Engine
 */
extern class CBaseEngine*				GEngine;

/**
 * @ingroup Engine
 * Input system
 */
extern class CInputSystem*				GInputSystem;

/**
 * @ingroup Engine
 * World
 */
extern class CWorld*					GWorld;

/**
 * @ingroup Engine
 * Is need use max tick rate
 */
extern bool								GUseMaxTickRate;

/**
 * @ingroup Engine
 * @brief Camera manager
 */
extern class CCameraManager*			GCameraManager;

#if WITH_EDITOR
/**
 * @ingroup Engine
 * @brief Table for convert from text to ESurfaceType
 */
extern std::vector< std::pair< std::wstring, ESurfaceType > >		GSurfaceTypeNames;

/**
 * @ingroup Engine
 * @brief Is need cook editor content
 */
extern bool						GIsCookEditorContent;

/**
 * @ingroup Engine
 * @brief Allow debug shader dump
 */
extern bool						GAllowDebugShaderDump;
#endif // WITH_EDITOR

/**
 * @ingroup Engine
 * @brief Full screen movie player
 */
extern class CFullScreenMovieSupport*		GFullScreenMovie;

#endif // !ENGINEGLOBALS_H
