/**
 * @file
 * @addtogroup shadercompile shadercompile
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

#ifndef SHADERCOMPILE_H
#define SHADERCOMPILE_H

#include <string>
#include <vector>

#include "core/types.h"
#include "core/memory.h"

/**
 * @ingroup shadercompile
 * @brief Shader flag
 */
struct ShaderFlag
{
	/**
	 * @brief Constructor
	 */
	ShaderFlag()
		: min( 0 )
		, max( 0 )
		, bSetDefault( false )
		, defaultValue( 0 )
	{}

	std::string		name;			/**< Flag name */
	int32			min;			/**< Minimum value */
	int32			max;			/**< Maximum value */
	bool			bSetDefault;	/**< Is default value set */
	int32			defaultValue;	/**< Default value */
};

/**
 * @ingroup shadercompile
 * @brief Shader define
 */
struct ShaderDefine
{
	std::string		name;	/**< Define name */
	std::string		value;	/**< Define value */
};

/**
 * @ingroup shadercompile
 * @brief Information about vertex factory
 */
struct VertexFactory
{
	/**
	 * @brief Constructor
	 */
	VertexFactory()
		: bSupportsInstancing( false )
		, instanceStreamIndex( 0 )
	{}

	bool						bSupportsInstancing;	/**< Is supports instancing */
	uint32						instanceStreamIndex;	/**< Instance stream index */
	EVertexFactoryType			type;					/**< Vertex factory type */
	std::string					source;					/**< Path to shader source file */
	std::vector<ShaderDefine>	defines;				/**< Vertex factory defines */
};

/**
 * @ingroup shadercompile
 * @brief Information about shader
 */
struct Shader
{
	/**
	 * @brief Constructor
	 */
	Shader()
		: numFlagCombos( 1 )
	{
		// By default we don't use any vertex factory
		ResetUseVFs();
	}

	/**
	 * @brief Reset usage flag of vertex factories
	 */
	FORCEINLINE void ResetUseVFs()
	{
		Mem_Memzero( &bUseVertexFactory[0], VERTEXFACTORY_NUM_TYPES * sizeof( bool ) );
	}

	/**
	 * @brief Set usage of all vertex factory types
	 */
	FORCEINLINE void SetUseAllVFs()
	{
		Mem_Memset( &bUseVertexFactory[0], 1, VERTEXFACTORY_NUM_TYPES * sizeof( bool ) );
	}

	/**
	 * @brief Calculate maximum number of flag combinations
	 */
	void CalcNumFlagCombos();

	bool						bUseVertexFactory[VERTEXFACTORY_NUM_TYPES];		/**< Vertex factories used in this shader */
	uint32						numFlagCombos;									/**< Maximum number of flag combinations */
	EStudioShaderType			type;											/**< Shader type */
	std::string					source;											/**< Path to shader source file */
	std::string					mainFunc;										/**< Main function */
	std::vector<ShaderFlag>		flags;											/**< Shader flags */
	std::vector<ShaderDefine>	defines;										/**< Shader defines */
};

/**
 * @ingroup shadercompile
 * @brief Convert string to vertex factory type
 * 
 * @param pVFTypeName	Vertex factory type in string view
 * @param vfType		Converted vertex factory type
 * @return Return TRUE if converted, otherwise returns FALSE
 */
bool ConvStringToVFType( const achar* pVFTypeName, EVertexFactoryType& vfType );

/**
 * @ingroup shadercompile
 * @brief Convert vertex factory type to string
 * 
 * @param vfType		Vertex factory type
 * @param pVFTypeName	Converted vertex factory type into string view (DO NOT edit!)
 */
void ConvVFTypeToString( EVertexFactoryType vfType, const achar*& pVFTypeName );

/**
 * @ingroup shadercompile
 * @brief Convert string to shader type
 *
 * @param pShaderTypeName	Shader type in string view
 * @param shaderType		Converted shader type
 * @return Return TRUE if converted, otherwise returns FALSE
 */
bool ConvStringToShaderType( const achar* pShaderTypeName, EStudioShaderType& shaderType );

/**
 * @ingroup shadercompile
 * @brief Convert shader type to string
 *
 * @param shaderType		Shader type
 * @param pShaderTypeName	Converted shader type into string view (DO NOT edit!)
 */
void ConvShaderTypeToString( EStudioShaderType shaderType, const achar*& pShaderTypeName );

#endif // !SHADERCOMPILE_H