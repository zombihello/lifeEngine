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
#include "core/icommandline.h"
#include "engine/icvar.h"
#include "appframework/iappsystemgroup.h"
#include "tools/shadercompile/makefile.h"
#include "tools/shadercompile/vf_cppclass_generator.h"
#include "tools/shadercompile/shader_cppclass_generator.h"
#include "tools/shadercompile/shadercompile.h"

// Table of vertex factory type names
static const achar* s_pVFTypeNames[] =
{
	"simpleelement"		// VERTEXFACTORY_TYPE_SIMPLEELEMENT
};
static_assert( VERTEXFACTORY_NUM_TYPES == ARRAYSIZE( s_pVFTypeNames ), "Array size 's_pVFTypeNames' must be equal to VERTEXFACTORY_NUM_TYPES" );

// Table of shader type names
static const achar* s_pShaderTypeNames[] =
{
	"vertex",			// STUDIO_SHADER_VERTEX
	"hull",				// STUDIO_SHADER_HULL
	"domain",			// STUDIO_SHADER_DOMAIN
	"geometry",			// STUDIO_SHADER_GEOMETRY
	"pixel",			// STUDIO_SHADER_PIXEL
	"compute"			// STUDIO_SHADER_COMPUTE
};
static_assert( STUDIO_SHADER_NUM_TYPES == ARRAYSIZE( s_pShaderTypeNames ), "Array size 's_pShaderTypeNames' must be equal to STUDIO_SHADER_NUM_TYPES" );


/*
==================
PrintShaderCompileHelp
==================
*/
static void PrintShaderCompileHelp()
{
	Msg( "" );
	Msg( "Shader compile" );
	Msg( "Usage: shadercompile -makefile <path> [-game <path>]" );
	Msg( "Ex: shadercompile -makefile C:/lifeEngine/src/materialsystem/stdshaders/shaderlist.makefile" );
	Msg( "Ex: shadercompile -makefile ../src/materialsystem/gameshaders/shaderlist.makefile -game eleot" );
	Msg( "" );
	Msg( "makefile\t\tPath to a shader make file. For syntax example see src/materialsystem/stdshaders/shaderlist.makefile" );
	Msg( "gencpp-vfs\t\tIs need generate C++ meta types for vertex factories" );
	Msg( "gencpp-shaders\t\tIs need generate C++ helper classes for shaders" );
	Msg( "outcpp-vfs\t\tOverride output path for generated vertex factory C++ meta types (Relative path from makefile)" );
	Msg( "outcpp-shaders\t\tOverride output path for generated shader helper C++ classes (Relative path from makefile)" );
	Msg( "skipcompilation\t\tIs need skip shader compilation" );
	Msg( "game\t\tOptional. Game directory name, it need to set when you compile game specific shaders" );
	Msg( "" );
}

/*
==================
ConvStringToVFType
==================
*/
bool ConvStringToVFType( const achar* pVFTypeName, EVertexFactoryType& vfType )
{
	for ( uint32 index = 0; index < ARRAYSIZE( s_pVFTypeNames ); ++index )
	{
		if ( !L_Stricmp( pVFTypeName, s_pVFTypeNames[index] ) )
		{
			// We found! Return current type
			vfType = ( EVertexFactoryType )index;
			return true;
		}
	}

	// We not found, return invalid type
	vfType = VERTEXFACTORY_NUM_TYPES;
	return false;
}

/*
==================
ConvVFTypeToString
==================
*/
void ConvVFTypeToString( EVertexFactoryType vfType, std::string& vfTypeName )
{
	Assert( vfType < VERTEXFACTORY_NUM_TYPES );
	vfTypeName = s_pVFTypeNames[( uint32 )vfType];
}

/*
==================
ConvStringToShaderType
==================
*/
bool ConvStringToShaderType( const achar* pShaderTypeName, EStudioShaderType& shaderType )
{
	for ( uint32 index = 0; index < ARRAYSIZE( s_pShaderTypeNames ); ++index )
	{
		if ( !L_Stricmp( pShaderTypeName, s_pShaderTypeNames[index] ) )
		{
			// We found! Return current type
			shaderType = ( EStudioShaderType )index;
			return true;
		}
	}

	// We not found, return invalid type
	shaderType = STUDIO_SHADER_NUM_TYPES;
	return false;
}

/*
==================
ConvShaderTypeToString
==================
*/
void ConvShaderTypeToString( EStudioShaderType shaderType, std::string& shaderTypeName )
{
	Assert( shaderType < STUDIO_SHADER_NUM_TYPES );
	shaderTypeName = s_pShaderTypeNames[( uint32 )shaderType];
}


/**
 * @ingroup shadercompile
 * @brief Shader compile application
 */
