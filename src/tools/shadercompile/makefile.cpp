/**
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

#include "pch_shadercompile.h"
#include "stdlib/jsondoc.h"
#include "tools/shadercompile/makefile.h"

/**
 * @ingroup shadercompile
 * @brief Default output path for generated C++ classes
 */
#define DEFAULT_CPPGEN_OUTPUT	"generated/"

// Table of makefile type names
static const achar* s_pMakeFileTypeNames[] =
{
	"shader",			// MAKEFILE_SHADER
	"vertexfactory"		// MAKEFILE_VERTEXFACTORY
};
static_assert( MAKEFILE_NUM_TYPES == ARRAYSIZE( s_pMakeFileTypeNames ), "Array size 's_pMakeFileTypeNames' must be equal to MAKEFILE_NUM_TYPES" );

/*
==================
ConvStringToMakeFileType
==================
*/
static bool ConvStringToMakeFileType( const achar* pMakeFileTypeName, EMakeFileType& makefileType )
{
	for ( uint32 index = 0; index < ARRAYSIZE( s_pMakeFileTypeNames ); ++index )
	{
		if ( !L_Stricmp( pMakeFileTypeName, s_pMakeFileTypeNames[index] ) )
		{
			// We found! Return current type
			makefileType = ( EMakeFileType )index;
			return true;
		}
	}

	// We not found, return invalid type
	makefileType = MAKEFILE_NUM_TYPES;
	return false;
}


/*
==================
CMakeFile::CMakeFile
==================
*/
CMakeFile::CMakeFile()
{
	Clear();
}

/*
==================
CMakeFile::LoadMakeFile
==================
*/
bool CMakeFile::LoadMakeFile( const achar* pPath )
{
	Msg( "CMakeFile: Load makefile '%s'", pPath );

	// Load JSON file
	CJsonDoc	jsonMakeFile;
	if ( !jsonMakeFile.LoadFromFile( pPath ) )
	{
		Warning( "CMakeFile: Failed to load, maybe wrong JSON syntax?" );
		return false;
	}
	bool			bResult = true;

	// Get path to directory with the main makefile
	{
		std::string		tmpBuffer;
		L_GetFilePath( pPath, tmpBuffer, false );
		L_MakeAbsolutePath( tmpBuffer, makeFileDir, "", false );
	}

	// Get makefile type
	{
		CJsonValue			jsonMakeFileType = jsonMakeFile.GetValue( "makefile_type" );
		if ( jsonMakeFileType.IsValid() )
		{
			if ( !LoadMakeFileType( jsonMakeFileType, type ) )
			{
				Warning( "CMakeFile: Failed to load makefile type from field 'makefile_type'" );
				return false;
			}
		}
		else
		{
			Warning( "CMakeFile: Invalid makefile, not found required field 'makefile_type'" );
			return false;
		}
	}

	switch ( type )
	{
		// Load makefile as shader type
	case MAKEFILE_SHADER:
		if ( !LoadShaderMakeFile( jsonMakeFile, makeFileDir ) )
		{
			bResult = false;
		}
		break;

		// Load makefile as vertex factory type
	case MAKEFILE_VERTEXFACTORY:
		if ( !LoadVFMakeFile( jsonMakeFile, makeFileDir ) )
		{
			bResult = false;
		}
		break;
	}

	return bResult;
}

/*
==================
CMakeFile::LoadMakeFileType
==================
*/
bool CMakeFile::LoadMakeFileType( const CJsonValue& jsonValue, EMakeFileType& makeFileType )
{
	bool			bResult = true;
	std::string		strMakeFileType = jsonValue.GetString();
	if ( !jsonValue.IsA( JSONVALUE_TYPE_STRING ) )
	{
		Warning( "CMakeFile: Invalid makefile type, must be string" );
		bResult = false;
	}

	if ( !ConvStringToMakeFileType( strMakeFileType.c_str(), makeFileType ) )
	{
		Warning( "CMakeFile: Unknown makefile type '%s'", strMakeFileType.c_str() );
		bResult = false;
	}

	return bResult;
}

