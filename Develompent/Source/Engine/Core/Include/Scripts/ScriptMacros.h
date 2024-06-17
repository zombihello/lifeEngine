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

/**
 * @ingroup Core
 * @brief A macro for autogenerated headers to fill out an ScriptNativeFunctionLookup
 *
 * @param InClass			Class name where the function is
 * @param InFunctionName	Function name
 */
#define MAP_NATIVE_FUNC( InClass, InFunctionName ) \
	{ #InClass #InFunctionName, ( ScriptNativeFn_t )&InClass::InFunctionName },

/**
 * @ingroup Core
 * @brief The type of a function callable by script
 */
typedef void ( CObject::*ScriptFn_t )( struct ScriptFrame& InStack );

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
 * @brief Virtual machine opcodes
 */
enum EScriptOpcode
{
	OP_Nop,		/**< No operation */
	OP_Call,	/**< Call script or native function */
	OP_Return,	/**< Return from function */
	OP_Count	/**< Count opcodes */
};

#endif // !SCRIPTMACROS_H