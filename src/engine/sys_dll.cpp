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

#include "pch_engine.h"
#include "appframework/iwindowmgr.h"
#include "appframework/iappsystemgroup.h"
#include "gameinfo/gameinfo.h"
#include "inputsystem/iinputsystem.h"
#include "filesystem/ifilesystem.h"
#include "studiorender/studioapi/istudioapi.h"
#include "studiorender/istudiorender.h"
#include "game/igame.h"
#include "engine/iengine_launcher.h"
#include "engine/sys_dll.h"
#include "engine/engine.h"

CreateInterfaceFn_t		g_pAppSystemFactory = nullptr;

/**
 * @ingroup engine
 * @brief Main engine interface exposed to launcher
 */
class CEngineLauncher : public CBaseAppSystem<IEngineLauncher>
{
public:
	/**
	 * @brief Connect application system
	 *
	 * @param pFactory		Pointer to interface factory
	 * @return Return TRUE if successes application system is connected, otherwise return FALSE
	 */
	virtual bool Connect( CreateInterfaceFn_t pFactory ) override;

	/**
	 * @brief Disconnect application system
	 */
	virtual void Disconnect() override;

	/**
	 * @brief Query interface
	 *
	 * Here's where systems can access other interfaces implemented by this object
	 *
	 * @param pInterfaceName	Interface name
	 * @return Return pointer to interface, if doesn't implement the requested interface return NULL
	 */
	virtual void* QueryInterface( const achar* pInterfaceName ) override;

	/**
	 * @brief Init application system
	 * @return Return TRUE if application system is inited
	 */
	virtual bool Init() override;

	/**
	 * @brief Shutdown application system
	 */
	virtual void Shutdown() override;

	/**
	 * @brief Set startup info
	 * @note This function must be called before Init()
	 *
	 * @param info	Startup info
	 */
	virtual void SetStartupInfo( const StartupInfo& info ) override;

	/**
	 * @brief Run the engine
	 * @return Return exit code. If all ok returns zero
	 */
	virtual int32 Run() override;

private:
	/**
	 * @brief Initialize file system for game
	 */
	void Game_InitFileSystem();

	/**
	 * @brief Shutdown file system for game
	 */
	void Game_ShutdownFileSystem();

	StartupInfo		startupInfo;	/**< Startup info */
};

static CEngineLauncher		s_EngineLauncher;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR( CEngineLauncher, IEngineLauncher, ENGINE_LAUNCHER_INTERFACE_VERSION, s_EngineLauncher );


/**
 * @ingroup engine
 * @brief Inner loop: initialize, shutdown game systems
 */
class CGameAppSystemGroup : public CAppSystemGroup
{
public:
	/**
	 * @brief Constructor
	 * @param pParentAppSystem		Parent application system
	 */
	CGameAppSystemGroup( CAppSystemGroup* pParentAppSystem = nullptr );

	/**
	 * @brief An installed application creation function,
	 *
	 * An installed application creation function, you should tell the group
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
	virtual bool PreInit() override
	{
		return true;
	}

	/**
	 * @brief Allow the application to do some work after AppSystems are initialized
	 *
	 * Allow the application to do some work after AppSystems are initialized but
	 * before main is run
	 *
	 * @return Return FALSE if there's any problems and the app will abort
	 */
	virtual bool PostInit() override;

	/**
	 * @brief Main loop implemented by the application
	 * @return Return exit code. If all ok returns zero
	 */
	virtual int32 Main() override;

	/**
	 * @brief Allow the application to do some work before all AppSystems are shut down
	 */
	virtual void PreShutdown() override;

	/**
	 * @brief Allow the application to do some work after all AppSystems are shut down
	 */
	virtual void PostShutdown() override
	{}

	/**
	 * @brief Call an installed application destroy function, occurring after all modules are unloaded
	 */
	virtual void Destroy() override;;