/*
==================
CMakeFile::LoadCppGenOutput
==================
*/
bool CMakeFile::LoadCppGenOutput( const CJsonValue& jsonValue, std::string& cppGenOutput )
{
	bool	bResult = true;
	cppGenOutput = jsonValue.GetString( DEFAULT_CPPGEN_OUTPUT );
	if ( !jsonValue.IsA( JSONVALUE_TYPE_STRING ) )
	{
		Warning( "CMakeFile: Invalid C++ generate output, must be string" );
		bResult = false;
	}

	return bResult;
}

/*
==================
CMakeFile::LoadShaderMakeFile
==================
*/
bool CMakeFile::LoadShaderMakeFile( const CJsonDoc& jsonDoc, const std::string& makeFileDir )
{
	bool	bResult = true;

	// Get output path for generated C++ classes
	CJsonValue		jsonCppGenOutput = jsonDoc.GetValue( "cppgen_output" );
	if ( jsonCppGenOutput.IsValid() )
	{
		if ( !LoadCppGenOutput( jsonCppGenOutput, cppGenOutput[MAKEFILE_SHADER] ) )
		{
			Warning( "CMakeFile: Failed to load C++ generate output, invalid 'cppgen_output'" );
			bResult = false;
		}
	}
	else
	{
		cppGenOutput[MAKEFILE_SHADER] = DEFAULT_CPPGEN_OUTPUT;
	}

	// Convert C++ generated output to absolute 
	{
		std::string		tmpBuffer = cppGenOutput[MAKEFILE_SHADER];
		L_MakeAbsolutePath( tmpBuffer, cppGenOutput[MAKEFILE_SHADER], makeFileDir );
	}

	// Get shader's include directories
	{
		CJsonValue		jsonInclude = jsonDoc.GetValue( "include" );
		if ( jsonInclude.IsValid() )
		{
			std::vector<CJsonValue>		jsonArray = jsonInclude.GetArray();
			if ( !jsonInclude.IsA( JSONVALUE_TYPE_ARRAY ) )
			{
				Warning( "CMakeFile: Invalid 'include', must be array of strings" );
				bResult = false;
			}
			else
			{
				for ( uint32 index = 0, count = ( uint32 )jsonArray.size(); index < count; ++index )
				{
					const CJsonValue&	jsonElement = jsonArray[index];
					if ( !jsonElement.IsA( JSONVALUE_TYPE_STRING ) )
					{
						Warning( "CMakeFile: Invalid 'include[%i]', must be string", index );
						bResult = false;
						continue;
					}

					// Convert of the include path to absolute and emplace at back
					std::string		absoluteInclude;
					L_MakeAbsolutePath( jsonElement.GetString(), absoluteInclude, makeFileDir );
					shaderIncludeDirs.emplace_back( absoluteInclude );
				}
			}
		}
	}

	// Get path to vertex factory makefile
	{
		CJsonValue		jsonVertexFactories = jsonDoc.GetValue( "vertex_factories" );
		if ( jsonVertexFactories.IsValid() )
		{
			if ( !jsonVertexFactories.IsA( JSONVALUE_TYPE_STRING ) )
			{
				Warning( "CMakeFile: Invalid 'vertex_factories', must be string" );
				bResult = false;
			}
			else
			{
				// Convert to absolute path and load vertex factory makefile
				std::string		tmpBuffer = jsonVertexFactories.GetString();
				std::string		absoluteVFMakeFilePath;
				L_MakeAbsolutePath( tmpBuffer.c_str(), absoluteVFMakeFilePath, makeFileDir );
				
				// Load JSON file
				CJsonDoc		jsonVFMakeFile;
				if ( !jsonVFMakeFile.LoadFromFile( absoluteVFMakeFilePath.c_str() ) )
				{
					Warning( "CMakeFile: Failed to load vertex factory makefile '%s', maybe wrong JSON syntax?", absoluteVFMakeFilePath.c_str() );
					bResult = false;
				}

				// Make sure what the makefile is type vertexfactory 
				EMakeFileType	vfMakeFileType = MAKEFILE_NUM_TYPES;
				CJsonValue		jsonVFMakeFileType = jsonVFMakeFile.GetValue( "makefile_type" );
				if ( jsonVFMakeFileType.IsValid() )
				{
					if ( !LoadMakeFileType( jsonVFMakeFileType, vfMakeFileType ) )
					{
						Warning( "CMakeFile: Failed to load makefile type from field 'makefile_type' in '%s'", absoluteVFMakeFilePath.c_str() );
						bResult = false;
					}

					if ( vfMakeFileType != MAKEFILE_VERTEXFACTORY )
					{
						Warning( "CMakeFile: Invalid vertex factory makefile '%s'. Field 'makefile_type' must be '%s'", absoluteVFMakeFilePath.c_str(), s_pMakeFileTypeNames[( uint32 )MAKEFILE_VERTEXFACTORY] );
						bResult = false;
					}
				}
				else
				{
					Warning( "CMakeFile: Invalid vertex factory makefile '%s', not found required field 'makefile_type'", absoluteVFMakeFilePath.c_str() );
					bResult = false;
				}

				if ( vfMakeFileType != MAKEFILE_NUM_TYPES )
				{
					// Get path to directory with the vertex factory makefile
					std::string		vfMakeFileDir;
					{
						std::string		tmpBuffer;
						L_GetFilePath( absoluteVFMakeFilePath, tmpBuffer, false );
						L_MakeAbsolutePath( tmpBuffer, vfMakeFileDir, "", false );
					}

					// Load the vertex factory makefile
					if ( !LoadVFMakeFile( jsonVFMakeFile, vfMakeFileDir ) )
					{
						Warning( "CMakeFile: Failed to load vertex factory makefile '%s'", tmpBuffer.c_str() );
						bResult = false;
					}
				}
			}
		}
	}

	// Get shaders to compile
	{
		CJsonValue		jsonShaders = jsonDoc.GetValue( "shaders" );
		if ( jsonShaders.IsValid() )
		{
			std::vector<CJsonValue>		jsonArray = jsonShaders.GetArray();
			if ( !jsonShaders.IsA( JSONVALUE_TYPE_ARRAY ) )
			{
				Warning( "CMakeFile: Invalid 'shaders', must be array of objects" );
				bResult = false;
			}
			else
			{
				for ( uint32 index = 0, count = ( uint32 )jsonArray.size(); index < count; ++index )
				{
					const CJsonValue&	jsonElement = jsonArray[index];
					if ( !jsonElement.IsA( JSONVALUE_TYPE_OBJECT ) )
					{
						Warning( "CMakeFile: Invalid 'shaders[%i]', must be object with required fields: 'source', 'main_func' and 'type'", index );
						bResult = false;
						continue;
					}

					// Load shader from the JSON object
					Shader			shader;
					if ( !LoadShader( jsonElement.GetObject(), shader ) )
					{
						Warning( "CMakeFile: Failed to load shader from 'shaders[%i]'", index );
						bResult = false;
					}

					// Convert source path to absolute
					std::string		tmpBuffer = shader.source;
					L_MakeAbsolutePath( tmpBuffer, shader.source, makeFileDir );

					// Add a new shader into array
					shaders.emplace_back( shader );
				}
			}
		}
	}

	return bResult;
}

