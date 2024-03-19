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

#include "pch_materialsystem.h"
#include "filesystem/ifilesystem.h"
#include "materialsystem/ishadersystem.h"
#include "materialsystem/ishader.h"
#include "materialsystem/vertexfactory_base.h"
#include "materialsystem/materialsystem.h"

// Array of registered vertex factories in the system
static CreateVertexFactoryFn_t s_pCreateVFFns[] =
{
	nullptr,		// VERTEXFACTORY_TYPE_SIMPLEELEMENT
};
static_assert( VERTEXFACTORY_NUM_TYPES == ARRAYSIZE( s_pCreateVFFns ), "Array size 's_pCreateVFFns' must be equal to VERTEXFACTORY_NUM_TYPES" );

/**
 * @ingroup materialsystem
 * @brief Information about shader library
 */
struct ShaderLibInfo
{
	std::string										fileName;		/**< File name */
	dllHandle_t										moduleHandle;	/**< Module handle */
	IShaderLib*										pShaderLib;		/**< Shader library */
	bool											bGameShaderLib;	/**< TRUE if this is a game's shader library, in which case it's not allowed to override any existing shader names */
	std::unordered_map<const achar*, IShader*>		shaderDict;		/**< Shaders in this library */
};

/**
 * @ingroup materialsystem
 * @brief Shader system
 */
class CShaderSystem : public IShaderSystem
{
public:
	/**
	 * @brief Initialize
	 * This function initialize the system and loads all engine shader DLLs
	 */
	virtual void Init() override;

	/**
	 * @brief Shutdown
	 * This function shutdown the system and unloads all engine shader DLLs
	 */
	virtual void Shutdown() override;

	/**
	 * @brief Initialize for the game
	 * This function loads all game specific shader DLLs
	 */
	virtual void GameInit() override;

	/**
	 * @brief Shutdown for the game
	 * This function unloads all game specific shader DLLs
	 */
	virtual void GameShutdown() override;

	/**
	 * @brief Create vertex factory instance for vfType
	 *
	 * @param vfType	Vertex factory type (see EVertexFactoryType)
	 * @return Return a new vertex factory instance. If this type isn't implemented by the system returns NULL
	 */
	virtual TRefPtr<IVertexFactory> CreateVertexFactory( EVertexFactoryType vfType ) const override;

private:
	/**
	 * @brief Load shader library
	 * 
	 * @param pPath				Path to shader library module
	 * @param bGameShaderLib	Is this game's shader library
	 * @return Return TRUE if shader library is loaded, otherwise returns FALSE
	 */
	bool LoadShaderLib( const achar* pPath, bool bGameShaderLib = false );

	/**
	 * @brief Unload shader library
	 * @param index		Shader library index
	 */
	void UnloadShaderLib( uint32 index );

	/**
	 * @brief Sets up the shader dictionary for a library
	 * @@param index		Shader library index
	 */
	void SetupShaderDictionary( uint32 index );

	/**
	 * @brief Find shader library by module path
	 * 
	 * @param pPath		Path to shader library module
	 * @return Return shader library index, if not found returns INDEX_NONE
	 */
	uint32 FindShaderLib( const achar* pPath ) const;

	/**
	 * @brief Load engine's shader libraries
	 */
	void LoadShaderLibs();

	/**
	 * @brief Unload engine's shader libraries
	 */
	void UnloadShaderLibs();

	/**
	 * @brief Load game's shader libraries
	 */
	void LoadGameShaderLibs();

	/**
	 * @brief Unload game's shader libraries
	 */
	void UnloadGameShaderLibs();

	std::vector<ShaderLibInfo>		shaderLibs;		/**< Loaded shader libraries */
};

EXPOSE_SINGLE_INTERFACE( CShaderSystem, IShaderSystem, SHADERSYSTEM_INTERFACE_VERSION );


