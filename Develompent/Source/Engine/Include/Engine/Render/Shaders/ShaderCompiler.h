/**
 * @file
 * @addtogroup Engine Engine
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

#ifndef SHADERCOMPILER_H
#define SHADERCOMPILER_H

#include <string>
#include <unordered_map>
#include <vector>

#include "RHI/BaseShaderRHI.h"
#include "Engine/Render/Shaders/ShaderCache.h"

/**
 * @ingroup Engine
 * @brief Enumeration of shader platform type
 */
enum EShaderPlatform
{
	SP_PCD3D_SM5,		/**< PC shader model 5 (DirectX 11) */
	SP_Unknown,			/**< Unknown */
	SP_NumPlatforms,	/**< Number of shader platforms */
};

/**
 * @ingroup Engine
 * @brief Converts shader platform to human readable string
 *
 * @param InShaderPlatform	Shader platform enum
 * @return Return text representation of enum
 */
FORCEINLINE const tchar* ShaderPlatformToText( EShaderPlatform InShaderPlatform )
{
	switch ( InShaderPlatform )
	{
	case SP_PCD3D_SM5:		return TEXT( "PC-D3D-SM5" );
	default:
		Sys_Errorf( TEXT( "Unknown shader platform 0x%X" ), InShaderPlatform );
		return TEXT( "UNKNOWN" );
	}
}

#if WITH_EDITOR
/**
 * @ingroup Engine
 * @brief Enumeration of flags for compile shader
 */
enum ECompilerFlags
{
	CF_PreferFlowControl,				/**< Prefer flow control */
	CF_Debug,							/**< Debug of shader */
	CF_AvoidFlowControl,				/**< Avoid flow control*/
	CF_SkipValidation					/**< Disable shader validation */
};

/**
 * @ingroup Engine
 * @brief The environment used to compile a shader
 */
struct ShaderCompilerEnvironment
{
	/**
	 * @brief Constructor
	 * @param[in] InFrequency Shader frequency
	 * @param[in] InVertexFactoryFileName Vertex factory file name
	 */
	ShaderCompilerEnvironment( EShaderFrequency InFrequency, const std::wstring InVertexFactoryFileName = TEXT( "" ) ) :
		vertexFactoryFileName( InVertexFactoryFileName )
	{
		difinitions.insert( std::make_pair( TEXT( "VERTEXSHADER" ), InFrequency == SF_Vertex ? TEXT( "1" ) : TEXT( "0" ) ) );
		difinitions.insert( std::make_pair( TEXT( "DOMAINSHADER" ), InFrequency == SF_Domain ? TEXT( "1" ) : TEXT( "0" ) ) );
		difinitions.insert( std::make_pair( TEXT( "HULLSHADER" ), InFrequency == SF_Hull ? TEXT( "1" ) : TEXT( "0" ) ) );
		difinitions.insert( std::make_pair( TEXT( "GEOMETRYSHADER" ), InFrequency == SF_Geometry ? TEXT( "1" ) : TEXT( "0" ) ) );
		difinitions.insert( std::make_pair( TEXT( "PIXELSHADER" ), InFrequency == SF_Pixel ? TEXT( "1" ) : TEXT( "0" ) ) );
		difinitions.insert( std::make_pair( TEXT( "COMPUTESHADER" ), InFrequency == SF_Compute ? TEXT( "1" ) : TEXT( "0" ) ) );		
	}

	/**
	 * @brief Constructor of copy
	 * @param[in] InCopy Copy
	 */
	ShaderCompilerEnvironment( const ShaderCompilerEnvironment& InCopy )
	{
		vertexFactoryFileName = InCopy.vertexFactoryFileName;
		includeFiles = InCopy.includeFiles;
		difinitions = InCopy.difinitions;
		compilerFlags = InCopy.compilerFlags;
	}

	std::wstring												vertexFactoryFileName;	/**< Vertex factory file name */
	std::unordered_map< std::wstring, std::wstring >			includeFiles;			/**< Include files for compiling shader */
	std::unordered_map< std::wstring, std::wstring >			difinitions;			/**< Difinitions for compiling shader */
	std::vector< ECompilerFlags >								compilerFlags;			/**< Array of flags compiling */
};

/**
 * @ingroup Engine
 * @brief The output of the shader compiler
 */
struct ShaderCompilerOutput
{
	/**
	 * @brief Constructor
	 */
	ShaderCompilerOutput() :
		numInstructions( 0 )
	{}

	std::vector< byte >			code;				/**< Output compiled code of shader */
	CShaderParameterMap			parameterMap;		/**< Shader parameter map */
	uint32						numInstructions;	/**< Number instructions in shader */
	std::wstring				errorMsg;			/**< Error message. Compiler puting to this field message when shader compiled is fail */
};

/**
 * @ingroup Engine
 * @brief Class-manager for compiler shaders
 */
class CShaderCompiler
{
public:
	/**
	 * @brief Compile all shaders
	 * 
	 * @param[in] InOutputCache	Path to output file with cache (example: ../../Content/GlobalShaderCache.bin)
	 * @param[in] InShaderPlatform Shader platform enum
	 * @param[in] InOnlyGlobals Compile only global shaders
	 * @return Return true if all shader compile successed, else return false
	 */
	bool CompileAll( const tchar* InOutputCache, EShaderPlatform InShaderPlatform, bool InOnlyGlobals = false );

	/**
	 * @brief Compile all shaders
	 * 
	 * @param InOutShaderCache		Shader cache
	 * @param InShaderPlatform		Shader platform enum
	 * @param InOnlyGlobals			Compile only global shaders
	 * @return Return TRUE if all shader compile successed, else return FALSE
	 */
	bool CompileAll( CShaderCache& InOutShaderCache, EShaderPlatform InShaderPlatform, bool InOnlyGlobals = false );

	/**
	 * Compile shader
	 * 
	 * @param[in] InShaderMetaType Shader meta type
	 * @param[in] InShaderPlatform Shader platform enum
	 * @param[in,out] InOutShaderCache Shader cache
	 * @param[out] OutErrorMsg Compile error message
	 * @param[in] InVertexFactoryType Vertex factory type
	 * @return Return true if shader compile successed, else return false
	 */
	bool CompileShader( class CShaderMetaType* InShaderMetaType, EShaderPlatform InShaderPlatform, class CShaderCache& InOutShaderCache, std::wstring& OutErrorMsg, class CVertexFactoryMetaType* InVertexFactoryType = nullptr );
};

#endif // !WITH_EDITOR
#endif // !SHADERCOMPILER_H
