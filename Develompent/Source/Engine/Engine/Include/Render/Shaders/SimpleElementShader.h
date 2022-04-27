/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef SIMPLEELEMENTSHADER_H
#define SIMPLEELEMENTSHADER_H

#include "Shader.h"
#include "ShaderManager.h"

/**
 * @ingroup Engine
 * @brief Class of simple element vertex shader
 */
class FSimpleElementVertexShader : public FShader
{
	DECLARE_SHADER_TYPE( FSimpleElementVertexShader )

public:
#if WITH_EDITOR
	/**
	 * @brief Is need compile shader for platform
	 *
	 * @param InShaderPlatform Shader platform
	 * @param InVFMetaType Vertex factory meta type. If him is nullptr - return general check
	 * @return Return true if need compile shader, else returning false
	 */
	static bool ShouldCache( EShaderPlatform InShaderPlatform, class FVertexFactoryMetaType* InVFMetaType = nullptr );
#endif // WITH_EDITOR
};

/**
 * @ingroup Engine
 * @brief Class of simple element pixel shader
 */
class FSimpleElementPixelShader : public FShader
{
	DECLARE_SHADER_TYPE( FSimpleElementPixelShader )

public:
#if WITH_EDITOR
	/**
	 * @brief Is need compile shader for platform
	 *
	 * @param InShaderPlatform Shader platform
	 * @param InVFMetaType Vertex factory meta type. If him is nullptr - return general check
	 * @return Return true if need compile shader, else returning false
	 */
	static bool ShouldCache( EShaderPlatform InShaderPlatform, class FVertexFactoryMetaType* InVFMetaType = nullptr );
#endif // WITH_EDITOR
};

#endif // !SIMPLEELEMENTSHADER_H