class CShaderCompileApp : public CDefaultAppSystemGroup<CAppSystemGroup>
{
public:
	/**
	 * @brief An installed application creation function, you should tell the group
	 * the DLLs and the singleton interfaces you want to instantiate.
	 *
	 * @return Return FALSE if there's any problems and the app will abort
	 */
	virtual bool Create() override;

	/**
	 * @brief Allow the application to do some work after AppSystems are connected
	 *
	 * Allow the application to do some work after AppSystems are connected but
	 * they aren't all Initialized
	 *
	 * @return Return FALSE if there's any problems and the app will abort
	 */
	virtual bool PreInit() override;

	/**
	 * @brief Main loop implemented by the application
	 * @return Return exit code. If all ok returns zero
	 */
	virtual int32 Main() override;

	/**
	 * @brief Allow the application to do some work after all AppSystems are shut down
	 */
	virtual void PostShutdown() override;

private:
	/**
	 * @brief Generate C++ meta types for vertex factories
	 * @return Return Return TRUE if all C++ meta type for vertex factories are generated, otherwise returns FALSE
	 */
	bool GenerateVFsCppMetaTypes();

	/**
	 * @brief Generate helper C++ classes for shaders
	 * @return Return Return TRUE if all C++ shader classes are generated, otherwise returns FALSE
	 */
	bool GenerateShaderCppClass();

	CMakeFile	makefile;	/**< Shader makefile */
};


/*
==================
CShaderCompileApp::Create
==================
*/
bool CShaderCompileApp::Create()
{
	// Load application systems
	AppSystemInfo		appSystemInfos[] =
	{
		{ "engine" DLL_EXT_STRING,			CVAR_QUERY_INTERFACE_VERSION		},	// This one must be first
		{ "filesystem" DLL_EXT_STRING,		FILESYSTEM_INTERFACE_VERSION		},
		{ "engine" DLL_EXT_STRING,			CVAR_INTERFACE_VERSION				},
		{ "", "" }																	// Required to terminate the list
	};

	// Add all systems from array
	if ( !AddSystems( appSystemInfos ) )
	{
		return false;
	}

	// We are done
	return true;
}

/*
==================
CShaderCompileApp::PreInit
==================
*/
bool CShaderCompileApp::PreInit()
{
	return ConnectStdLib( GetFactory() );
}

/*
==================
CShaderCompileApp::Main
==================
*/
int32 CShaderCompileApp::Main()
{
	// Is need to print help of usage
	bool			bPrintHelpUsage = CommandLine()->HasParam( "h" ) || CommandLine()->HasParam( "help" ) || CommandLine()->HasParam( "?" );

	// Get and parse a shader make file
	const achar*	pMakeFilePath = CommandLine()->GetFirstValue( "makefile" );
	bool			bInvalidMakeFilePath = !pMakeFilePath || pMakeFilePath[0] == '\0';
	if ( !bInvalidMakeFilePath && !makefile.LoadMakeFile( pMakeFilePath ) )
	{
		Error( "ShaderCompile: Failed to load makefile '%s'", pMakeFilePath );
		return 1;
	}

	// Print help of usage if it need or some parameters aren't set
	if ( bPrintHelpUsage || bInvalidMakeFilePath )
	{
		PrintShaderCompileHelp();
		return 0;
	}

	// Override output path for generated vertex factory C++ meta types
	if ( CommandLine()->HasParam( "outcpp-vfs" ) )
	{
		std::string		outputCppGenVFs;
		L_MakeAbsolutePath( CommandLine()->GetFirstValue( "outcpp-vfs" ), outputCppGenVFs, makefile.GetMakeFileDir(), false );
		makefile.SetCppGenOutput( MAKEFILE_VERTEXFACTORY, outputCppGenVFs.c_str() );
	}

	// Override output path of shader C++ helper classes
	if ( CommandLine()->HasParam( "outcpp-shaders" ) )
	{
		std::string		outputCppGenShaders;
		L_MakeAbsolutePath( CommandLine()->GetFirstValue( "outcpp-shaders" ), outputCppGenShaders, makefile.GetMakeFileDir(), false );
		makefile.SetCppGenOutput( MAKEFILE_SHADER, outputCppGenShaders.c_str() );
	}

	// Generate C++ meta types for vertex factories
	if ( CommandLine()->HasParam( "gencpp-vfs" ) )
	{
		if ( !GenerateVFsCppMetaTypes() )
		{
			return 1;
		}

		
	}

	// Generate C++ helper classes for shaders
	if ( CommandLine()->HasParam( "gencpp-shaders" ) )
	{
		if ( !GenerateShaderCppClass() )
		{
			return 1;
		}
	}

	// We are done if no need do compilation of shaders
	if ( CommandLine()->HasParam( "skipcompilation" ) )
	{
		return 0;
	}

	// We are done
	return 0;
}

