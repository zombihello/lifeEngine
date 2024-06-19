#include "Reflection/Object.h"
#include "Reflection/Function.h"
#include "Scripts/ScriptFrame.h"

//
// GLOBALS
//
CObject::ScriptNativeFunctionsMap_t		CObject::nativeFunctionsMap;
ScriptFn_t								CObject::OpcodeFunctions[OP_Count] =
{
	&CObject::execNop,		// OP_Nop
	&CObject::execCall,		// OP_Call
};

/*
==================
CObject::StaticProcessFunction
==================
*/
void CObject::ProcessFunction( class CFunction* InFunction )
{
	AssertMsg( InFunction, TEXT( "Invalid function to call in %s" ), GetFullName().c_str() );
	AssertMsg( IsInGameThread(), TEXT( "Calling %s from outside game thread" ), *InFunction->GetName().c_str() );

	// Create a new local execution stack
	ScriptFrame		stack( this, InFunction );

	// Call script function
	ScriptFn_t		FunctionFn = InFunction->GetFunction();
	( this->*FunctionFn )( stack );
}

/*
==================
CObject::ProcessInternal
==================
*/
void CObject::ProcessInternal( ScriptFrame& InStack )
{
	while ( *InStack.bytecode != OP_Return )
	{
		InStack.Step( InStack.object );
	}
}

//
// LifeScript opcodes
//

#define IMPLEMENT_FUNCTION( InFuncName ) void CObject::exec##InFuncName( struct ScriptFrame& InStack )
/*
==================
Opcode OP_Nop
==================
*/
IMPLEMENT_FUNCTION( Nop )
{
	// Do nothing
}

/*
==================
Opcode OP_Call
==================
*/
IMPLEMENT_FUNCTION( Call )
{
	// Get pointer to function from bytecode
	uptrint			tmpCode		= *( uptrint* )InStack.bytecode;
	CFunction*		function	= ( CFunction* )tmpCode;
	InStack.bytecode += sizeof( uptrint );

	// Process script function
	AssertMsg( function, TEXT( "Invalid function to call in %s" ), InStack.object->GetName().c_str() );
	ScriptFrame		stack( InStack.object, function, &InStack );
	ScriptFn_t		FunctionFn	= function->GetFunction();
	( InStack.object->*FunctionFn )( stack );
}

//
// LifeScript native functions
//

/*
==================
StackTrace
==================
*/
IMPLEMENT_FUNCTION( StackTrace )
{
	Logf( TEXT( "Stack Trace:\n%s\n" ), InStack.GetStackTrace().c_str() );
}
#undef IMPLEMENT_FUNCTION