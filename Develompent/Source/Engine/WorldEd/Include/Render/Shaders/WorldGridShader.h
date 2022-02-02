/**
 * @file
 * @addtogroup WorldEd WorldEd
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef WORLDGRIDSHADER_H
#define WORLDGRIDSHADER_H

#include "Render/Shaders/Shader.h"
#include "Render/Shaders/ShaderManager.h"

/**
 * @ingroup WorldEd
 * @brief Class of world grid vertex shader
 */
class FWorldGridVertexShader : public FShader
{
	DECLARE_SHADER_TYPE( FWorldGridVertexShader )

public:
};

/**
 * @ingroup WorldEd
 * @brief Class of world grid pixel shader
 */
class FWorldGridPixelShader : public FShader
{
	DECLARE_SHADER_TYPE( FWorldGridPixelShader )

public:
};

#endif // !WORLDGRIDSHADER_H

