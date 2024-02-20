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

#include "pch_appframework.h"
#include "appframework/iappsystemgroup.h"

//-----------------------------------------------------------------------------
// Gets at a factory that works just like FindSystem
//-----------------------------------------------------------------------------
// This function is used to make this system appear to the outside world to
// function exactly like the currently existing factory system
CAppSystemGroup*	s_pCurrentAppSystem = nullptr;
void* AppSystemCreateInterfaceFn( const achar* pName )
{
	return s_pCurrentAppSystem->FindSystem( pName );
}

static const achar* s_pStageLookup[] =
{
	"CREATION",					// APPSYSTEM_STAGE_CREATION
	"CONNECTION",				// APPSYSTEM_STAGE_CONNECTION
	"PREINITIALIZATION",		// APPSYSTEM_STAGE_PREINITIALIZATION
	"INITIALIZATION",			// APPSYSTEM_STAGE_INITIALIZATION
	"POSTINITIALIZATION",		// APPSYSTEM_STAGE_POSTINITIALIZATION
	"RUNNING",					// APPSYSTEM_STAGE_RUNNING
	"PRESHUTDOWN",				// APPSYSTEM_STAGE_PRESHUTDOWN
	"SHUTDOWN",					// APPSYSTEM_STAGE_SHUTDOWN
	"POSTSHUTDOWN",				// APPSYSTEM_STAGE_POSTSHUTDOWN
	"DISCONNECTION",			// APPSYSTEM_STAGE_DISCONNECTION
	"DESTRUCTION",				// APPSYSTEM_STAGE_DESTRUCTION
};
static_assert( CAppSystemGroup::APPSYSTEM_GROUP_STAGE_COUNT == ARRAYSIZE( s_pStageLookup ), "Array size 's_pStageLookup' must be equal to APPSYSTEM_GROUP_STAGE_COUNT" );


/*
==================
CAppSystemGroup::CAppSystemGroup
==================
*/
CAppSystemGroup::CAppSystemGroup( CAppSystemGroup* pParentAppSystem /* = nullptr */ )
	: pParentAppSystemGroup( pParentAppSystem )
	, currentStage( APPSYSTEM_STAGE_NONE )
{}

/*
==================
CAppSystemGroup::Run
==================
*/
int32 CAppSystemGroup::Run()
{
	// The factory now uses this app system group
	s_pCurrentAppSystem = this;
	Msg( "AppFramework: user %s//%s", Sys_GetComputerName(), Sys_GetUserName() );

	// Load, connect, init
	int32	retVal = Startup();

	// Main loop implemented by the application 
	if ( currentStage == APPSYSTEM_STAGE_RUNNING )
	{
		retVal = Main();
		if ( retVal != 0 )
		{
			ReportFailure( APPSYSTEM_STAGE_RUNNING );
		}
	}

	// Shutdown, disconnect, unload
	Shutdown();

	// The factory now uses the parent's app system group
	s_pCurrentAppSystem = GetParent();
	return retVal;
}

/*
==================
CAppSystemGroup::Startup
==================
*/
int32 CAppSystemGroup::Startup()
{
	// The factory now uses this app system group
	uint32	failureSysIndex = 0;
	s_pCurrentAppSystem = this;

	// Call an installed application creation function
	currentStage = APPSYSTEM_STAGE_CREATION;
	if ( !Create() )
	{
		ReportFailure( APPSYSTEM_STAGE_CREATION );
		return -1;
	}

	// Let all systems know about each other
	currentStage = APPSYSTEM_STAGE_CONNECTION;
	if ( !ConnectSystems( failureSysIndex ) )
	{
		ReportFailure( APPSYSTEM_STAGE_CONNECTION, failureSysIndex );
		return -1;
	}

	// Allow the application to do some work before init
	currentStage = APPSYSTEM_STAGE_PREINITIALIZATION;
	if ( !PreInit() )
	{
		ReportFailure( APPSYSTEM_STAGE_PREINITIALIZATION );
		return -1;
	}

	// Call Init on all App Systems
	currentStage = APPSYSTEM_STAGE_INITIALIZATION;
	if ( !InitSystems( failureSysIndex ) )
	{
		ReportFailure( APPSYSTEM_STAGE_INITIALIZATION, failureSysIndex );
		return -1;
	}

	// Allow the application to do some work after init
	currentStage = APPSYSTEM_STAGE_POSTINITIALIZATION;
	if ( !PostInit() )
	{
		ReportFailure( APPSYSTEM_STAGE_POSTINITIALIZATION );
		return -1;
	}

	currentStage = APPSYSTEM_STAGE_RUNNING;
	return 0;
}