/*
==================
CShaderCompileApp::GenerateVFsCppMetaTypes
==================
*/
bool CShaderCompileApp::GenerateVFsCppMetaTypes()
{
	bool						bResult = true;
	CVFCppClassGenerator		vfCppClassGenerator;
	Msg( "ShaderCompile: Generate C++ meta types for vertex factories" );

	for ( uint32 index = 0; index < VERTEXFACTORY_NUM_TYPES; ++index )
	{
		const VertexFactory&	vertexFactory = makefile.GetVertexFactory( ( EVertexFactoryType )index );
		if ( makefile.IsValidVertexFactory( ( EVertexFactoryType )index ) )
		{
			// Generate C++ class for this vertex factory
			vfCppClassGenerator.Reset();
			vfCppClassGenerator.Generate( vertexFactory, ( EVertexFactoryType )index );
			const std::string&		buffer = vfCppClassGenerator.GetBuffer();

			// Generate file path from vertex factory type
			std::string		vfTypeName;
			std::string		filePath;
			{
				// Convert vertex factory type to string
				ConvVFTypeToString( ( EVertexFactoryType )index, vfTypeName );

				// Get file path
				filePath = L_Sprintf( "%s/vf_%s.gen.h", makefile.GetCppGenOutput( MAKEFILE_VERTEXFACTORY ).c_str(), vfTypeName.c_str() );
				L_FixPathSeparators( filePath );
			}

			// Save buffer into file
			TRefPtr<IFileWriter>	file = g_pFileSystem->CreateFileWriter( filePath.c_str() );
			if ( file )
			{
				file->Write( ( void* )buffer.data(), buffer.size() * sizeof( achar ) );
				Msg( "ShaderCompile: C++ meta type for '%s' saved to '%s'", vfTypeName.c_str(), filePath.c_str() );
			}
			else
			{
				Error( "ShaderCompile: Failed to save C++ meta type for '%s' to '%s'", vfTypeName.c_str(), filePath.c_str() );
				bResult = false;
			}
		}
	}

	return bResult;
}

/*
==================
CShaderCompileApp::GenerateShaderCppClass
==================
*/
bool CShaderCompileApp::GenerateShaderCppClass()
{
	bool							bResult = true;
	CShaderCppClassGenerator		shaderCppClassGenerator;
	const std::vector<Shader>&		shaders = makefile.GetShaders();	
	Msg( "ShaderCompile: Generate C++ class for each shader" );

	for ( uint32 index = 0, count = ( uint32 )shaders.size(); index < count; ++index )
	{
		// Generate C++ class for this shader
		const Shader&			shader = shaders[index];
		shaderCppClassGenerator.Reset();
		shaderCppClassGenerator.Generate( shader );
		const std::string&		buffer = shaderCppClassGenerator.GetBuffer();

		// Generate file path from base name of shader source file and the one type
		std::string		shaderName;
		std::string		filePath;
		{
			// Convert shader type to string
			std::string		shaderTypeName;
			ConvShaderTypeToString( shader.type, shaderTypeName );

			// Get shader name from source file name
			L_GetFileBaseName( shader.source, shaderName, false );

			// Get file path
			filePath = L_Sprintf( "%s/%s_%s.gen.h", makefile.GetCppGenOutput( MAKEFILE_SHADER ).c_str(), shaderName.c_str(), shaderTypeName.c_str() );
			L_FixPathSeparators( filePath );
		}

		// Save buffer into file
		TRefPtr<IFileWriter>	file = g_pFileSystem->CreateFileWriter( filePath.c_str() );
		if ( file )
		{
			file->Write( ( void* )buffer.data(), buffer.size() * sizeof( achar ) );
			Msg( "ShaderCompile: C++ class for '%s' saved to '%s'", shaderName.c_str(), filePath.c_str() );
		}
		else
		{
			Error( "ShaderCompile: Failed to save C++ class for '%s' to '%s'", shaderName.c_str(), filePath.c_str() );
			bResult = false;
		}
	}

	return bResult;
}

/*
==================
CShaderCompileApp::PostShutdown
==================
*/
void CShaderCompileApp::PostShutdown()
{
	DisconnectStdLib();
}


/*
==================
main
==================
*/
int main( int argc, char** argv )
{
	// Initialize command line
	{
		std::string		arguments;
		for ( uint32 index = 0; index < ( uint32 )argc; ++index )
		{
			if ( *argv[index] == '-' || *argv[index] == '/' )
			{
				arguments += argv[index];
				arguments += " ";
			}
			else
			{
				arguments += L_Sprintf( "\"%s\" ", argv[index] );
			}
		}
		CommandLine()->Init( arguments.c_str() );
	}

	// Run application
	CShaderCompileApp		shaderCompileApp;
	return shaderCompileApp.Run();
}