/*
==================
RegisterVFType
==================
*/
void RegisterVFType( EVertexFactoryType vfType, CreateVertexFactoryFn_t pCreateVertexFactoryFn )
{
	// We cannot register one vertex factory type twice
	if ( s_pCreateVFFns[( uint32 )vfType] )
	{
		Error( "CShaderSystem: Cannot register vertex factory type 0x%X twice!", vfType );
	}

	s_pCreateVFFns[( uint32 )vfType] = pCreateVertexFactoryFn;
	Msg( "CShaderSystem: Registered Vertex factory type 0x%X", vfType );
}


/*
==================
CShaderSystem::Init
==================
*/
void CShaderSystem::Init()
{
	// Load all engine's shader libraries
	LoadShaderLibs();
}

/*
==================
CShaderSystem::Shutdown
==================
*/
void CShaderSystem::Shutdown()
{
	// Unload all engine's shader libraries
	UnloadShaderLibs();
}

/*
==================
CShaderSystem::GameInit
==================
*/
void CShaderSystem::GameInit()
{
	// Load game's shader libraries
	LoadGameShaderLibs();
}

/*
==================
CShaderSystem::GameShutdown
==================
*/
void CShaderSystem::GameShutdown()
{
	// Unload game's shader libraries
	UnloadGameShaderLibs();
}

/*
==================
CShaderSystem::LoadShaderLib
==================
*/
bool CShaderSystem::LoadShaderLib( const achar* pPath, bool bGameShaderLib /* = false */ )
{
	// Load shader module
	dllHandle_t		shaderModule = g_pFileSystem->LoadModule( pPath );
	if ( !shaderModule )
	{
		Warning( "CShaderSystem: Failed to load shader library '%s'", pPath );
		return false;
	}

	// Get at the shader library interface
	CreateInterfaceFn_t		pFactory = Sys_GetFactory( shaderModule );
	if ( !pFactory )
	{
		Warning( "CShaderSystem: Could not find factory interface in '%s'", pPath );
		g_pFileSystem->UnloadModule( shaderModule );
		return false;
	}

	IShaderLib*				pShaderLib = ( IShaderLib* )pFactory( SHADERLIB_INTERFACE_VERSION );
	if ( !pShaderLib )
	{
		Warning( "CShaderSystem: Could not get IShaderLib interface from '%s'", pPath );
		g_pFileSystem->UnloadModule( shaderModule );
		return false;
	}

	// Allow the Lib to try to connect to interfaces it needs
	if ( !pShaderLib->Connect( g_pAppSystemFactory ) )
	{
		Warning( "CShaderSystem: Failed to init shader library '%s'", pPath );
		g_pFileSystem->UnloadModule( shaderModule );
		return false;
	}

	// Now replace any existing shader
	uint32			shaderLibIndex = FindShaderLib( pPath );
	ShaderLibInfo*	pShaderLibInfo = nullptr;
	if ( shaderLibIndex != INDEX_NONE )
	{
		UnloadShaderLib( shaderLibIndex );
		pShaderLibInfo = &shaderLibs[shaderLibIndex];
	}
	else
	{
		shaderLibIndex = shaderLibs.size();
		pShaderLibInfo = &shaderLibs.emplace_back();
	}

	// Ok, the shader library good!
	pShaderLibInfo->fileName		= pPath;
	pShaderLibInfo->moduleHandle	= shaderModule;
	pShaderLibInfo->pShaderLib		= pShaderLib;
	pShaderLibInfo->bGameShaderLib	= bGameShaderLib;

	// Add the shaders to the dictionary of shaders
	SetupShaderDictionary( shaderLibIndex );

	// We are done!
	Msg( "CShaderSystem: Shader library '%s' loaded", pPath );
	return true;
}

/*
==================
CShaderSystem::UnloadShaderLib
==================
*/
void CShaderSystem::UnloadShaderLib( uint32 index )
{
	// Do nothing if index isn't valid
	if ( index == INDEX_NONE )
	{
		return;
	}
	ShaderLibInfo&		info = shaderLibs[index];
	
	// Shutdown the shader library
	if ( info.pShaderLib )
	{
		info.pShaderLib->Disconnect();
	}

	// Unload shader module
	if ( info.moduleHandle )
	{
		g_pFileSystem->UnloadModule( info.moduleHandle );
	}

	Msg( "CShaderSystem: Shader library '%s' unloaded", info.fileName.c_str() );
	info.fileName.clear();
	info.shaderDict.clear();
	info.pShaderLib		= nullptr;
	info.moduleHandle	= nullptr;
}

