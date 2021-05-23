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
 * @brief Class of test shader. Need delete after test
 */
class FTestShader : public FShader
{
	DECLARE_SHADER_TYPE( FTestShader )

public:
};

#endif // !TESTSHADER_H

