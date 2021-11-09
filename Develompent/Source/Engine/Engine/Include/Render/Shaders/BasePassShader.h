/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef BASEPASSSHADER_H
#define BASEPASSSHADER_H

#include "Shader.h"
#include "ShaderManager.h"

/**
 * @ingroup Engine
 * @brief Class of base pass vertex shader
 */
class FBasePassVertexShader : public FShader
{
	DECLARE_SHADER_TYPE( FBasePassVertexShader )

public:
};

/**
 * @ingroup Engine
 * @brief Class of base pass pixel shader
 */
class FBasePassPixelShader : public FShader
{
	DECLARE_SHADER_TYPE( FBasePassPixelShader )

public:
};

#endif // !BASEPASSSHADER_H

