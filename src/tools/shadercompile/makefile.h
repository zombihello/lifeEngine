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

#ifndef MAKEFILE_H
#define MAKEFILE_H

#include "stdlib/jsondoc.h"
#include "tools/shadercompile/shadercompile.h"
#include "materialsystem/ivertexfactory.h"

/**
 * @ingroup shadercompile
 * @brief Makefile type
 */
enum EMakeFileType
{
	MAKEFILE_SHADER,		/**< Shader makefile */
	MAKEFILE_VERTEXFACTORY,	/**< Vertex factory makefile */
	MAKEFILE_NUM_TYPES,		/**< Number of type makefiles */
};

/**
 * @ingroup shadercompile
 * @brief Shader compile makefile
 */
class CMakeFile
{
public:
	/**
	 * @brief Constructor
	 */
	CMakeFile();

	/**
	 * @brief Load a makefile
	 *
	 * @param pPath		Path to the makefile
	 * @return Return TRUE if the makefile loaded, otherwise returns FALSE
	 */
	bool LoadMakeFile( const achar* pPath );

	/**
	 * @brief Clear makefile
	 */
	void Clear();

	/**
	 * @brief Set output path for generated C++ classes
	 * 
	 * @param makeFileType	Output path for specific makefile type
	 * @param pPath			New output path
	 */
	FORCEINLINE void SetCppGenOutput( EMakeFileType makeFileType, const achar* pPath )
	{
		cppGenOutput[( uint32 )makeFileType] = pPath;
	}

	/**
	 * @brief Get output path for generated C++ classes
	 * 
	 * @param makeFileType	Output path for specific makefile type
	 * @return Return output path for generated C++ classes. If not set returns 'generated/'
	 */
	FORCEINLINE const std::string& GetCppGenOutput( EMakeFileType makeFileType ) const
	{
		return cppGenOutput[( uint32 )makeFileType];
	}

	/**
	 * @brief Get array of shader's include directories
	 * @return Return array of shader's include directories
	 */
	FORCEINLINE const std::vector<std::string>& GetShaderIncludeDirs() const
	{
		return shaderIncludeDirs;
	}

	/**
	 * @brief Is valid vertex factory
	 * 
	 * @param vfType	Vertex factory type
	 * @return Return TRUE if vertex factory is valid, otherwise returns FALSE
	 */
	FORCEINLINE bool IsValidVertexFactory( EVertexFactoryType vfType ) const
	{
		Assert( vfType < VERTEXFACTORY_NUM_TYPES );
		return !vertexFactories[( uint32 )vfType].source.empty();
	}

	/**
	 * @brief Get vertex factory
	 * 
	 * @param vfType	Vertex factory type
	 * @return Return vertex factory with vfType type
	 */
	FORCEINLINE const VertexFactory& GetVertexFactory( EVertexFactoryType vfType ) const
	{
		Assert( vfType < VERTEXFACTORY_NUM_TYPES );
		return vertexFactories[( uint32 )vfType];
	}

	/**
	 * @brief Get array of shaders to compilation
	 * @return Return array of shaders to compilation
	 */
	FORCEINLINE const std::vector<Shader>& GetShaders() const
	{
		return shaders;
	}

	/**
	 * @brief Get makefile type
	 * @return Return makefile type (see EMakeFileType)
	 */
	FORCEINLINE EMakeFileType GetType() const
	{
		return type;
	}

	/**
	 * @brief Get path to directory where the makefile
	 * @return Return path to directory where the makefile
	 */
	FORCEINLINE const std::string GetMakeFileDir() const
	{
		return makeFileDir;
	}

private:
	/**
	 * @brief Load a shader makefile from JSON doc
	 *
	 * @param jsonDoc		JSON doc
	 * @param makeFileDir	Path to directory where the shader makefile
	 * @return Return TRUE if the makefile loaded, otherwise returns FALSE
	 */
	bool LoadShaderMakeFile( const CJsonDoc& jsonDoc, const std::string& makeFileDir );

	/**
	 * @brief Load a C++ generate output from JSON value
	 *
	 * @param jsonValue		JSON value
	 * @param cppGenOutput	Loaded C++ generate output
	 * @return Return TRUE if C++ generate output loaded, otherwise returns FALSE
	 */
	bool LoadCppGenOutput( const CJsonValue& jsonValue, std::string& cppGenOutput );

	/**
	 * @brief Load a vertex factory makefile from JSON doc
	 *
	 * @param jsonDoc		JSON doc
	 * @param makeFileDir	Path to directory where the vertex factory makefile
	 * @return Return TRUE if the makefile loaded, otherwise returns FALSE
	 */
	bool LoadVFMakeFile( const CJsonDoc& jsonDoc, const std::string& makeFileDir );

	/**
	 * @brief Load a vertex factory from JSON object
	 * 
	 * @param jsonObject	JSON object
	 * @return Return TRUE if the vertex factory loaded, otherwise returns FALSE
	 */
	bool LoadVertexFactory( const CJsonObject& jsonObject );

	/**
	 * @brief Load makefile type from JSON value
	 * 
	 * @param jsonValue		JSON value
	 * @param makeFileType	Make file Type
	 * @return Return TRUE if the makefile type loaded, otherwise returns FALSE
	 */
	bool LoadMakeFileType( const CJsonValue& jsonValue, EMakeFileType& makeFileType );

	/**
	 * @brief Load vertex factory type from JSON value
	 * 
	 * @param jsonValue		JSON value
	 * @param vfType		Loaded vertex factory type
	 * @return Return TRUE if the vertex factory type loaded, otherwise returns FALSE
	 */
	bool LoadVertexFactoryType( const CJsonValue& jsonValue, EVertexFactoryType& vfType );

	/**
	 * @brief Load defines from JSON array
	 * 
	 * @param jsonArray		JSON array
	 * @param defines		Loaded defines
	 * @return Return TRUE if defines loaded, otherwise returns FALSE
	 */
	bool LoadDefines( const std::vector<CJsonValue>& jsonArray, std::vector<ShaderDefine>& defines );

	/**
	 * @brief Load shader from JSON object
	 * 
	 * @param jsonObject	JSON object
	 * @param shader		Loaded shader
	 * @return Return TRUE if shader loaded, otherwise returns FALSE
	 */
	bool LoadShader( const CJsonObject& jsonObject, Shader& shader );

	/**
	 * @brief Load flags from JSON array
	 *
	 * @param jsonArray		JSON array
	 * @param flags			Loaded flags
	 * @return Return TRUE if flags loaded, otherwise returns FALSE
	 */
	bool LoadFlags( const std::vector<CJsonValue>& jsonArray, std::vector<ShaderFlag>& flags );

	EMakeFileType				type;										/**< Makefile type */
	std::string					makeFileDir;								/**< Path to directory where the makefile */
	std::string					cppGenOutput[MAKEFILE_NUM_TYPES];			/**< Output path for generated C++ classes */
	std::vector<std::string>	shaderIncludeDirs;							/**< Array of shader's include directories */
	VertexFactory				vertexFactories[VERTEXFACTORY_NUM_TYPES];	/**< Array of vertex factories */
	std::vector<Shader>			shaders;									/**< Array of shaders to compilation */
};

#endif // !MAKEFILE_H