	/**
	 * @brief Get appsystemgroup's interface factory
	 * @return Return appsystemgroup's interface factory
	 */
	FORCEINLINE CreateInterfaceFn_t GetFactory() const
	{
		return CAppSystemGroup::GetFactory();
	}

private:
	/**
	 * @brief Load game dll
	 * @return Return TRUE if game dll loaded, otherwise returns FALSE
	 */
	bool GameDLL_Load();

	/**
	 * @brief Unload game dll
	 */
	void GameDLL_Unload();

	dllHandle_t								gameDLLHandle;					/**< Game DLL handle */
	CreateInterfaceFn_t						pGameFactory;					/**< Game interface factory */
	IOnProcessWindowEvent::FuncDelegate*	pProcessWindoeEventDelegate;	/**< Delegate of OnProcessWindowEvent */
};


/*
==================
CGameAppSystemGroup::CGameAppSystemGroup
==================
*/
CGameAppSystemGroup::CGameAppSystemGroup( CAppSystemGroup* pParentAppSystem /* = nullptr */ )
	: CAppSystemGroup( pParentAppSystem )
	, gameDLLHandle( nullptr )
	, pGameFactory( nullptr )
	, pProcessWindoeEventDelegate( nullptr )
{}

/*
==================
CGameAppSystemGroup::Create
==================
*/
bool CGameAppSystemGroup::Create()
{
	// Load game dll
	if ( !GameDLL_Load() )
	{
		Sys_Error( "Failed to load game" DLL_EXT_STRING );
		return false;
	}

	// Add to the group the game's app systems if IGameAppSystems is exist
	IGameAppSystems*				pGameAppSystems = ( IGameAppSystems* )pGameFactory( GAME_APPSYSTEMS_INTERFACE_VERSION );
	std::vector<AppSystemInfo>		gameAppSystems;
	if ( pGameAppSystems )
	{
		// Add each app system into our array
		for ( uint32 index = 0, count = pGameAppSystems->GetNum(); index < count; ++index )
		{
			AppSystemInfo&					appSystemInfo = gameAppSystems.emplace_back();
			appSystemInfo.pModuleName		= pGameAppSystems->GetModuleName( index );
			appSystemInfo.pInterfaceName	= pGameAppSystems->GetInterfaceName( index );
		}		
	}

	// Terminate of array
	AppSystemInfo&						nullTerminateInfo = gameAppSystems.emplace_back();
	nullTerminateInfo.pModuleName		= "";
	nullTerminateInfo.pInterfaceName	= "";

	// Add all the game's app systems into our group
	if ( !AddSystems( gameAppSystems.data() ) )
	{
		return false;
	}

	// We are done!
	return true;
}

/*
==================
CGameAppSystemGroup::PostInit
==================
*/
bool CGameAppSystemGroup::PostInit()
{
	// Initialize the game engine
	if ( !g_Engine.Init() )
	{
		return false;
	}

	// Subscribe on window events
	pProcessWindoeEventDelegate = g_pWindowMgr->OnProcessWindowEvent()->AddFunc( []( void* pUserData, const WindowEvent& windowEvent )
																				 {
																					 switch ( windowEvent.type )
																					 {
																						 // Close window
																					 case WindowEvent::EVENT_WINDOW_CLOSE:
																						 Sys_RequestExit( false );
																						 break;
																					 }
																				 }, nullptr );
	return true;
}

/*
==================
CGameAppSystemGroup::Main
==================
*/
int32 CGameAppSystemGroup::Main()
{
	// Main game loop
	while ( !Sys_IsRequestingExit() )
	{
		// Process window events and update game frame
		g_pWindowMgr->ProcessEvents();
		g_Engine.FrameUpdate();
	}

	return 0;
}

/*
==================
CGameAppSystemGroup::PreShutdown
==================
*/
void CGameAppSystemGroup::PreShutdown()
{
	// Shutdown the game engine
	g_Engine.Shutdown();

	// Describe from window events
	if ( pProcessWindoeEventDelegate )
	{
		g_pWindowMgr->OnProcessWindowEvent()->RemoveFunc( pProcessWindoeEventDelegate );
		pProcessWindoeEventDelegate = nullptr;
	}
}

