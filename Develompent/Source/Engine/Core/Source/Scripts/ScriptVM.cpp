#include "Reflection/Object.h"
#include "Reflection/Function.h"
#include "Scripts/ScriptVM.h"
#include "Scripts/ScriptFrame.h"

//
// GLOBALS
//
CScriptVM::ScriptNativeFunctionsMap_t	CScriptVM::nativeFunctionsMap;
ScriptFn_t								CScriptVM::OpcodeFunctions[OP_Count] =
{
	&CObject::execOpcode_Nop,		// OP_Nop
	&CObject::execOpcode_Call,		// OP_Call
};

/*
==================
CScriptVM::StaticProcessFunction
==================
*/
void CScriptVM::StaticProcessFunction( class CObject* InObject, class CFunction* InFunction )
{
	AssertMsg( InObject, TEXT( "Invalid object" ) );
	AssertMsg( InFunction, TEXT( "Invalid function to call in %s" ), InObject->GetFullName().c_str() );
	AssertMsg( IsInGameThread(), TEXT( "Calling %s from outside game thread" ), *InFunction->GetName().c_str() );

	// Create a new local execution stack
	ScriptFrame		stack( InObject, InFunction );

	// Call script function
	ScriptFn_t		FunctionFn = InFunction->GetFunction();
	( InObject->*FunctionFn )( stack );
}

/*
==================
CScriptVM::ProcessInternal
==================
*/
void CScriptVM::ProcessInternal( ScriptFrame& InStack )
{
	while ( *InStack.bytecode != OP_Return )
	{
		InStack.Step( InStack.object );
	}
}

//
// LifeScript opcodes
//

#define IMPLEMENT_OPCODE_FUNCTION( InOpcode ) void CScriptVM::execOpcode_##InOpcode( struct ScriptFrame& InStack )
/*
==================
Nop
==================
*/
IMPLEMENT_OPCODE_FUNCTION( Nop )
{
	// Do nothing
}

/*
==================
Call
==================
*/
IMPLEMENT_OPCODE_FUNCTION( Call )
{
	// Get pointer to function from bytecode
	uptrint			tmpCode = *( uptrint* )InStack.bytecode;
	CFunction*		function = ( CFunction* )tmpCode;
	InStack.bytecode += sizeof( uptrint );

	// Process script function
	AssertMsg( function, TEXT( "Invalid function to call in %s" ), InStack.object->GetName().c_str() );
	ScriptFrame		stack( InStack.object, function, &InStack );
	ScriptFn_t		FunctionFn = function->GetFunction();
	( InStack.object->*FunctionFn )( stack );
}
#undef IMPLEMENT_OPCODE_FUNCTION