/*
==================
CMakeFile::LoadVFMakeFile
==================
*/
bool CMakeFile::LoadVFMakeFile( const CJsonDoc& jsonDoc, const std::string& makeFileDir )
{
	bool			bResult = true;

	// Get output path for generated C++ classes
	CJsonValue		jsonCppGenOutput = jsonDoc.GetValue( "cppgen_output" );
	if ( jsonCppGenOutput.IsValid() )
	{
		if ( !LoadCppGenOutput( jsonCppGenOutput, cppGenOutput[MAKEFILE_VERTEXFACTORY] ) )
		{
			Warning( "CMakeFile: Failed to load C++ generate output, invalid 'cppgen_output'" );
			bResult = false;
		}
	}
	else
	{
		cppGenOutput[MAKEFILE_VERTEXFACTORY] = DEFAULT_CPPGEN_OUTPUT;
	}

	// Convert C++ generated output to absolute 
	{
		std::string		tmpBuffer = cppGenOutput[MAKEFILE_VERTEXFACTORY];
		L_MakeAbsolutePath( tmpBuffer, cppGenOutput[MAKEFILE_VERTEXFACTORY], makeFileDir );
	}

	// Get vertex factories
	CJsonValue		jsonVertexFactories = jsonDoc.GetValue( "vertex_factories" );
	if ( jsonVertexFactories.IsValid() )
	{
		std::vector<CJsonValue>		jsonArray = jsonVertexFactories.GetArray();
		if ( !jsonVertexFactories.IsA( JSONVALUE_TYPE_ARRAY ) )
		{
			Warning( "CMakeFile: Invalid 'vertex_factories', must be array of objects" );
			bResult = false;
		}
		else
		{
			for ( uint32 index = 0, count = ( uint32 )jsonArray.size(); index < count; ++index )
			{
				const CJsonValue&	jsonElement = jsonArray[index];
				if ( !jsonElement.IsA( JSONVALUE_TYPE_OBJECT ) )
				{
					Warning( "CMakeFile: Invalid 'vertex_factories[%i]', must be object with required fields: 'type' and 'source'", index );
					bResult = false;
					continue;
				}

				// Load vertex factory from the JSON object
				if ( !LoadVertexFactory( jsonElement.GetObject() ) )
				{
					Warning( "CMakeFile: Failed to load vertex factory from 'vertex_factories[%i]'", index );
					bResult = false;
				}
			}
		}
	}

	// Convert all vertex factory's source paths to absolute
	for ( uint32 index = 0; index < VERTEXFACTORY_NUM_TYPES; ++index )
	{
		// Convert source path to absolute
		VertexFactory&		vertexFactory = vertexFactories[index];
		if ( !vertexFactory.source.empty() )
		{
			std::string		tmpBuffer = vertexFactory.source;
			L_MakeAbsolutePath( tmpBuffer, vertexFactory.source, makeFileDir );
		}
	}

	return bResult;
}

