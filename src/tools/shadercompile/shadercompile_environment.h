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

#ifndef SHADERCOMPILE_ENVIRONMENT_H
#define SHADERCOMPILE_ENVIRONMENT_H

#include <string>
#include <vector>
#include <unordered_map>

#include "tools/shadercompile/ishadercompile_backend.h"

/**
 * @ingroup shadercompile
 * @brief The environment used to compile shader
 */
class CShaderCompileEnvironment : public IShaderCompileEnvironment
{
public:
	/**
	 * @brief Constructor
	 *
	 * @param type	Shader type
	 */
	CShaderCompileEnvironment( EStudioShaderType type );

	/**
	 * @brief Copy constructor
	 * @param copy		Copy
	 */
	CShaderCompileEnvironment( const CShaderCompileEnvironment& copy );

	/**
	 * @brief Set vertex factory file name
	 * @param pVFFileName		Vertex factory file name. NULL or empty string means what we don't use any vertex factory
	 */
	FORCEINLINE void SetVertexFactoryFileName( const achar* pVFFileName )
	{
		vertexFactoryFileName = pVFFileName;
	}

	/**
	 * @brief Get vertex factory file name
	 * @return Return vertex factory file name. If not set will return empty string
	 */
	virtual const achar* GetVertexFactoryFileName() const override;

	/**
	 * @brief Add include dir
	 * @param pPath		Path to directory where shader compiler will find include files
	 */
	FORCEINLINE void AddIncludeDir( const achar* pPath )
	{
		includeDirs.push_back( pPath );
	}

	/**
	 * @brief Add define
	 *
	 * Example usage: @code AddDefine( "MYDEF", "1" ); @endcode
	 *
	 * @param pName		Name
	 * @param pValue	Value
	 */
	FORCEINLINE void AddDefine( const achar* pName, const achar* pValue )
	{
		defines[pName] = pValue;
	}

	/**
	 * @brief Set compile flags
	 * @param flags		Compiler flags (see EShaderCompileFlags)
	 */
	FORCEINLINE void SetCompileFlags( uint32 flags )
	{
		compileFlags = flags;
	}

	/**
	 * @brief Get compile flags
	 * @return Return compile flags (see EShaderCompileFlags)
	 */
	virtual uint32 GetCompileFlags() const override;

	/**
	 * @brief Get number of include directories
	 * @return Return number of include directories
	 */
	virtual uint32 GetNumIncludeDirs() const override;

	/**
	 * @brief Get number of defines
	 * @return Return number of defines
	 */
	virtual uint32 GetNumDefines() const override;

	/**
	 * @brief Get include directory by index
	 *
	 * @param index			Include directory index
	 * @return Return path to include directory at index
	 */
	virtual const achar* GetIncludeDir( uint32 index ) const override;

	/**
	 * @brief Get define by index
	 *
	 * @param index			Define index
	 * @param pName			Output name
	 * @param pValue		Output value
	 */
	virtual void GetDefine( uint32 index, const achar*& pName, const achar*& pValue ) const override;

private:
	std::string										vertexFactoryFileName;	/**< Vertex factory file name */
	std::vector<std::string>						includeDirs;			/**< Include directories */
	std::unordered_map<std::string, std::string>	defines;				/**< Defines for compiling shader */
	uint32											compileFlags;			/**< Compile flags (see EShaderCompileFlags) */
};

#endif // !SHADERCOMPILE_ENVIRONMENT_H