/*
==================
CAppSystemGroup::Shutdown
==================
*/
void CAppSystemGroup::Shutdown()
{
	// The factory now uses this app system group
	s_pCurrentAppSystem	= this;

	switch(currentStage )
	{
	case APPSYSTEM_STAGE_RUNNING:
	case APPSYSTEM_STAGE_POSTINITIALIZATION:
		break;

	case APPSYSTEM_STAGE_PREINITIALIZATION:
	case APPSYSTEM_STAGE_INITIALIZATION:
		goto disconnect;
	
	case APPSYSTEM_STAGE_CREATION:
	case APPSYSTEM_STAGE_CONNECTION:
		goto destroy;

	default:
		break;
	}

	// Allow the application to do some work before shutdown
	currentStage = APPSYSTEM_STAGE_PRESHUTDOWN;
	PreShutdown();

	// Cal Shutdown on all App Systems
	currentStage = APPSYSTEM_STAGE_SHUTDOWN;
	ShutdownSystems();

	// Allow the application to do some work after shutdown
	currentStage = APPSYSTEM_STAGE_POSTSHUTDOWN;
	PostShutdown();

disconnect:
	// Systems should disconnect from each other
	currentStage = APPSYSTEM_STAGE_DISCONNECTION;
	DisconnectSystems();

destroy:
	// Unload all DLLs loaded in the AppCreate block
	currentStage = APPSYSTEM_STAGE_DESTRUCTION;
	RemoveAllSystems();
	UnloadAllModules();

	// Call an installed application destroy function
	Destroy();
}

/*
==================
CAppSystemGroup::GetCurrentStage
==================
*/
CAppSystemGroup::EAppSystemGroupStage CAppSystemGroup::GetCurrentStage() const
{
	return currentStage;
}

/*
==================
CAppSystemGroup::LoadModule
==================
*/
appModule_t CAppSystemGroup::LoadModule( const achar* pDLLName )
{
	// See if we already loaded it..
	for ( int32 index = ( int32 )modules.size(); --index >= 0; )
	{
		if ( modules[index].name == pDLLName )
		{
			return index;
		}
	}

	// Otherwise we load module
	dllHandle_t		dllHandle = Sys_DLL_LoadModule( pDLLName );
	if ( !dllHandle )
	{
		Warning( "AppFramework: Unable to load module '%s'!", pDLLName );
		return APP_MODULE_INVALID;
	}

	Msg( "AppFramework: Loaded module '%s'", pDLLName );
	int32	index = ( int32 )modules.size();
	modules.push_back( Module{ dllHandle, nullptr, pDLLName } );
	return index;
}

/*
==================
CAppSystemGroup::LoadModule
==================
*/
appModule_t CAppSystemGroup::LoadModule( CreateInterfaceFn_t pFactory )
{
	if ( !pFactory )
	{
		Warning( "AppFramework: Unable to load module because pFactory is NULL!", pFactory );
		return APP_MODULE_INVALID;
	}

	// See if we already loaded it..
	for ( int32 index = ( int32 )modules.size(); --index >= 0; )
	{
		if ( modules[index].pFactoryFn == pFactory )
		{
			return index;
		}
	}

	Msg( "AppFramework: Loaded module '%p'", pFactory );
	int32	index = ( int32 )modules.size();
	modules.push_back( Module{ nullptr, pFactory, "" } );
	return index;
}