/*
==================
CMakeFile::LoadVertexFactory
==================
*/
bool CMakeFile::LoadVertexFactory( const CJsonObject& jsonObject )
{
	bool				bResult = true;
	VertexFactory		vertexFactory;
	EVertexFactoryType	vfType;

	// Get vertex factory type
	CJsonValue	jsonType = jsonObject.GetValue( "type" );
	if ( jsonType.IsValid() )
	{
		if ( !LoadVertexFactoryType( jsonType, vfType ) )
		{
			Warning( "CMakeFile: Failed to load vertex factory type from field 'type'" );
			return false;
		}
	}
	else
	{
		Warning( "CMakeFile: Invalid vertex factory, not found required field 'type'" );
		bResult = false;
	}

	// Get source file
	CJsonValue	jsonSource = jsonObject.GetValue( "source" );
	if ( jsonSource.IsValid() )
	{
		if ( !jsonSource.IsA( JSONVALUE_TYPE_STRING ) )
		{
			Warning( "CMakeFile: Invalid 'source', must be string" );
			bResult = false;
		}
		else
		{
			vertexFactory.source = jsonSource.GetString();
		}
	}
	else
	{
		Warning( "CMakeFile: Invalid vertex factory, not found required field 'source'" );
		bResult = false;
	}

	// Is the vertex factory supports instancing
	CJsonValue	jsonSupportsInstancing = jsonObject.GetValue( "supports_instancing" );
	if ( jsonSupportsInstancing.IsValid() )
	{
		if ( !jsonSupportsInstancing.IsA( JSONVALUE_TYPE_BOOL ) )
		{
			Warning( "CMakeFile: Invalid 'supports_instancing', must be bool" );
			bResult = false;
		}
		else
		{
			vertexFactory.bSupportsInstancing = jsonSupportsInstancing.GetBool();
		}
	}

	// Get instance stream index
	CJsonValue	jsonInstanceStreamIndex = jsonObject.GetValue( "instance_stream_index" );
	if ( jsonInstanceStreamIndex.IsValid() )
	{
		if ( !jsonInstanceStreamIndex.IsA( JSONVALUE_TYPE_INT ) )
		{
			Warning( "CMakeFile: Invalid 'instance_stream_index', must be int" );
			bResult = false;
		}
		else
		{
			vertexFactory.instanceStreamIndex = jsonInstanceStreamIndex.GetInt();
		}
	}

	// Get defines
	CJsonValue		jsonDefines = jsonObject.GetValue( "defines" );
	if ( jsonDefines.IsValid() )
	{
		if ( !jsonDefines.IsA( JSONVALUE_TYPE_ARRAY ) )
		{
			Warning( "CMakeFile: Invalid 'defines', must be array of objects" );
			bResult = false;
		}
		else if ( !LoadDefines( jsonDefines.GetArray(), vertexFactory.defines ) )
		{
			Warning( "CMakeFile: Failed to load defines from 'defines'" );
			bResult = false;
		}
	}

	// Add a new vertex factory
	vertexFactories[( uint32 )vfType] = std::move( vertexFactory );
	return bResult;
}

