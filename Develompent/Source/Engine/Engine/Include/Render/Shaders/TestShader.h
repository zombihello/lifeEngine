/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef TESTSHADER_H
#define TESTSHADER_H

#include "Shader.h"
#include "ShaderManager.h"

/**
 * @ingroup Engine
 * @brief Class of test vertex shader. Need delete after test
 */
class FTestVertexShader : public FShader
{
	DECLARE_SHADER_TYPE( FTestVertexShader )

public:
};

/**
 * @ingroup Engine
 * @brief Class of test pixel shader. Need delete after test
 */
class FTestPixelShader : public FShader
{
	DECLARE_SHADER_TYPE( FTestPixelShader )

public:
};

#endif // !TESTSHADER_H