/*
==================
CAppSystemGroup::AddSystem
==================
*/
IAppSystem* CAppSystemGroup::AddSystem( appModule_t module, const achar* pInterfaceName )
{
	if ( module == APP_MODULE_INVALID )
	{
		return nullptr;
	}

	if ( systemDict.find( pInterfaceName ) != systemDict.end() )
	{
		Warning( "AppFramework: Attempted to add two systems with the same interface name '%s'!", pInterfaceName );
		return systems[systemDict[pInterfaceName]];
	}

	Assert( module >= 0 && module < modules.size() );
	CreateInterfaceFn_t		pFactoryFn = modules[module].handle ? Sys_GetFactory( modules[module].handle ) : modules[module].pFactoryFn;

	void*	pSystem = pFactoryFn( pInterfaceName );
	if ( !pSystem )
	{
		Warning( "AppFramework: Unable to create system '%s'!", pInterfaceName );
		return nullptr;
	}

	IAppSystem*	pAppSystem = ( IAppSystem* )pSystem;
	int32		sysIndex = ( int32 )systems.size();
	systems.push_back( pAppSystem );

	// Inserting into the dict will help us do named lookup later
	systemDict[pInterfaceName] = sysIndex;
	return pAppSystem;
}

/*
==================
CAppSystemGroup::AddSystem
==================
*/
void CAppSystemGroup::AddSystem( IAppSystem* pAppSystem, const achar* pInterfaceName )
{
	if ( !pAppSystem )
	{
		return;
	}

	if ( systemDict.find( pInterfaceName ) != systemDict.end() )
	{
		Warning( "AppFramework: Attempted to add two systems with the same interface name '%s'!", pInterfaceName );
		return;
	}

	int32		sysIndex = ( int32 )systems.size();
	systems.push_back( pAppSystem );

	// Inserting into the dict will help us do named lookup later
	systemDict[pInterfaceName] = sysIndex;
}

/*
==================
CAppSystemGroup::AddSystems
==================
*/
bool CAppSystemGroup::AddSystems( AppSystemInfo* pAppSystems )
{
	while ( !pAppSystems->moduleName.empty() )
	{
		appModule_t		module = LoadModule( pAppSystems->moduleName.c_str() );
		IAppSystem*		pSystem = AddSystem( module, pAppSystems->interfaceName.c_str() );
		if ( !pSystem )
		{
			Warning( "AppFramework: Unable to load interface '%s' from '%s'", pAppSystems->interfaceName.c_str(), pAppSystems->moduleName.c_str() );
			return false;
		}

		++pAppSystems;
	}
	return true;
}

/*
==================
CAppSystemGroup::FindSystem
==================
*/
void* CAppSystemGroup::FindSystem( const achar* pInterfaceName ) const
{
	auto	it = systemDict.find( pInterfaceName );
	if ( it != systemDict.end() )
	{
		return systems[it->second];
	}

	// If it's not an interface we know about, it could be an older
	// version of an interface, or maybe something implemented by
	// one of the instantiated interfaces...
	for ( uint32 index = 0, count = ( uint32 )systems.size(); index < count; ++index )
	{
		void*	pInterface = systems[index]->QueryInterface( pInterfaceName );
		if ( pInterface )
		{
			return pInterface;
		}
	}

	if ( pParentAppSystemGroup )
	{
		void*	pInterface = pParentAppSystemGroup->FindSystem( pInterfaceName );
		if ( pInterface )
		{
			return pInterface;
		}
	}

	return nullptr;
}

/*
==================
CAppSystemGroup::GetFactory
==================
*/
CreateInterfaceFn_t CAppSystemGroup::GetFactory()
{
	return AppSystemCreateInterfaceFn;
}

