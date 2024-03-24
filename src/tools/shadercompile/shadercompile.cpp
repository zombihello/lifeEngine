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
#include "libs/shadercache/shadercache.h"
#include "tools/shadercompile/makefile.h"
#include "tools/shadercompile/vf_cppclass_generator.h"
#include "tools/shadercompile/shader_cppclass_generator.h"
#include "tools/shadercompile/shadercompile.h"
#include "tools/shadercompile/ishadercompile_backend.h"
#include "tools/shadercompile/shadercompile_environment.h"
#include "tools/shadercompile/shadercompile_output.h"

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
	Msg( "Usage: shadercompile -makefile <path> -backend <name> [-game <path>]" );
	Msg( "Ex: shadercompile -makefile C:/lifeEngine/src/materialsystem/stdshaders/shaderlist.makefile -backend shadercompile_dx11" );
	Msg( "Ex: shadercompile -makefile ../src/materialsystem/gameshaders/shaderlist.makefile -game eleot -backend shadercompile_dx11" );
	Msg( "" );
	Msg( "makefile\t\tPath to a shader make file. For syntax example see src/materialsystem/stdshaders/shaderlist.makefile" );
	Msg( "backend\t\tShader compile backend. When command line has skipcompilation then this is ignoring (Backend examples: shadercompile_dx11, shadercompile_vulkan, etc)" );
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
void ConvVFTypeToString( EVertexFactoryType vfType, const achar*& pVFTypeName )
{
	Assert( vfType < VERTEXFACTORY_NUM_TYPES );
	pVFTypeName = s_pVFTypeNames[( uint32 )vfType];
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
void ConvShaderTypeToString( EStudioShaderType shaderType, const achar*& pShaderTypeName )
{
	Assert( shaderType < STUDIO_SHADER_NUM_TYPES );
	pShaderTypeName = s_pShaderTypeNames[( uint32 )shaderType];
}


/**
 * @ingroup shadercompile
 * @brief Shader compile application
 */
class CShaderCompileApp : public CDefaultAppSystemGroup<CAppSystemGroup>
{
public:
	/**
	 * @brief Constructor
	 */
	CShaderCompileApp();

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
	 * @return Return TRUE if all C++ meta type for vertex factories are generated, otherwise returns FALSE
	 */
	bool GenerateVFsCppMetaTypes();

	/**
	 * @brief Generate helper C++ classes for shaders
	 * @return Return TRUE if all C++ shader classes are generated, otherwise returns FALSE
	 */
	bool GenerateShaderCppClass();

	/**
	 * @brief Compile shaders
	 * @return Return TRUE if all shaders were compiled, otherwise returns FALSE
	 */
	bool CompileShaders();

	/**
	 * @brief Compile shader
	 * 
	 * @param shaderCacheDoc	Shader cache document
	 * @param shader			Shader to compile
	 * @param pVertexFactory	Vertex factory. NULL means what we don't use any vertex factory
	 * @return Return TRUE if all shaders were compiled, otherwise returns FALSE
	 */
	bool CompileShader( CShaderCacheDoc& shaderCacheDoc, const Shader& shader, VertexFactory* pVertexFactory = nullptr );

	/**
	 * @brief Load backend
	 * @return Return TRUE if backend was loaded, otherwise returns FALSE
	 */
	bool LoadBackend();

	/**
	 * @brief Unload backend
	 */
	void UnloadBackend();

	CMakeFile				makefile;				/**< Shader makefile */
	dllHandle_t				backendHandle;			/**< Backend module handle */
	IShaderCompileBackend*	pShaderCompileBackend;	/**< Shader compile backend */
};


/*
==================
CShaderCompileApp::CShaderCompileApp
==================
*/
CShaderCompileApp::CShaderCompileApp()
	: backendHandle( nullptr )
	, pShaderCompileBackend( nullptr )
{}

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
		{ "engine"		DLL_EXT_STRING,			CVAR_QUERY_INTERFACE_VERSION		},	// This one must be first
		{ "filesystem"	DLL_EXT_STRING,			FILESYSTEM_INTERFACE_VERSION		},
		{ "engine"		DLL_EXT_STRING,			CVAR_INTERFACE_VERSION				},
		{ "", "" }																		// Required to terminate the list
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

	// Make sure what we have valid the backend (if we haven't 'skipcompilation')
	const achar*	pBackendName = CommandLine()->GetFirstValue( "backend" );
	bool			bInvalidBackend = !CommandLine()->HasParam( "skipcompilation" ) && ( !pBackendName || pBackendName[0] == '\0' );

	// Print help of usage if it need or some parameters aren't set
	if ( bPrintHelpUsage || bInvalidMakeFilePath || bInvalidBackend )
	{
		PrintShaderCompileHelp();
		return 0;
	}

	// Set path to game directory if -game is set
	if ( CommandLine()->HasParam( "game" ) )
	{
		g_pFileSystem->AddSearchPath( CommandLine()->GetFirstValue( "game" ), "GAME" );
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

	// Now load backend for compilation
	if ( !LoadBackend() )
	{
		return 1;
	}

	// Compile shaders
	if ( !CompileShaders() )
	{
		return 1;
	}

	// We are done
	UnloadBackend();
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
			const achar*	pVFTypeName;
			std::string		filePath;
			{
				// Convert vertex factory type to string
				ConvVFTypeToString( ( EVertexFactoryType )index, pVFTypeName );

				// Get file path
				filePath = L_Sprintf( "%s/vf_%s.gen.h", makefile.GetCppGenOutput( MAKEFILE_VERTEXFACTORY ).c_str(), pVFTypeName );
				L_FixPathSeparators( filePath );
			}

			// Save buffer into file
			TRefPtr<IFileWriter>	file = g_pFileSystem->CreateFileWriter( filePath.c_str() );
			if ( file )
			{
				file->Write( ( void* )buffer.data(), buffer.size() * sizeof( achar ) );
				Msg( "ShaderCompile: C++ meta type for '%s' saved to '%s'", pVFTypeName, filePath.c_str() );
			}
			else
			{
				Error( "ShaderCompile: Failed to save C++ meta type for '%s' to '%s'", pVFTypeName, filePath.c_str() );
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
			const achar*	pShaderTypeName;
			ConvShaderTypeToString( shader.type, pShaderTypeName );

			// Get shader name from source file name
			L_GetFileBaseName( shader.source, shaderName, false );

			// Get file path
			filePath = L_Sprintf( "%s/%s_%s.gen.h", makefile.GetCppGenOutput( MAKEFILE_SHADER ).c_str(), shaderName.c_str(), pShaderTypeName );
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
CShaderCompileApp::CompileShaders
==================
*/
bool CShaderCompileApp::CompileShaders()
{
	// Iterate over shaders and each the one compile for all flag combination
	bool						bResult = true;
	const std::vector<Shader>&	shaders = makefile.GetShaders();
	const achar*				pOutputShaderCacheDir = CommandLine()->HasParam( "game" ) ? "//GAME/shaders/" : "//ENGINE/shaders";
	for ( uint32 index = 0, count = ( uint32 )shaders.size(); index < count; ++index )
	{
		const Shader&		shader = shaders[index];
		CShaderCacheDoc		shaderCacheDoc;
		shaderCacheDoc.SetShaderType( shader.type );

		// Compile the shader for each vertex factories (which set for this shader in makefile) if the one use VFs
		for ( uint32 vfIdx = 0; vfIdx < VERTEXFACTORY_NUM_TYPES; ++vfIdx )
		{
			if ( shader.bUseVertexFactory[vfIdx] )
			{
				shaderCacheDoc.SetUseVertexFactory( true );
				const VertexFactory&	vertexFactory		= makefile.GetVertexFactory( ( EVertexFactoryType )vfIdx );
				const achar*			pVertexFactoryName	= "";
				ConvVFTypeToString( ( EVertexFactoryType )vfIdx, pVertexFactoryName );
				Msg( "ShaderCompile: Compile shader '%s' for vertex factory '%s' (%i combos)", shader.source.c_str(), pVertexFactoryName, shader.numFlagCombos );		
				
				if ( !CompileShader( shaderCacheDoc, shader, ( VertexFactory* )&vertexFactory ) )
				{
					bResult = false;
					continue;
				}
			}
		}

		// Otherwise we simple compile shader without any vertex factories
		if ( !shaderCacheDoc.IsUseVertexFactory() )
		{
			Msg( "ShaderCompile: Compile shader '%s' (%i combos)", shader.source.c_str(), shader.numFlagCombos );
			if ( !CompileShader( shaderCacheDoc, shader ) )
			{
				bResult = false;
				continue;
			}
		}

		// Save shader cache document	
		std::string		baseShaderName;
		L_GetFileBaseName( shader.source, baseShaderName, false );
		std::string		fullPath = L_Sprintf( "%s/%s.ssc", pOutputShaderCacheDir, baseShaderName.c_str() );
		if ( !shaderCacheDoc.Save( fullPath.c_str() ) )
		{
			bResult = false;
			Error( "ShaderCompile: Failed to save shader cache to '%s'", fullPath.c_str() );
			continue;
		}
	}

	return bResult;
}

/*
==================
CShaderCompileApp::CompileShader
==================
*/
bool CShaderCompileApp::CompileShader( CShaderCacheDoc& shaderCacheDoc, const Shader& shader, VertexFactory* pVertexFactory /* = nullptr */ )
{
	std::vector<int32>				flagVarSlots( shader.flags.size() );

	// Set all the variables to min values
    int32*		pFlagVarSlotBegin   = flagVarSlots.data();
    int32*		pFlagVarSlotEnd     = pFlagVarSlotBegin + ( uint32 )flagVarSlots.size();
    int32*		pSetFlagVar         = nullptr;
	ShaderFlag*	pSetFlag			= nullptr;

	for ( pSetFlagVar = pFlagVarSlotBegin, pSetFlag = ( ShaderFlag* )shader.flags.data(); pSetFlagVar < pFlagVarSlotEnd; ++pSetFlagVar, ++pSetFlag )
	{
		*pSetFlagVar = pSetFlag->min;
	}

	bool							bResult		= true;
	const std::vector<std::string>&	includeDirs = makefile.GetShaderIncludeDirs();
	uint32							currentCombo = 0;
	while ( currentCombo < shader.numFlagCombos )
	{
		++currentCombo;

		// Compile shader
		CShaderCompileOutput		shaderCompileOutput;
		CShaderCompileEnvironment	shaderCompileEnvironment( shader.type );

		// Add to the environment all include directories
		for ( uint32 includeDirIdx = 0, numIncludeDirs = ( uint32 )includeDirs.size(); includeDirIdx < numIncludeDirs; ++includeDirIdx )
		{
			shaderCompileEnvironment.AddIncludeDir( includeDirs[includeDirIdx].c_str() );
		}

		// Add to the environment all defines
		for ( uint32 defineIdx = 0, numDefines = ( uint32 )shader.defines.size(); defineIdx < numDefines; ++defineIdx )
		{
			const ShaderDefine&		shaderDefine = shader.defines[defineIdx];
			shaderCompileEnvironment.AddDefine( shaderDefine.name.c_str(), shaderDefine.value.c_str() );
		}

		// Add to the environment current flags
		for ( pSetFlagVar = pFlagVarSlotBegin, pSetFlag = ( ShaderFlag* )shader.flags.data(); pSetFlagVar < pFlagVarSlotEnd; ++pSetFlagVar, ++pSetFlag )
		{
			shaderCompileEnvironment.AddDefine( pSetFlag->name.c_str(), L_Sprintf( "%i", *pSetFlagVar ).c_str() );
		}

		// If we have a vertex factory then setup the environment for it
		if ( pVertexFactory )
		{
			shaderCompileEnvironment.SetVertexFactoryFileName( pVertexFactory->source.c_str() );
			for ( uint32 index = 0, count = ( uint32 )pVertexFactory->defines.size(); index < count; ++index )
			{
				const ShaderDefine&		shaderDefine = pVertexFactory->defines[index];
				shaderCompileEnvironment.AddDefine( shaderDefine.name.c_str(), shaderDefine.value.c_str() );
			}
		}

		// Compile shader
		bool	bCompileResult = pShaderCompileBackend->CompileShader( shader.source.c_str(), shader.mainFunc.c_str(), shader.type, &shaderCompileEnvironment, &shaderCompileOutput );
		if ( !bCompileResult )
		{
			Error( "ShaderCompile: Failed to compile shader '%s' at %i combo", shader.source.c_str(), currentCombo );
			if ( !shaderCompileOutput.GetErrorMsg().empty() )
			{
				Error( "ShaderCompile: Message:\n%s", shaderCompileOutput.GetErrorMsg().c_str() );
			}
			else
			{
				Error( "ShaderCompile: Message: <None>" );
			}
			Error( "ShaderCompile: ------------ ENVIRONMENT INFO --------------" );

			// Print includes
			Error( "ShaderCompile: Includes:" );
			for ( uint32 includeIdx = 0, numIncludes = shaderCompileEnvironment.GetNumIncludeDirs(); includeIdx < numIncludes; ++includeIdx )
			{
				Error( "ShaderCompile:\t %s", shaderCompileEnvironment.GetIncludeDir( includeIdx ) );
			}
			Error( "ShaderCompile:" );

			// Print defines
			Error( "ShaderCompile: Defines:" );
			for ( uint32 defineIdx = 0, numDefines = shaderCompileEnvironment.GetNumDefines(); defineIdx < numDefines; ++defineIdx )
			{
				const achar*	pDefineName = nullptr;
				const achar*	pDefineValue = nullptr;
				shaderCompileEnvironment.GetDefine( defineIdx, pDefineName, pDefineValue );
				Error( "ShaderCompile:\t %s: %s", pDefineName, pDefineValue );
			}

			// Print combinations
			if ( shader.numFlagCombos > 1 )
			{
				Error( "ShaderCompile:" );
				Error( "ShaderCompile: Current combo:" );
				for ( pSetFlagVar = pFlagVarSlotBegin, pSetFlag = ( ShaderFlag* )shader.flags.data(); pSetFlagVar < pFlagVarSlotEnd; ++pSetFlagVar, ++pSetFlag )
				{
					Error( "ShaderCompile:\t %s: %i", pSetFlag->name.c_str(), *pSetFlagVar );
				}
			}

			Error( "ShaderCompile: --------------------------------------------" );
			bResult = false;
		}

		// If shader was compiled add output data into shader cache
		if ( bCompileResult )
		{
			shaderCacheDoc.AddShaderCache( shaderCompileOutput.GetShaderCache(), pVertexFactory ? pVertexFactory->type : ( EVertexFactoryType )-1 );
		}

		// Do a next iteration
		for ( pSetFlagVar = pFlagVarSlotBegin, pSetFlag = ( ShaderFlag* )shader.flags.data(); pSetFlagVar < pFlagVarSlotEnd; ++pSetFlagVar, ++pSetFlag )
		{
			if ( ++*pSetFlagVar <= pSetFlag->max )
			{
				break;
			}

			*pSetFlagVar = pSetFlag->min;
		}
	}

	return bResult;
}

/*
==================
CShaderCompileApp::LoadBackend
==================
*/
bool CShaderCompileApp::LoadBackend()
{
	// Unload old backend
	UnloadBackend();

	// Get path to backend
	std::string		backendPath = L_Sprintf( "//ENGINEBIN/%s" DLL_EXT_STRING, CommandLine()->GetFirstValue( "backend" ) );

	// Load module
	dllHandle_t		backendHandle = g_pFileSystem->LoadModule( backendPath.c_str() );
	if ( !backendHandle )
	{
		Warning( "ShaderCompile: Failed to load backend '%s'", backendPath.c_str() );
		return false;
	}

	// Get at the backend interface
	CreateInterfaceFn_t		pFactory = Sys_GetFactory( backendHandle );
	if ( !pFactory )
	{
		Warning( "ShaderCompile: Could not find factory interface in '%s'", backendPath.c_str() );
		g_pFileSystem->UnloadModule( backendHandle );
		return false;
	}

	IShaderCompileBackend*	pShaderCompileBackend = ( IShaderCompileBackend* )pFactory( SHADERCOMPILEBACKEND_INTERFACE_VERSION );
	if ( !pShaderCompileBackend )
	{
		Warning( "ShaderCompile: Could not get IShaderCompileBackend interface from '%s'", backendPath.c_str() );
		g_pFileSystem->UnloadModule( backendHandle );
		return false;
	}

	// Allow the backend to try to connect to interfaces it needs
	if ( !pShaderCompileBackend->Connect( GetFactory() ) )
	{
		Warning( "ShaderCompile: Failed to init backend '%s'", backendPath.c_str() );
		g_pFileSystem->UnloadModule( backendHandle );
		return false;
	}

	// We are done
	Msg( "ShaderCompile: Backend '%s' loaded", backendPath.c_str() );
	CShaderCompileApp::backendHandle			= backendHandle;
	CShaderCompileApp::pShaderCompileBackend	= pShaderCompileBackend;
	return true;
}

/*
==================
CShaderCompileApp::UnloadBackend
==================
*/
void CShaderCompileApp::UnloadBackend()
{
	if ( backendHandle || pShaderCompileBackend )
	{
		// Shutdown the backend
		if ( pShaderCompileBackend )
		{
			pShaderCompileBackend->Disconnect();
		}

		// Unload shader module
		if ( backendHandle )
		{
			g_pFileSystem->UnloadModule( backendHandle );
		}

		Msg( "ShaderCompile: Backend unloaded" );
		backendHandle = nullptr;
		pShaderCompileBackend = nullptr;
	}
}

/*
==================
CShaderCompileApp::PostShutdown
==================
*/
void CShaderCompileApp::PostShutdown()
{
	UnloadBackend();
	DisconnectStdLib();
}


/*
==================
Shader::CalcNumFlagCombos
==================
*/
void Shader::CalcNumFlagCombos()
{
	numFlagCombos = 1;
	for ( uint32 index = 0, count = ( uint32 )flags.size(); index < count; ++index )
	{
		const ShaderFlag&		flag = flags[index];
		numFlagCombos *= flag.max - flag.min + 1;
	}
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