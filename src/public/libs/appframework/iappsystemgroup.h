/**
 * @file
 * @addtogroup appframework appframework
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

#ifndef IAPPSYSTEMGROUP_H
#define IAPPSYSTEMGROUP_H

#include <string>
#include <vector>
#include <unordered_map>

#include "core/core.h"
#include "appframework/iappsystem.h"
#include "stdlib/interface.h"

/**
 * @ingroup appframework
 * @brief Handle to a DLL
 */
typedef int32		appModule_t;

/**
 * @ingroup appframework
 * @brief Invalid module application
 */
enum
{
	APP_MODULE_INVALID = ( appModule_t )~0
};

/**
 * @ingroup appframework
 * @brief Specifies a module + interface name for initialization
 */
struct AppSystemInfo
{
	std::string		moduleName;		/**< Module name */
	std::string		interfaceName;	/**< Inteface name */
};

/**
 * @ingroup appframework
 * @brief Interface of application system group
 */
class IAppSystemGroup
{
public:
	/**
	 * @brief An installed application creation function, 
	 * 
	 * An installed application creation function, you should tell the group
	 * the DLLs and the singleton interfaces you want to instantiate.
	 * 
	 * @return Return FALSE if there's any problems and the app will abort
	 */
	virtual bool Create() = 0;

	/**
	 * @brief Allow the application to do some work after AppSystems are connected
	 * 
	 * Allow the application to do some work after AppSystems are connected but
	 * they aren't all Initialized
	 * 
	 * @return Return FALSE if there's any problems and the app will abort
	 */
	virtual bool PreInit() = 0;

	/**
	 * @brief Allow the application to do some work after AppSystems are initialized
	 *
	 * Allow the application to do some work after AppSystems are initialized but
	 * before main is run
	 * 
	 * @return Return FALSE if there's any problems and the app will abort
	 */
	virtual bool PostInit() = 0;

	/**
	 * @brief Main loop implemented by the application
	 */
	virtual int32 Main() = 0;

	/**
	 * @brief Allow the application to do some work before all AppSystems are shut down
	 */
	virtual void PreShutdown() = 0;

	/**
	 * @brief Allow the application to do some work after all AppSystems are shut down
	 */
	virtual void PostShutdown() = 0;

	/**
	 * @brief Call an installed application destroy function, occurring after all modules are unloaded
	 */
	virtual void Destroy() = 0;
};

/**
 * @ingroup appframework
 * @brief This class represents a group of app systems
 *
 * This class represents a group of app systems that all have the same lifetime
 * that need to be connected/initialized, etc. in a well-defined order
 */
class CAppSystemGroup : public IAppSystemGroup
{
public:
	/**
	 * @brief Used to determine where we exited out from the system
	 */
	enum EAppSystemGroupStage
	{
		APPSYSTEM_STAGE_CREATION,			/**< Creation */
		APPSYSTEM_STAGE_CONNECTION,			/**< Connection */
		APPSYSTEM_STAGE_PREINITIALIZATION,	/**< Pre initialization */
		APPSYSTEM_STAGE_INITIALIZATION,		/**< Initialization */
		APPSYSTEM_STAGE_POSTINITIALIZATION,	/**< Post initialization */
		APPSYSTEM_STAGE_RUNNING,			/**< Running */
		APPSYSTEM_STAGE_PRESHUTDOWN,		/**< Pre shutdown */
		APPSYSTEM_STAGE_SHUTDOWN,			/**< Shutdown */
		APPSYSTEM_STAGE_POSTSHUTDOWN,		/**< Post shutdown */
		APPSYSTEM_STAGE_DISCONNECTION,		/**< Disconnection */
		APPSYSTEM_STAGE_DESTRUCTION,		/**< Destruction */

		APPSYSTEM_GROUP_STAGE_COUNT,		/**< Count stages */
		APPSYSTEM_STAGE_NONE,				/**< This means no error */
	};

	/**
	 * @brief Constructor
	 * @param pParentAppSystem		Parent application system
	 */
	CAppSystemGroup( CAppSystemGroup* pParentAppSystem = nullptr );

	/**
	 * @brief Runs the app system group
	 * 
	 * First, modules are loaded, next they are connected, followed by initialization
	 * then Main() is run, then modules are shut down, disconnected, and unloaded
	 * 
	 * @return Return exit code
	 */
	int32 Run();

	/**
	 * @brief Allow the application to do some work after AppSystems are initialized
	 * 
	 * Allow the application to do some work after AppSystems are initialized but
	 * before main is run
	 *
	 * @return Return FALSE if there's any problems and the app will abort
	 */
	virtual bool PostInit() override
	{
		return true;
	}

	/**
	 * @brief Allow the application to do some work after all AppSystems are shut down
	 */
	virtual void PreShutdown() override
	{}

