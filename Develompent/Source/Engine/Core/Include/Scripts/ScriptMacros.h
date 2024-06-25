/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef SCRIPTMACROS_H
#define SCRIPTMACROS_H

#include "Misc/Types.h"

// Forward declarations
class CObject;

#define RESULT_DECLARE		void* const InResult

/**
 * @ingroup Core
 * @brief Macro declare function for callable in/from LifeScript
 *
 * @param TFunction     Function
 */
#define DECLARE_FUNCTION( TFunction ) \
    void exec##TFunction( ScriptFrame& InStack, RESULT_DECLARE )

/**
 * @ingroup Core
 * @brief A macro for autogenerated headers to fill out an ScriptNativeFunctionLookup
 *
 * @param InClass			Class name where the function is
 * @param InFunctionName	Function name
 */
#define MAP_NATIVE_FUNC( InClass, InFunctionName ) \
	{ #InClass "exec" #InFunctionName, ( ScriptFn_t )&InClass::exec##InFunctionName },

/**
 * @ingroup Core
 * @brief Get 32bit integer parameter at current executing bytecode for native function
 * @param InVariable	Variable name
 */
#define STACKFRAME_GET_INT32( InVariable )		int32 InVariable = 0; InStack.Step( InStack.object, &InVariable );

/**
 * @ingroup Core
 * @brief End of function parameters (it need for skip opcode 'OP_EndFunctionParams')
 */
#define STACKFRAME_GET_FINISH					++InStack.bytecode;

/**
 * @ingroup Core
 * @brief The type of a function callable by script
 */
typedef void ( CObject::*ScriptFn_t )( struct ScriptFrame& InStack, RESULT_DECLARE );

/**
 * @ingroup Core
 * @brief A struct that maps a string name to a native function
 */
struct ScriptNativeFunctionLookup
{
	const achar*	name;		/**< Function name */
	ScriptFn_t		FunctionFn;	/**< Pointer to function */
};

/**
 * @ingroup Core
 * @brief Size of the buffer used by LifeScript virtual machine for unused simple return values
 */
enum
{
	MAX_SIMPLE_RETURN_VALUE_SIZE = 64
};

/**
 * @ingroup Core
 * @brief Virtual machine opcodes
 */
enum EScriptOpcode
{
	OP_Nop,					/**< No operation */
	OP_Call,				/**< Call script or native function */
	OP_Return,				/**< Return from function */
	OP_IntConst,			/**< Integer constant */
	OP_EndFunctionParms,	/**< End of function call parameters */
	OP_Count				/**< Count opcodes */
};

#endif // !SCRIPTMACROS_H