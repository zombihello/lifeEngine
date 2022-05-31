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
 * @brief Macro for implement class API for scripts
 *
 * @param[in] TClass Class
 * @param[in] TRegisterClassAPICode Code of register class API for script
 *
 * Example usage: @code IMPLEMENT_SCRIPT_API( CObject, <Code of register class API for scripts with help LuaBridge> ) @endcode
*/
#define IMPLEMENT_SCRIPT_API( TClass, TRegisterClassAPICode ) \
	/** Register API class for scripts */ \
    void        TClass::StaticRegisterClassAPI( struct lua_State* InVM ) \
    { \
        TRegisterClassAPICode \
    } \
    \
    struct SRegisterClassAPI##TClass \
    { \
        SRegisterClassAPI##TClass() \
        { \
            CScriptEngine::StaticAddRegisterClassAPI( &TClass::StaticRegisterClassAPI ); \
        } \
    } GRegisterClassAPI##TClass;

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
	void				                Init();

    /**
     * @brief Add register class API for scripts
     * @param[in] InRegisterClassAPI Pointer to function for register script API
     */
    static FORCEINLINE void             StaticAddRegisterClassAPI( void( *InRegisterClassAPI )( struct lua_State* InVM ) )
    {
        SManagerClassAPI::Get()->registerClassAPIs.push_back( InRegisterClassAPI );
    }

    /**
     * @brief Register class API
     * @param[in] InVM Pointer to virtual machine
     */
    static FORCEINLINE void             StaticRegisterClassAPI( struct lua_State* InVM )
    {
        SManagerClassAPI*           managerClassAPI = SManagerClassAPI::Get();
        for ( uint32 indexRegister = 0, countRegisters = ( uint32 ) managerClassAPI->registerClassAPIs.size(); indexRegister < countRegisters; ++indexRegister )
        {
            managerClassAPI->registerClassAPIs[ indexRegister ]( InVM );
        }
    }

private:
    /**
     * @brief Structure for storing in the global area an array of pointers to functions for registering API classes for scripts
     */
    struct SManagerClassAPI
    {
        /**
         * @brief Get instance
         * @return Pointer to instance of SManagerClassAPI
         */
        static FORCEINLINE SManagerClassAPI*        Get()
        {
            static SManagerClassAPI*        managerClassAPI = nullptr;
            if ( !managerClassAPI )
            {
                managerClassAPI = new SManagerClassAPI();
            }

            return managerClassAPI;
        }

        std::vector< void(*)( struct lua_State* InVM ) >           registerClassAPIs;           /**< Array of methods for register API classes in scripts */
    };    
};

#endif // !SCRIPTENGINE_H
