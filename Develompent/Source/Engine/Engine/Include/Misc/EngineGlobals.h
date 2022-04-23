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
extern class FBaseRHI*				GRHI;

/**
 * @ingroup Engine
 * @brief Script engine
 */
extern class FScriptEngine*			GScriptEngine;

/**
 * @ingroup Engine
 * @brief Shader manager
 */
extern class FShaderManager*		GShaderManager;

/**
 * @ingroup Engine
 * Cooked directory
 */
extern std::wstring					GCookedDir;

/**
 * @ingroup Engine
 * Engine
 */
extern class LBaseEngine*			GEngine;

/**
 * @ingroup Engine
 * Input system
 */
extern class FInputSystem*			GInputSystem;

/**
 * @ingroup Engine
 * World
 */
extern class FWorld*				  GWorld;

/**
 * @ingroup Engine
 * Is need use max tick rate
 */
extern bool                     GUseMaxTickRate;

/**
 * @ingroup Engine
 * Is allow render thread
 */
extern bool                     GAllowRenderThread;

/**
 * @ingroup Engine
 * @brief Camera manager
 */
extern class FCameraManager*    GCameraManager;

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
#endif // WITH_EDITOR

#endif // !ENGINEGLOBALS_H
