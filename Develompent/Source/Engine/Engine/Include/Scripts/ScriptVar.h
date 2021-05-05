/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef SCRIPTVAR_H
#define SCRIPTVAR_H

#include "Misc/Types.h"
#include "Misc/EngineGlobals.h"
#include "ScriptEngine.h"
#include "Containers/StringConv.h"

/**
 * @ingroup Engine
 * @brief Class for work with script values
 */
template< typename Type >
class ScriptVar
{
public:
	/**
	 * @brief Constructor
	 */
							ScriptVar() : value( nullptr )
	{}

	/**
	 * @brief Destructor
	 */
							~ScriptVar()
	{}

	/**
	 * @brief Initialize value
	 */
	FORCEINLINE void		Init( int32 InValueID, const tchar* InModuleName )
	{
		value = ( Type* )GScriptEngine->GetASScriptEngine()->GetModule( TCHAR_TO_ANSI( InModuleName ) )->GetAddressOfGlobalVar( InValueID );
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
	Type*		value;			/**< Pointer on script value */
};

#endif // !SCRIPTVAR_H