/*
==================
CShaderSystem::SetupShaderDictionary
==================
*/
void CShaderSystem::SetupShaderDictionary( uint32 index )
{
	ShaderLibInfo&		info = shaderLibs[index];
	uint32				numShaders = info.pShaderLib->GetNumShaders();
	for ( uint32 index = 0; index < numShaders; ++index )
	{
		IShader*		pShader		= info.pShaderLib->GetShader( index );
		const achar*	pShaderName	= pShader->GetName();

		// Make sure it doesn't try to override another shader library's names
		if ( info.bGameShaderLib )
		{
			for ( uint32 testIdx = 0, countShaderLibs = ( uint32 )shaderLibs.size(); testIdx < countShaderLibs; ++testIdx )
			{
				const ShaderLibInfo&	testInfo = shaderLibs[testIdx];
				if ( !testInfo.bGameShaderLib )
				{
					if ( testInfo.shaderDict.find( pShaderName ) != testInfo.shaderDict.end() )
					{
						Error( "CShaderSystem: Game shaders '%s' trying to override a base shader '%s'", info.fileName.c_str(), pShaderName );
					}
				}
			}
		}

		info.shaderDict.insert( std::make_pair( pShaderName, pShader ) );
	}
}

/*
==================
CShaderSystem::FindShaderLib
==================
*/
uint32 CShaderSystem::FindShaderLib( const achar* pPath ) const
{
	for ( int32 index = ( int32 )shaderLibs.size(); --index >= 0; )
	{
		const ShaderLibInfo&	info = shaderLibs[index];
		if ( !L_Stricmp( pPath, info.fileName.c_str() ) )
		{
			return index;
		}
	}

	return INDEX_NONE;
}

/*
==================
CShaderSystem::LoadShaderLibs
==================
*/
void CShaderSystem::LoadShaderLibs()
{
	// Unload old shader libs
	UnloadShaderLibs();

	// Load standard shader lib
	LoadShaderLib( "//ENGINEBIN/stdshaders" DLL_EXT_STRING );
}

/*
==================
CShaderSystem::UnloadShaderLibs
==================
*/
void CShaderSystem::UnloadShaderLibs()
{
	if ( !shaderLibs.empty() )
	{
		for ( int32 index = ( int32 )shaderLibs.size(); --index >= 0; )
		{
			UnloadShaderLib( index );
		}

		shaderLibs.clear();
	}
}

/*
==================
CShaderSystem::LoadGameShaderLibs
==================
*/
void CShaderSystem::LoadGameShaderLibs()
{
	// Unload old game's shader libraries
	UnloadGameShaderLibs();

	// Load game's shader lib
	LoadShaderLib( "//GAMEBIN/game_shaders" DLL_EXT_STRING, true );
}

/*
==================
CShaderSystem::UnloadGameShaderLibs
==================
*/
void CShaderSystem::UnloadGameShaderLibs()
{
	if ( !shaderLibs.empty() )
	{
		for ( int32 index = ( int32 )shaderLibs.size(); --index >= 0; )
		{
			const ShaderLibInfo&	info = shaderLibs[index];
			if ( info.bGameShaderLib )
			{
				UnloadShaderLib( index );
				shaderLibs.erase( shaderLibs.begin() + index );
			}
		}
	}
}

/*
==================
CShaderSystem::CreateVertexFactory
==================
*/
TRefPtr<IVertexFactory> CShaderSystem::CreateVertexFactory( EVertexFactoryType vfType ) const
{
	Assert( vfType < VERTEXFACTORY_NUM_TYPES );
	if ( vfType < VERTEXFACTORY_NUM_TYPES && s_pCreateVFFns[( uint32 )vfType] )
	{
		return s_pCreateVFFns[( uint32 )vfType]();
	}
	return nullptr;
}