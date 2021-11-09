/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef SHADERCOMPILER_H
#define SHADERCOMPILER_H

#include <string>
#include <unordered_map>
#include <vector>

#include "RHI/BaseShaderRHI.h"

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
struct FShaderCompilerEnvironment
{
	/**
	 * @brief Constructor
	 * @param[in] InFrequency Shader frequency
	 * @param[in] InVertexFactoryFileName Vertex factory file name
	 */
	FShaderCompilerEnvironment( EShaderFrequency InFrequency, const std::wstring InVertexFactoryFileName = TEXT( "" ) ) :
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
	FShaderCompilerEnvironment( const FShaderCompilerEnvironment& InCopy )
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
struct FShaderCompilerOutput
{
	/**
	 * @brief Constructor
	 */
	FShaderCompilerOutput() :
		numInstructions( 0 )
	{}

	std::vector< byte >			code;				/**< Output compiled code of shader */
	uint32						numInstructions;	/**< Number instructions in shader */
};

/**
 * @ingroup Engine
 * @brief Class-manager for compiler shaders
 */
class FShaderCompiler
{
public:
	/**
	 * @brief Compile all shaders
	 * @param[in] InOutputCache	Path to output file with cache (example: ../../Content/GlobalShaderCache.bin)
	 * @return Return true if all shader compile successed, else return false
	 */
	bool CompileAll( const tchar* InOutputCache );

private:
	/**
	 * Compile shader
	 * 
	 * @param[in] InShaderName Shader name
	 * @param[in] InShaderSourceFileName Path to shader source file
	 * @param[in] InFunctionName Function of entry point in shader
	 * @param[in] InShaderFrequency Shader frequency
	 * @param[in,out] InOutShaderCache Shader cache
	 * @param[in] InVertexFactoryType Vertex factory type
	 * @return Return true if shader compile successed, else return false
	 */
	bool CompileShader( const std::wstring& InShaderName, const std::wstring& InShaderSourceFileName, const std::wstring& InFunctionName, EShaderFrequency InShaderFrequency, class FShaderCache& InOutShaderCache, class FVertexFactoryMetaType* InVertexFactoryType = nullptr );
};

#endif // !WITH_EDITOR
#endif // !SHADERCOMPILER_H
