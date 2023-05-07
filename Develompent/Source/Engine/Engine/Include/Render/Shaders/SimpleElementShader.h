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
class CSimpleElementVertexShader : public CShader
{
	DECLARE_SHADER_TYPE( CSimpleElementVertexShader )

public:
#if WITH_EDITOR
	/**
	 * @brief Is need compile shader for platform
	 *
	 * @param InShaderPlatform Shader platform
	 * @param InVFMetaType Vertex factory meta type. If him is nullptr - return general Assert
	 * @return Return true if need compile shader, else returning false
	 */
	static bool ShouldCache( EShaderPlatform InShaderPlatform, class CVertexFactoryMetaType* InVFMetaType = nullptr );
#endif // WITH_EDITOR
};

/**
 * @ingroup Engine
 * @brief Class of simple element pixel shader
 */
class CSimpleElementPixelShader : public CShader
{
	DECLARE_SHADER_TYPE( CSimpleElementPixelShader )

public:
#if WITH_EDITOR
	/**
	 * @brief Is need compile shader for platform
	 *
	 * @param InShaderPlatform Shader platform
	 * @param InVFMetaType Vertex factory meta type. If him is nullptr - return general Assert
	 * @return Return true if need compile shader, else returning false
	 */
	static bool ShouldCache( EShaderPlatform InShaderPlatform, class CVertexFactoryMetaType* InVFMetaType = nullptr );
#endif // WITH_EDITOR
};

#endif // !SIMPLEELEMENTSHADER_H