/*
==================
CGameAppSystemGroup::Destroy
==================
*/
void CGameAppSystemGroup::Destroy()
{
	// Unload game dll
	GameDLL_Unload();
}

/*
==================
CGameAppSystemGroup::GameDLL_Load
==================
*/
bool CGameAppSystemGroup::GameDLL_Load()
{
	// Do nothing if game dll already loaded
	if ( gameDLLHandle )
	{
		return true;
	}

	// Load game dll
	gameDLLHandle = g_pFileSystem->LoadModule( "//GAMEBIN/game" DLL_EXT_STRING );
	if ( !gameDLLHandle )
	{
		Warning( "GameDLL_Load: Failed to load '//GAMEBIN/game" DLL_EXT_STRING "'" );
		GameDLL_Unload();
		return false;
	}

	// Load interface factory and any interfaces exported by the game dll
	pGameFactory = Sys_GetFactory( gameDLLHandle );
	if ( pGameFactory )
	{
		// Get game interface from dll
		g_pGame = ( IGame* )pGameFactory( GAME_INTERFACE_VERSION );
		if ( !g_pGame )
		{
			Warning( "GameDLL_Load: Could not get IGame interface from '//GAMEBIN/game" DLL_EXT_STRING "'" );
			GameDLL_Unload();
			return false;
		}
	}
	else
	{
		Warning( "GameDLL_Load: Could not find factory interface in '//GAMEBIN/game" DLL_EXT_STRING "'" );
		GameDLL_Unload();
		return false;
	}

	// We are done, all ok
	Msg( "GameDLL_Load: game" DLL_EXT_STRING " loaded for %s", g_pGame->GetGameDescription() );
	return true;
}

/*
==================
CGameAppSystemGroup::GameDLL_Unload
==================
*/
void CGameAppSystemGroup::GameDLL_Unload()
{
	if ( gameDLLHandle )
	{
		Msg( "GameDLL_Unload: game" DLL_EXT_STRING " unloaded" );
		g_pFileSystem->UnloadModule( gameDLLHandle );

		gameDLLHandle	= nullptr;
		pGameFactory	= nullptr;
		g_pGame			= nullptr;
	}
}

/*
==================
CEngineLauncher::Connect
==================
*/
bool CEngineLauncher::Connect( CreateInterfaceFn_t pFactory )
{
	// Connect StdLib
	if ( !ConnectStdLib( pFactory ) )
	{
		return false;
	}

	// Get the window manager
	g_pWindowMgr = ( IWindowMgr* )pFactory( WINDOWMGR_INTERFACE_VERSION );
	if ( !g_pWindowMgr )
	{
		return false;
	}

	// Get the input system
	g_pInputSystem = ( IInputSystem* )pFactory( INPUTSYSTEM_INTERFACE_VERSION );
	if ( !g_pInputSystem )
	{
		return false;
	}

	// Get the Studio API
	g_pStudioAPI = ( IStudioAPI* )pFactory( STUDIOAPI_INTERFACE_VERSION );
	if ( !g_pStudioAPI )
	{
		return false;
	}

	// Get the studiorender
	g_pStudioRender = ( IStudioRender* )pFactory( STUDIORENDER_INTERFACE_VERSION );
	if ( !g_pStudioRender )
	{
		return false;
	}

	return true;
}

/*
==================
CEngineLauncher::Disconnect
==================
*/
void CEngineLauncher::Disconnect()
{
	g_pWindowMgr	= nullptr;
	g_pInputSystem	= nullptr;
	DisconnectStdLib();
}

/*
==================
CEngineLauncher::QueryInterface
==================
*/
void* CEngineLauncher::QueryInterface( const achar* pInterfaceName )
{
	CreateInterfaceFn_t		pFactory = Sys_GetFactoryThis();
	return pFactory( pInterfaceName );
}

