/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef SCRIPTVM_H
#define SCRIPTVM_H

#include <unordered_map>

#include "Misc/StringTools.h"
#include "System/Name.h"
#include "Scripts/ScriptMacros.h"

/**
 * @ingroup Core
 * @brief LifeScript virtual machine
 */
class CScriptVM
{
public:
	/**
	 * @brief Add a new native functions into the virtual machine
	 * @warning The native functions table must have NULL item for terminate array
	 *
	 * @param InClassName		Class name of owner the native function table
	 * @param InNativeFuncTable	The native function table
	 */
	static FORCEINLINE void StaticAddNativeFunctions( const CName& InClassName, ScriptNativeFunctionLookup* InNativeFuncTable )
	{
		Assert( InClassName != NAME_None && InNativeFuncTable );
		nativeFunctionsMap.insert( std::make_pair( InClassName, InNativeFuncTable ) );
	}

	/**
	 * @brief Find native function by name
	 *
	 * @param InClassName		Class name
	 * @param InFunctionName	Function name
	 * @return Return found native function. If not returns NULL
	 */
	static FORCEINLINE ScriptFn_t StaticFindNativeFunction( const CName& InClassName, const achar* InFunctionName )
	{
		// If the class has a native function table, loop over the table and find the function
		auto	itNativeFuncTable = nativeFunctionsMap.find( InClassName );
		if ( itNativeFuncTable != nativeFunctionsMap.end() )
		{
			uint32	functionIndex = 0;
			while ( itNativeFuncTable->second[functionIndex].name )
			{
				if ( !L_Strcmp( InFunctionName, itNativeFuncTable->second[functionIndex].name ) )
				{
					return itNativeFuncTable->second[functionIndex].FunctionFn;
				}
				++functionIndex;
			}
		}

		// We nothing to found, return NULL
		return nullptr;
	}

	/**
	 * @brief Get function to execute script opcode
	 *
	 * @param InOpcode  Opcode to execute
	 * @return Return function to execute script opcode
	 */
	static FORCEINLINE ScriptFn_t StaticGetOpcodeFunc( uint32 InOpcode )
	{
		return OpcodeFunctions[InOpcode];
	}

	/**
	 * @brief Process script function
	 * 
	 * @param InObject		Object which the function is
	 * @param InFunction    Function to call
	 */
	static void StaticProcessFunction( class CObject* InObject, class CFunction* InFunction );

	/**
	 * @brief Internal function call processing
	 * @param InStack   Script stack
	 */
	void ProcessInternal( struct ScriptFrame& InStack );

	// LifeScript opcodes
#define DECLARE_OPCODE_FUNCTION( InOpcode ) void execOpcode_##InOpcode( struct ScriptFrame& InStack )
	DECLARE_OPCODE_FUNCTION( Nop );		// OP_Nop
	DECLARE_OPCODE_FUNCTION( Call );	// OP_Call
#undef DECLARE_OPCODE_FUNCTION

private:
	/**
	 * @brief Typedef map of registered native functions
	 */
	typedef std::unordered_map<CName, ScriptNativeFunctionLookup*, CName::HashFunction>		ScriptNativeFunctionsMap_t;

	static ScriptNativeFunctionsMap_t		nativeFunctionsMap;			/**< Map of registered native functions */
	static ScriptFn_t						OpcodeFunctions[OP_Count];	/**< Native functions to execute script opcodes */
};

#endif // !SCRIPTVM_H