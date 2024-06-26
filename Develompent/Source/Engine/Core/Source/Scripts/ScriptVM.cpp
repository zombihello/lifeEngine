#include "Reflection/Object.h"
#include "Reflection/Function.h"
#include "Reflection/Property.h"
#include "Reflection/ObjectPackage.h"
#include "Reflection/FieldIterator.h"
#include "Scripts/ScriptFrame.h"

//
// GLOBALS
//
CObject::ScriptNativeFunctionsMap_t		CObject::nativeFunctionsMap;
ScriptFn_t								CObject::OpcodeFunctions[OP_Count] =
{
	&CObject::execNop,		// OP_Nop
	&CObject::execCall,		// OP_Call
	&CObject::execReturn,	// OP_Return
	&CObject::execIntConst	// OP_IntConst
};

/*
==================
CObject::ProcessFunction
==================
*/
void CObject::ProcessFunction( class CFunction* InFunction, void* InParams, void* OutResult /* = nullptr */ )
{
	AssertMsg( InFunction && !InFunction->IsUnreachable(), TEXT( "Invalid function to call in %s" ), GetFullName().c_str() );
	AssertMsg( IsInGameThread(), TEXT( "Calling %s from outside game thread" ), InFunction->GetName().c_str() );
	AssertMsg( !CObjectPackage::GetObjectSerializeContext().IsRoutingPostLoad(), TEXT( "Cannot call LifeScript while PostLoading objects (object: % / function: %s)" ), GetFullName().c_str(), InFunction->GetFullName().c_str() );

	// Reject a call if object is pending kill
	if ( IsPendingKill() )
	{
		return;
	}

	// Check on we got all parameters for function
	Assert( InFunction->GetPropertiesSize() == 0 || InParams );

	// Create a new local execution stack
	ScriptFrame		newStack( this, InFunction, 0, ( byte* )Memory_Alloca( InFunction->GetPropertiesSize() ) );
	Assert( newStack.localData || InFunction->GetPropertiesSize() == 0 );

	// Initialize function parameters
	Memory::Memcpy( newStack.localData, InParams, InFunction->GetPropertiesSize() );

	// Call native function or CObject::ProcessInternal
	ScriptFn_t		FunctionFn = InFunction->GetFunction();
	( this->*FunctionFn )( newStack, ( byte* )OutResult );
}

/*
==================
CObject::CallFunction
==================
*/
void CObject::CallFunction( struct ScriptFrame& InStack, RESULT_DECLARE, CFunction* InFunction )
{
	// Call regular native function
	if ( InFunction->HasAnyFunctionFlags( FUNC_Native ) )
	{
		ScriptFn_t		FunctionFn = InFunction->GetFunction();
		( this->*FunctionFn )( InStack, InResult );
	}
	// Otherwise call script function
	else
	{
		ScriptFrame		newStack( this, InFunction, 0, ( byte* )Memory_Alloca( InFunction->GetPropertiesSize() ), &InStack );
		Memory::Memzero( newStack.localData, InFunction->GetPropertiesSize() );

		// Copy function parameters from bytecode
		for ( TFieldIterator<CProperty> it( InFunction, false ); it && *InStack.bytecode != OP_EndFunctionParms; ++it )
		{
			// Copy the result of the expression for this parameter into the appropriate part of the local variable space
			byte*			paramData		= newStack.localData + it->GetOffset();
			Assert( paramData );
			InStack.Step( InStack.object, paramData );
		}

		// Skip OP_EndFunctionParms
		++InStack.bytecode;

		// Execute the code
		ProcessInternal( newStack, InResult );
	}
}

/*
==================
CObject::ProcessInternal
==================
*/
void CObject::ProcessInternal( ScriptFrame& InStack, RESULT_DECLARE )
{
	byte		buffer[MAX_SIMPLE_RETURN_VALUE_SIZE];
	while ( *InStack.bytecode != OP_Return )
	{
		InStack.Step( InStack.object, buffer );
	}
}

//
// LifeScript opcodes
//

#define IMPLEMENT_FUNCTION( InFuncName ) void CObject::exec##InFuncName( struct ScriptFrame& InStack, RESULT_DECLARE )
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
	// Call a function
	CallFunction( InStack, InResult, ( CFunction* )InStack.ReadObject() );
}

/*
==================
Opcode OP_Return
==================
*/
IMPLEMENT_FUNCTION( Return )
{
	AssertMsg( false, TEXT( "Need implement" ) );
}

/*
==================
Opcode OP_IntConst
==================
*/
IMPLEMENT_FUNCTION( IntConst )
{
	*( uint32* )InResult = InStack.ReadInt32();
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
	STACKFRAME_GET_FINISH;
	Logf( TEXT( "Stack Trace:\n%s\n" ), InStack.GetStackTrace().c_str() );
}
#undef IMPLEMENT_FUNCTION