/*
==================
CEngineLauncher::Init
==================
*/
bool CEngineLauncher::Init()
{
	// Register cvars
	ConVar_Register();
	return true;
}

/*
==================
CEngineLauncher::Shutdown
==================
*/
void CEngineLauncher::Shutdown()
{
	// Unregister cvars
	ConVar_Unregister();
}

/*
==================
CEngineLauncher::SetStartupInfo
==================
*/
void CEngineLauncher::SetStartupInfo( const StartupInfo& info )
{
	// Copy off all the startup info
	startupInfo = info;
}

/*
==================
CEngineLauncher::Run
==================
*/
int32 CEngineLauncher::Run()
{
	//
	// NOTE: Systems set up here should depend on the game 
	// Systems which are game-independent should be set up in the launcher or Init()
	//

	// Load gameinfo.txt
	if ( !g_Engine.LoadGameInfo( L_Sprintf( "//BASE_PATH/%s/gameinfo.txt", startupInfo.pGame ).c_str() ) )
	{
		Sys_Error( "CEngineLauncher: Setup file 'gameinfo.txt' doesn't exist in subdirectory '%s'", startupInfo.pGame );
		return 1;
	}

	// Initialize file system for the game
	Game_InitFileSystem();

	// Run game app system group
	CGameAppSystemGroup		gameAppSystemGroup( startupInfo.pParentAppSystemGroup );
	g_pAppSystemFactory	= gameAppSystemGroup.GetFactory();
	int32 result		= gameAppSystemGroup.Run();
	g_pAppSystemFactory = nullptr;

	// We are done, shutdown the file system for the game
	Game_ShutdownFileSystem();

	// Unload gameinfo.txt and exit
	g_Engine.UnloadGameInfo();
	return result;
}

/*
==================
CEngineLauncher::Game_InitFileSystem
==================
*/
void CEngineLauncher::Game_InitFileSystem()
{
	// Setup search paths from gameinfo
	const std::vector<GameInfoSearchPath>&		searchPaths = g_Engine.GetGameInfo().GetSearchPaths();
	for ( uint32 index = 0, count = ( uint32 )searchPaths.size(); index < count; ++index )
	{
		const GameInfoSearchPath&		searchPath = searchPaths[index];
		g_pFileSystem->AddSearchPath( searchPath.path.c_str(), searchPath.id.c_str() );
	}
}

/*
==================
CEngineLauncher::Game_ShutdownFileSystem
==================
*/
void CEngineLauncher::Game_ShutdownFileSystem()
{
	// Remove only paths "GAME" and "GAMEBIN" if gameinfo.txt not loaded
	const CGameInfoDoc&		gameinfo = g_Engine.GetGameInfo();
	if ( !gameinfo.IsLoaded() )
	{
		Warning( "CEngineLauncher::Game_ShutdownFileSystem: gameinfo.txt not laoded, will be remove only search paths \"GAME\" and \"GAMEBIN\"" );
		g_pFileSystem->RemoveSearchPath( "GAME" );
		g_pFileSystem->RemoveSearchPath( "GAMEBIN" );
		return;
	}

	// Otherwise we look for search paths in gameinfo.txt and
	// remove they from the file system
	std::unordered_set<std::string>			pathIDSet;
	const std::vector<GameInfoSearchPath>&	searchPaths = gameinfo.GetSearchPaths();
	for ( uint32 index = 0, count = ( uint32 )searchPaths.size(); index < count; ++index )
	{
		const GameInfoSearchPath&	searchPath = searchPaths[index];
		pathIDSet.insert( searchPath.id );
	}

	for ( auto it = pathIDSet.begin(), itEnd = pathIDSet.end(); it != itEnd; ++it )
	{
		g_pFileSystem->RemoveSearchPath( ( *it ).c_str() );
	}
}