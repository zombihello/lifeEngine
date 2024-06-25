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
	 * @param InByteCodeOffset	Offset in function bytecode
	 * @param InLocalData		Buffer for local data where store function parameters, local parameters and output parameters
	 * @param InPreviousFrame	Previous frame on the stack
	 */
	ScriptFrame( CObject* InObject, CFunction* InFunction, uint32 InByteCodeOffset, byte* InLocalData, ScriptFrame* InPreviousFrame = nullptr );

	/**
	 * @brief Step of executing code
	 * @param InContext		Context (the same as operator 'this' in C++)
	 * @param InResult		Output result of operations
	 */
	FORCEINLINE void Step( CObject* InContext, RESULT_DECLARE )
	{
		uint32	opcode = *bytecode++;
		( InContext->*( CObject::StaticGetOpcodeFunc( opcode ) ) )( *this, InResult );
	}

	/**
	 * @brief Read 32bit integer at current executing bytecode
	 * @note After read 32bit integer at current executing bytecode the position will be shifted
	 * @return Return 32bit integer at current code position
	 */
	FORCEINLINE int32 ReadInt32()
	{
		int32	result = 0;
		result = *( int32* )bytecode;
		bytecode += sizeof( int32 );
		return result;
	}

	/**
	 * @brief Read 16bit integer at current executing bytecode
	 * @note After read 16bit integer at current executing bytecode the position will be shifted
	 * @return Return 16bit integer at current code position
	 */
	FORCEINLINE int16 ReadInt16()
	{
		int16	result = 0;
		result = *( int16* )bytecode;
		bytecode += sizeof( int16 );
		return result;
	}

	/**
	 * @brief Read float at current executing bytecode
	 * @note After read float at current executing bytecode the position will be shifted
	 * @return Return float at current code position
	 */
	FORCEINLINE float ReadFloat()
	{
		float	result = 0.f;
		result = *( float* )bytecode;
		bytecode += sizeof( float );
		return result;
	}

	/**
	 * @brief Read object at current executing bytecode
	 * @note After read object at current executing bytecode the position will be shifted
	 * @return Return object at current code position
	 */
	FORCEINLINE CObject* ReadObject()
	{
		uptrint		tmpCode = 0;
		tmpCode = *( uptrint* )bytecode;
		bytecode += sizeof( uptrint );
		return ( CObject* )tmpCode;
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
	byte*			localData;		/**< Buffer for local data where store function parameters, local parameters and output parameters */
};

#endif // !SCRIPTFRAME_H