/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef SCRIPTVAR_H
#define SCRIPTVAR_H

#include <type_traits>

#include "Misc/Types.h"
#include "Misc/EngineGlobals.h"
#include "ScriptEngine.h"
#include "Containers/StringConv.h"

/**
 * @ingroup Engine
 * @brief Class for work with script values
 */
template< typename Type, bool InIsScriptObject = std::is_base_of< class ScriptObject, Type >::value >
class ScriptVar
{
public:
	/**
	 * @brief Constructor
	 */
							ScriptVar() : value( nullptr ), isNeedFree( false )
	{}

	/**
	 * @brief Destructor
	 */
							~ScriptVar()
	{
		if ( isNeedFree )
		{
			delete value;
		}
	}

	/**
	 * @brief Initialize value
	 * 
	 * @param[in] InValueID Index of global value in script module
	 * @param[in] InModuleName Script module name
	 */
	FORCEINLINE void		Init( int32 InValueID, const tchar* InModuleName )
	{
		void*		address = GScriptEngine->GetASScriptEngine()->GetModule( TCHAR_TO_ANSI( InModuleName ) )->GetAddressOfGlobalVar( InValueID );		
		SetValue< InIsScriptObject >( address );
	}

	/**
	 * @brief Initialize value
	 * 
	 * @param[in] InValueID Index of property in script object
	 * @param[in] InScriptObject Pointer to script object
	 */
	FORCEINLINE void		Init( int32 InValueID, asIScriptObject* InScriptObject )
	{
		check( InScriptObject );
		void*		address = InScriptObject->GetAddressOfProperty( InValueID );
		SetValue< InIsScriptObject >( address );
	}

	/**
	 * @brief Get value
	 * @return Return pointer on script value. If not initialize or empty return nullptr
	 */
	FORCEINLINE Type*		GetValue() const
	{
		return value;
	}

	/**
	 * @brief Operator for cast to Type*
	 * @return Pointer on script value
	 */
	FORCEINLINE				operator Type*() const
	{
		return value;
	}

	/**
	 * @brief Operator for cast to Type
	 * @return Script value
	 */
	FORCEINLINE				operator Type() const
	{
		return *value;
	}

	/**
	 * @brief Operator ->
	 * @return Pointer on script value
	 */
	FORCEINLINE Type*		operator->() const
	{
		return value;
	}

private:
	/**
	 * @brief Set pointer to script value
	 * @warning This method cannot be called, since it is used to compile different variations of SetValue (for simple types and classes derived from ScriptObject)
	 * 
	 * @param[in] InAddress Pointer to script value
	 */
	template< bool >
	void SetValue( void* InAddress )
	{
		check( false );
	}

	/**
	 * @brief Set pointer to script value for simple and C++ types (float, int, std::string, etc)
	 * 
	 * @param[in] InAddress Pointer to script value
	 */
	template<>
	void SetValue< false >( void* InAddress )
	{
		value = ( Type* )InAddress;
		isNeedFree = false;
	}

	/**
	 * @brief Set pointer to script value for classes derived from ScriptObject
	 * 
	 * @param[in] InAddress Pointer to script value
	 */
	template<>
	void SetValue< true >( void* InAddress )
	{
		value = new Type( ( asIScriptObject* )InAddress );
		isNeedFree = true;
	}

	bool		isNeedFree;		/**< Is need free memory when object destroyed */
	Type*		value;			/**< Pointer on script value */
};

#endif // !SCRIPTVAR_H
