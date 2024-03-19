/**
 * @file
 * @addtogroup materialsystem materialsystem
 *
 * ************************************************************
 *                  This file is part of:
 *                      LIFEENGINE
 *          https://github.com/zombihello/lifeEngine
 * ************************************************************
 * Copyright (C) 2024 Yehor Pohuliaka.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef ISHADER_H
#define ISHADER_H

#include "core/types.h"

/**
 * @ingroup materialsystem
 * @brief Shader flags
 */
enum EShaderFlags
{
	SHADER_NOT_EDITABLE			= 1 << 1		/**< Shader not editable */
};

/**
 * @ingroup materialsystem
 * @brief Shader parameter flags
 */
enum EShaderParamFlags
{
	SHADER_PARAM_NOT_EDITABLE	= 1 << 1		/**< Shader parameter not editable */
};

/**
 * @ingroup materialsystem
 * @brief Shader parameter type
 */
enum EShaderParamType
{
	SHADER_PARAM_TYPE_UNDEFINED,	/**< Undefined type */
	SHADER_PARAM_TYPE_BOOL,			/**< Bool */
	SHADER_PARAM_TYPE_INT,			/**< Integer */
	SHADER_PARAM_TYPE_FLOAT,		/**< Float */
	SHADER_PARAM_TYPE_VECTOR_2D,	/**< Vector 2D */
	SHADER_PARAM_TYPE_VECTOR_3D,	/**< Vector 3D */
	SHADER_PARAM_TYPE_VECTOR_4D,	/**< Vector 4D */
	SHADER_PARAM_TYPE_MATRIX,		/**< Matrix */
	SHADER_PARAM_TYPE_STRING,		/**< String */
	SHADER_PARAM_TYPE_TEXTURE,		/**< Texture */
	SHADER_PARAM_TYPE_MATERIAL		/**< Material */
};

/**
 * @ingroup materialsystem
 * @brief Shader parameter
 */
struct ShaderParam
{
	const achar*		pName;			/**< Name */
	EShaderParamType	type;			/**< Type */
	const achar*		pDefaultValue;	/**< Default value */
	const achar*		pHelpString;	/**< Help message */
	uint32				flags;			/**< Flags (see EShaderParamFlags) */
};

/**
 * @ingroup materialsystem
 * @brief Shader interface
 */
class IShader
{
public:
	/**
	 * @brief Get number of shader parameters
	 * @return Return number of shader parameters
	 */
	virtual uint32 GetNumParams() const = 0;

	/**
	 * @brief Get shader parameter by index
	 * 
	 * @param index		Parameter index
	 * @return Return shader parameter
	 */
	virtual ShaderParam GetParam( uint32 index ) const = 0;

	/**
	 * @brief Get shader flags
	 * @return Return shader flags (see EShaderFlags)
	 */
	virtual uint32 GetFlags() const = 0;

	/**
	 * @brief Get shader name
	 * @return Return name of this shader
	 */
	virtual const achar* GetName() const = 0;

	/**
	 * @brief Get shader help message
	 * @return Return help message for this shader 
	 */
	virtual const achar* GetHelp() const = 0;

	/**
	 * @brief Get fallback shader 
	 * @return If StudioAPI isn't support this shader return fallback shader name, otherwise returns NULL
	 */
	virtual const achar* GetFallbackShader() const = 0;
};

#endif // !ISHADER_H