/*
==================
CMakeFile::LoadVertexFactoryType
==================
*/
bool CMakeFile::LoadVertexFactoryType( const CJsonValue& jsonValue, EVertexFactoryType& vfType )
{
	bool	bResult = true;
	if ( !jsonValue.IsA( JSONVALUE_TYPE_STRING ) )
	{
		Warning( "CMakeFile: Invalid vertex factory type, must be string" );
		bResult = false;
	}
	else if ( !ConvStringToVFType( jsonValue.GetString().c_str(), vfType ) )
	{
		Warning( "CMakeFile: Unknown vertex factory type '%s'", jsonValue.GetString().c_str() );
		bResult = false;
	}

	return bResult;
}

/*
==================
CMakeFile::LoadDefines
==================
*/
bool CMakeFile::LoadDefines( const std::vector<CJsonValue>& jsonArray, std::vector<ShaderDefine>& defines )
{
	bool	bResult = true;
	for ( uint32 index = 0, count = ( uint32 )jsonArray.size(); index < count; ++index )
	{
		const CJsonValue&	jsonElement = jsonArray[index];
		if ( !jsonElement.IsA( JSONVALUE_TYPE_OBJECT ) )
		{
			Warning( "CMakeFile: Invalid define at %i, must be object with required fields: 'name' and 'value'", index );
			bResult = false;
			continue;
		}

		CJsonObject		jsonObject = jsonElement.GetObject();
		ShaderDefine	define;

		// Get define name
		CJsonValue		jsonName = jsonObject.GetValue( "name" );
		if ( jsonName.IsValid() )
		{
			if ( !jsonName.IsA( JSONVALUE_TYPE_STRING ) )
			{
				Warning( "CMakeFile: Invalid 'name' of define at %i', must be string", index );
				bResult = false;
			}
			else
			{
				define.name = jsonName.GetString();
			}
		}
		else
		{
			Warning( "CMakeFile: Invalid define at %i, not found required field 'name'", index );
			bResult = false;
		}

		// Get define value
		CJsonValue		jsonValue = jsonObject.GetValue( "value" );
		if ( jsonValue.IsValid() )
		{
			if ( !jsonValue.IsA( JSONVALUE_TYPE_STRING ) )
			{
				Warning( "CMakeFile: Invalid 'value' of define at %i', must be string", index );
				bResult = false;
			}
			else
			{
				define.value = jsonValue.GetString();
			}
		}
		else
		{
			Warning( "CMakeFile: Invalid define at %i, not found required field 'value'", index );
			bResult = false;
		}

		// Add a new define into array
		defines.emplace_back( define );
	}

	return bResult;
}

