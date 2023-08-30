/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef SCRIPTENGINE_H
#define SCRIPTENGINE_H

extern "C"
{
    #include <lua.h>
    #include <lauxlib.h>
    #include <lualib.h>
    #include <luajit.h>
}

#include <LuaBridge/LuaBridge.h>
#include <unordered_map>

#include "Core.h"

/**
 * @ingroup Engine
 * @brief Macro for declare class API for scripts
 */
#define DECLARE_SCRIPT_API( ... ) \
	public: \
        /** */ \
        /** @brief Register class API for scripts */ \
        /** @param[in] InVM Pointer to virtual machine */ \
        /** */ \
        static void                 StaticRegisterClassAPI( struct lua_State* InVM );

/**
 * @ingroup Engine
 * @brief Macro for begin implement a class API for scripts
 *
 * @param Class     Class
*/
#define BEGIN_SCRIPT_API( Class ) \
    struct RegisterClassAPI##Class \
    { \
        RegisterClassAPI##Class() \
        { \
            CScriptEngine::StaticAddRegisterClassAPI( &Class::StaticRegisterClassAPI ); \
        } \
    } g_RegisterClassAPI##Class; \
    \
	/** Register API class for scripts */ \
    void        Class::StaticRegisterClassAPI( struct lua_State* InVM ) \
    {

/**
* @ingroup Engine
* @brief Macro for end implement a class API for scripts
*/
#define END_SCRIPT_API() \
    }

/**
 * @ingroup Engine
 * @brief Class for work with script engine
 */
class CScriptEngine
{
public:
	/**
	 * @brief Initialize script engine
	 */
	void Init();

    /**
     * @brief Add register class API for scripts
     * @param[in] InRegisterClassAPI Pointer to function for register script API
     */
    static FORCEINLINE void StaticAddRegisterClassAPI( void( *InRegisterClassAPI )( struct lua_State* InVM ) )
    {
        ManagerClassAPI::Get()->registerClassAPIs.push_back( InRegisterClassAPI );
    }

    /**
     * @brief Register class API
     * @param[in] InVM Pointer to virtual machine
     */
    static FORCEINLINE void StaticRegisterClassAPI( struct lua_State* InVM )
    {
        ManagerClassAPI*           managerClassAPI = ManagerClassAPI::Get();
        for ( uint32 indexRegister = 0, countRegisters = ( uint32 ) managerClassAPI->registerClassAPIs.size(); indexRegister < countRegisters; ++indexRegister )
        {
            managerClassAPI->registerClassAPIs[ indexRegister ]( InVM );
        }
    }

private:
    /**
     * @brief Structure for storing in the global area an array of pointers to functions for registering API classes for scripts
     */
    struct ManagerClassAPI
    {
        /**
         * @brief Get instance
         * @return Pointer to instance of ManagerClassAPI
         */
        static FORCEINLINE ManagerClassAPI* Get()
        {
            static ManagerClassAPI*        managerClassAPI = nullptr;
            if ( !managerClassAPI )
            {
                managerClassAPI = new ManagerClassAPI();
            }

            return managerClassAPI;
        }

        std::vector< void(*)( struct lua_State* InVM ) >           registerClassAPIs;           /**< Array of methods for register API classes in scripts */
    };    
};

#endif // !SCRIPTENGINE_H
