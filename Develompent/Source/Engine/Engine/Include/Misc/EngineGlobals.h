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
 * Game name
 */
extern std::wstring					GGameName;

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
 * @ingroup Core
 * Is need use max tick rate
 */
extern bool                   GUseMaxTickRate;

/**
 * @ingroup Core
 * Is allow render thread
 */
extern bool                   GAllowRenderThread;

#endif // !ENGINEGLOBALS_H