/*
==================
CMakeFile::LoadShader
==================
*/
bool CMakeFile::LoadShader( const CJsonObject& jsonObject, Shader& shader )
{
	bool	bResult = true;

	// Get source file
	CJsonValue	jsonSource = jsonObject.GetValue( "source" );
	if ( jsonSource.IsValid() )
	{
		if ( !jsonSource.IsA( JSONVALUE_TYPE_STRING ) )
		{
			Warning( "CMakeFile: Invalid 'source', must be string" );
			bResult = false;
		}
		else
		{
			shader.source = jsonSource.GetString();
		}
	}
	else
	{
		Warning( "CMakeFile: Invalid shader, not found required field 'source'" );
		bResult = false;
	}

	// Get main function
	CJsonValue	jsonMainFunc = jsonObject.GetValue( "main_func" );
	if ( jsonMainFunc.IsValid() )
	{
		if ( !jsonMainFunc.IsA( JSONVALUE_TYPE_STRING ) )
		{
			Warning( "CMakeFile: Invalid 'main_func', must be string" );
			bResult = false;
		}
		else
		{
			shader.mainFunc = jsonMainFunc.GetString();
		}
	}
	else
	{
		Warning( "CMakeFile: Invalid shader, not found required field 'main_func'" );
		bResult = false;
	}

	// Get shader type
	CJsonValue	jsonType = jsonObject.GetValue( "type" );
	if ( jsonType.IsValid() )
	{
		if ( !jsonType.IsA( JSONVALUE_TYPE_STRING ) )
		{
			Warning( "CMakeFile: Invalid 'type', must be string" );
			bResult = false;
		}
		else if ( !ConvStringToShaderType( jsonType.GetString().c_str(), shader.type ) )
		{
			Warning( "CMakeFile: Unknown shader type '%s'", jsonType.GetString().c_str() );
			bResult = false;
		}
	}
	else
	{
		Warning( "CMakeFile: Invalid shader, not found required field 'type'" );
		bResult = false;
	}

	// Is used to this shader vertex factories
	bool		bUseVertexFactory = false;
	CJsonValue	jsonUseVertexFactory = jsonObject.GetValue( "use_vertex_factory" );
	if ( jsonUseVertexFactory.IsValid() )
	{
		if ( !jsonUseVertexFactory.IsA( JSONVALUE_TYPE_BOOL ) )
		{
			Warning( "CMakeFile: Invalid 'use_vertex_factory', must be bool" );
			bResult = false;
		}
		else
		{
			bUseVertexFactory = jsonUseVertexFactory.GetBool();
		}
	}

	// By default reset all vertex factory flags into false
	shader.ResetUseVFs();

	// Get vertex factory types for this shader if we use any VF
	if ( bUseVertexFactory )
	{
		CJsonValue	jsonVertexFactories = jsonObject.GetValue( "vertex_factories" );
		if ( jsonVertexFactories.IsValid() )
		{
			std::vector<CJsonValue>		jsonArray = jsonVertexFactories.GetArray();
			if ( !jsonVertexFactories.IsA( JSONVALUE_TYPE_ARRAY ) )
			{
				Warning( "CMakeFile: Invalid 'vertex_factories', must be array of strings" );
				bResult = false;
			}

			for ( uint32 index = 0, count = ( uint32 )jsonArray.size(); index < count; ++index )
			{
				const CJsonValue&	jsonElement = jsonArray[index];
				if ( !jsonElement.IsA( JSONVALUE_TYPE_STRING ) )
				{
					Warning( "CMakeFile: Invalid 'vertex_factories[%i]', must be string", index );
					bResult = false;
					continue;
				}

				EVertexFactoryType		vfType;
				if ( !LoadVertexFactoryType( jsonElement, vfType ) )
				{
					Warning( "CMakeFile: Failed to load vertex factory type from 'vertex_factories[%i]'", index );
					bResult = false;
					continue;
				}

				// Set usage flag for the vertex factory type
				shader.bUseVertexFactory[( uint32 )vfType] = true;
			}
		}
		else
		{
			// If 'vertex_factories' not set but we have 'use_vertex_factory' = true then use all vertex factory types
			shader.SetUseAllVFs();
		}
	}

	// Get flags
	CJsonValue		jsonFlags = jsonObject.GetValue( "flags" );
	if ( jsonFlags.IsValid() )
	{
		if ( !jsonFlags.IsA( JSONVALUE_TYPE_ARRAY ) )
		{
			Warning( "CMakeFile: Invalid 'flags', must be array of objects" );
			bResult = false;
		}
		else if ( !LoadFlags( jsonFlags.GetArray(), shader.flags ) )
		{
			Warning( "CMakeFile: Failed to load flags from 'flags'" );
			bResult = false;
		}
	}

	// Get defines
	CJsonValue		jsonDefines = jsonObject.GetValue( "defines" );
	if ( jsonDefines.IsValid() )
	{
		if ( !jsonDefines.IsA( JSONVALUE_TYPE_ARRAY ) )
		{
			Warning( "CMakeFile: Invalid 'defines', must be array of objects" );
			bResult = false;
		}
		else if ( !LoadDefines( jsonDefines.GetArray(), shader.defines ) )
		{
			Warning( "CMakeFile: Failed to load defines from 'defines'" );
			bResult = false;
		}
	}

	return bResult;
}