	/**
	 * @brief Get current stage
	 * @return Returns the stage at which the app system group ran into an error
	 */
	EAppSystemGroupStage GetCurrentStage() const;

protected:
	/**
	 * @brief Load module
	 * 
	 * @param pDLLName	DLL name to load
	 * @return Return index of loaded module, in fail case will return APP_MODULE_INVALID
	 */
	appModule_t LoadModule( const achar* pDLLName );

	/**
	 * @brief Load module
	 *
	 * @param pFactory	Interface factory
	 * @return Return index of loaded module, in fail case will return APP_MODULE_INVALID
	 */
	appModule_t LoadModule( CreateInterfaceFn_t pFactory );

	/**
	 * @brief Add system
	 * 
	 * @param module			Module index
	 * @param pInterfaceName	Interface name
	 * @return Return application system, in fail case will return NULL
	 */
	IAppSystem* AddSystem( appModule_t module, const achar* pInterfaceName );

	/**
	 * @brief Add system
	 * 
	 * @param pAppSystem		Application system
	 * @param pInterfaceName	Interface name
	 */
	void AddSystem( IAppSystem* pAppSystem, const achar* pInterfaceName );

	/**
	 * @brief Add array of systems
	 * @note Make sure the last AppSystemInfo has a NULL module name
	 * 
	 * @param pAppSystems	Array of systems
	 */
	bool AddSystems( AppSystemInfo* pAppSystems );

	/**
	 * @brief Find application system by interface name
	 * 
	 * @param pInterfaceName	Interface name
	 * @return Return found application system. If not exist return NULL
	 */
	void* FindSystem( const achar* pInterfaceName ) const;

	/**
	 * @brief Get appsystemgroup's interface factory
	 * @return Return appsystemgroup's interface factory
	 */
	static CreateInterfaceFn_t GetFactory();

private:
	friend void* AppSystemCreateInterfaceFn( const achar* pName );

	/**
	 * @brief Module
	 */
	struct Module
	{
		dllHandle_t				handle;			/**< Module handle */
		CreateInterfaceFn_t		pFactoryFn;		/**< Interface factory of module */
		std::string				name;			/**< Module name */
	};

	/**
	 * @brief Startup
	 * @return Return exit code
	 */
	int32 Startup();

	/**
	 * @brief Shutdown
	 */
	void Shutdown();

	/**
	 * @brief Unload all modules
	 */
	void UnloadAllModules();

	/**
	 * @brief Remove all systems
	 */
	void RemoveAllSystems();

	/**
	 * @brief Connect systems
	 * 
	 * @param failureSysIndex	Indicates the index of the system that could not be connected
	 * @return Return TRUE in case when all systems successes connected, otherwise will return FALSE
	 */
	bool ConnectSystems( uint32& failureSysIndex );

	/**
	 * @brief Disconnect systems
	 */
	void DisconnectSystems();

	/**
	 * @brief Init all systems
	 * 
	 * @param failureSysIndex	Indicates the index of the system that could not be initialized
	 * @return Return TRUE in case when all systems successes inited, otherwise will return FALSE
	 */
	bool InitSystems( uint32& failureSysIndex );

	/**
	 * @brief Shutdown all systems
	 */
	void ShutdownSystems();

	/**
	 * @brief Get parent application system group
	 * @return Return parent application system group. If not exist returns NULL
	 */
	CAppSystemGroup* GetParent() const;

	/**
	 * @brief Find application system name by index
	 * @param index		Application system index
	 * 
	 * @return Return application system name. If not found return empty string
	 */
	std::string FindSystemName( int32 index );

	/**
	 * @brief Report failure
	 * 
	 * @param errorStage		Error stage
	 * @param sysIndex			Application system index. Ignored if it is -1
	 */
	void ReportFailure( int32 errorStage, int32 sysIndex = -1 );

	std::vector<Module>							modules;				/**< Modules */
	std::vector<IAppSystem*>					systems;				/**< Application systems */
	std::unordered_map<std::string, uint32>		systemDict;				/**< Application systems dictionary for find it by interface name */
	CAppSystemGroup*							pParentAppSystemGroup;	/**< Parent application system group */
	EAppSystemGroupStage						currentStage;			/**< Current stage */
};

/**
 * @ingroup appframework
 * @brief Helper empty decorator implementation of an IAppSystemGroup
 */
template<class CBaseClass>
class CDefaultAppSystemGroup : public CBaseClass
{
public:
	virtual bool Create()		{ return true; }
	virtual bool PreInit()		{ return true; }
	virtual bool PostInit()		{ return true; }
	virtual void PreShutdown()	{}
	virtual void PostShutdown() {}
	virtual void Destroy()		{}
};

#endif // !IAPPSYSTEMGROUP_H