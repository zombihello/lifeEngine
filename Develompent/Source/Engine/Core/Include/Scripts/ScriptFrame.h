/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef SCRIPTFRAME_H
#define SCRIPTFRAME_H

#include "Reflection/Object.h"

/**
 * @ingroup Core
 * @brief Script frame
 */
struct ScriptFrame
{
	/**
	 * @brief Constructor
	 * @param InObject	Current object (the same as operator 'this' in C++)
	 */
	ScriptFrame( CObject* InObject );

	/**
	 * @brief Constructor
	 * 
	 * @param InObject			Current object (the same as operator 'this' in C++)
	 * @param InFunction		Function to execute
	 * @param InPreviousFrame	Previous frame on the stack
	 */
	ScriptFrame( CObject* InObject, CFunction* InFunction, ScriptFrame* InPreviousFrame = nullptr );

	/**
	 * @brief Step of executing code
	 * @param InContext		Context (the same as operator 'this' in C++)
	 */
	FORCEINLINE void Step( CObject* InContext )
	{
		uint32	opcode = *bytecode++;
		( InContext->*( CObject::StaticGetOpcodeFunc( opcode ) ) )( *this );
	}

	/**
	 * @brief Get script stack trace
	 * @return Return the stack trace of the current callstack from LifeScript land
	 **/
	std::wstring GetStackTrace() const;

	CObject*		object;			/**< Current object (the same as operator 'this' in C++) */
	CFunction*		node;			/**< Current function who executing */
	const byte*		bytecode;		/**< Current executing bytecode */
	ScriptFrame*	previousFrame;	/**< Previous frame on the stack */
};

#endif // !SCRIPTFRAME_H