/*
==================
CMakeFile::LoadFlags
==================
*/
bool CMakeFile::LoadFlags( const std::vector<CJsonValue>& jsonArray, std::vector<ShaderFlag>& flags )
{
	bool	bResult = true;
	for ( uint32 index = 0, count = ( uint32 )jsonArray.size(); index < count; ++index )
	{
		const CJsonValue&	jsonElement = jsonArray[index];
		if ( !jsonElement.IsA( JSONVALUE_TYPE_OBJECT ) )
		{
			Warning( "CMakeFile: Invalid flag at %i, must be object with required fields: 'name', 'min' and 'max'", index );
			bResult = false;
			continue;
		}

		CJsonObject		jsonObject = jsonElement.GetObject();
		ShaderFlag		flag;

		// Get flag name
		CJsonValue		jsonName = jsonObject.GetValue( "name" );
		if ( jsonName.IsValid() )
		{
			if ( !jsonName.IsA( JSONVALUE_TYPE_STRING ) )
			{
				Warning( "CMakeFile: Invalid 'name' of flag at %i', must be string", index );
				bResult = false;
			}
			else
			{
				flag.name = jsonName.GetString();
			}
		}
		else
		{
			Warning( "CMakeFile: Invalid flag at %i, not found required field 'name'", index );
			bResult = false;
		}

		// Get minimum value
		CJsonValue		jsonMin = jsonObject.GetValue( "min" );
		if ( jsonMin.IsValid() )
		{
			if ( !jsonMin.IsA( JSONVALUE_TYPE_INT) )
			{
				Warning( "CMakeFile: Invalid 'min' of flag at %i', must be int", index );
				bResult = false;
			}
			else
			{
				flag.min = jsonMin.GetInt();
			}
		}
		else
		{
			Warning( "CMakeFile: Invalid flag at %i, not found required field 'min'", index );
			bResult = false;
		}

		// Get maximum value
		CJsonValue		jsonMax = jsonObject.GetValue( "max" );
		if ( jsonMax.IsValid() )
		{
			if ( !jsonMax.IsA( JSONVALUE_TYPE_INT ) )
			{
				Warning( "CMakeFile: Invalid 'max' of flag at %i', must be int", index );
				bResult = false;
			}
			else
			{
				flag.max = jsonMax.GetInt();
			}
		}
		else
		{
			Warning( "CMakeFile: Invalid flag at %i, not found required field 'max'", index );
			bResult = false;
		}

		// Get default value
		CJsonValue		jsonDefault = jsonObject.GetValue( "default" );
		if ( jsonDefault.IsValid() )
		{
			if ( !jsonDefault.IsA( JSONVALUE_TYPE_INT ) )
			{
				Warning( "CMakeFile: Invalid 'default' of flag at %i', must be int", index );
				bResult = false;
			}
			else
			{
				flag.bSetDefault	= true;
				flag.defaultValue	= jsonDefault.GetInt();
			}
		}

		// Make sure what min and max range is valid. If not we fix it
		if ( flag.min > flag.max )
		{
			Swap( flag.min, flag.max );
		}

		// Make sure what default value in the range
		if ( flag.bSetDefault && ( flag.defaultValue < flag.min || flag.defaultValue > flag.max ) )
		{
			Warning( "CMakeFile: Invalid default value of flag at %i, out of range [%i; %i]", index, flag.min, flag.max );
			bResult = false;
		}

		// Add a new flag into array
		flags.emplace_back( flag );
	}

	return bResult;
}

/*
==================
CMakeFile::Clear
==================
*/
void CMakeFile::Clear()
{
	for ( uint32 index = 0; index < VERTEXFACTORY_NUM_TYPES; ++index )
	{
		vertexFactories[index] = VertexFactory();
	}
	
	for ( uint32 index = 0; index < MAKEFILE_NUM_TYPES; ++index )
	{
		cppGenOutput[index] = DEFAULT_CPPGEN_OUTPUT;
	}

	makeFileDir.clear();
	shaderIncludeDirs.clear();
	shaders.clear();
}