/*
==================
CAppSystemGroup::UnloadAllModules
==================
*/
void CAppSystemGroup::UnloadAllModules()
{
	// NOTE: Iterate in reverse order so they are unloaded in opposite order
	// from loading
	for ( int32 index = ( int32 )modules.size(); --index >= 0; )
	{
		if ( modules[index].handle )
		{
			Msg( "AppFramework: Unloaded module '%s'", modules[index].name.c_str() );
			Sys_DLL_UnloadModule( modules[index].handle );
		}
		else if ( modules[index].pFactoryFn )
		{
			Msg( "AppFramework: Unloaded module '%p'", modules[index].pFactoryFn );
		}
	}

	modules.clear();
}

/*
==================
CAppSystemGroup::RemoveAllSystems
==================
*/
void CAppSystemGroup::RemoveAllSystems()
{
	systems.clear();
	systemDict.clear();
}

/*
==================
CAppSystemGroup::ConnectSystems
==================
*/
bool CAppSystemGroup::ConnectSystems( uint32& failureSysIndex )
{
	// Let the libraries grab any other interfaces they may need
	for ( uint32 index = 0, count = ( uint32 )systems.size(); index < count; ++index )
	{
		IAppSystem*		pSystem = systems[index];
		if ( !pSystem->Connect( GetFactory() ) )
		{
			failureSysIndex = index;
			return false;
		}
	}

	return true;
}

/*
==================
CAppSystemGroup::DisconnectSystems
==================
*/
void CAppSystemGroup::DisconnectSystems()
{
	// Disconnect in reverse order of connection
	for ( int32 index = ( int32 )systems.size(); --index >= 0; )
	{
		systems[index]->Disconnect();
	}
}

/*
==================
CAppSystemGroup::InitSystems
==================
*/
bool CAppSystemGroup::InitSystems( uint32& failureSysIndex )
{
	for ( uint32 index = 0, count = ( uint32 )systems.size(); index < count; ++index )
	{
		if ( !systems[index]->Init() )
		{
			for ( int32 systemRewindIndex = index; systemRewindIndex-- > 0; )
			{
				systems[systemRewindIndex]->Shutdown();
			}

			failureSysIndex = index;
			return false;
		}
	}

	return true;
}

/*
==================
CAppSystemGroup::ShutdownSystems
==================
*/
void CAppSystemGroup::ShutdownSystems()
{
	// Shutdown in reverse order of initialization
	for ( int32 index = ( int32 )systems.size(); --index >= 0; )
	{
		systems[index]->Shutdown();
	}
}

/*
==================
CAppSystemGroup::GetParent
==================
*/
CAppSystemGroup* CAppSystemGroup::GetParent() const
{
	return pParentAppSystemGroup;
}

/*
==================
CAppSystemGroup::FindSystemName
==================
*/
std::string CAppSystemGroup::FindSystemName( int32 index )
{
	for ( auto it = systemDict.begin(), itEnd = systemDict.end(); it != itEnd; ++it )
	{
		if ( it->second == index )
		{
			return it->first;
		}
	}
	return "";
}

/*
==================
CAppSystemGroup::ReportFailure
==================
*/
void CAppSystemGroup::ReportFailure( int32 errorStage, int32 sysIndex /* = -1 */ )
{
	const achar*	pStageDesc = "Unknown";
	if ( errorStage >= 0 && errorStage < ( int32 )ARRAYSIZE( s_pStageLookup ) )
	{
		pStageDesc = s_pStageLookup[errorStage];
	}

	// Failure happened on some a system
	if ( sysIndex != -1 )
	{
		std::string		systemName = FindSystemName( sysIndex );
		if ( systemName.empty() )
		{
			systemName = "(Unknown)";
		}

		Sys_Error( "AppFramework: System (%s) failed during stage '%s'", systemName.c_str(), pStageDesc );
	}
	// Failure happened in application
	else
	{
		Sys_Error( "AppFramework: Application failed during stage '%s'", pStageDesc );
	}
}