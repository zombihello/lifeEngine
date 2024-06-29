#include "Misc/StringConv.h"
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
	&CObject::execNop,				// OP_Nop
	&CObject::execCall,				// OP_Call
	&CObject::execReturn,			// OP_Return
	&CObject::execIntConst,			// OP_IntConst
	&CObject::execFloatConst,		// OP_FloatConst
	&CObject::execStringConst,		// OP_StringConst
	nullptr,						// OP_EndFunctionParms
	&CObject::execLocalVariable,	// OP_LocalVariable
	&CObject::execObjectVariable	// OP_ObjectVariable
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
	Assert( InFunction->GetParamsSize() == 0 || InParams );

	// Create a new local execution stack
	ScriptFrame		newStack( this, InFunction, 0, ( byte* )Memory_Alloca( InFunction->GetPropertiesSize() ) );
	Assert( newStack.localData || InFunction->GetParamsSize() == 0 );

	// Initialize function parameters
	Memory::Memcpy( newStack.localData, InParams, InFunction->GetParamsSize() );

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

/*
==================
Opcode OP_FloatConst
==================
*/
IMPLEMENT_FUNCTION( FloatConst )
{
	*( float* )InResult = InStack.ReadFloat();
}

/*
==================
Opcode OP_StringConst
==================
*/
IMPLEMENT_FUNCTION( StringConst )
{
	*( std::wstring* )InResult = ANSI_TO_TCHAR( ( achar* )InStack.bytecode );
	InStack.bytecode += ( ( std::wstring* )InResult )->size() + 1;
}

/*
==================
Opcode OP_LocalVariable
==================
*/
IMPLEMENT_FUNCTION( LocalVariable )
{
	CProperty*		property = ( CProperty* )InStack.ReadObject();
	if ( InResult )
	{
		property->CopyPropertyValue( ( byte* )InResult, InStack.localData + property->GetOffset() );
	}
}

/*
==================
Opcode OP_ObjectVariable
==================
*/
IMPLEMENT_FUNCTION( ObjectVariable )
{
	CProperty*		property = ( CProperty* )InStack.ReadObject();
	if ( InResult )
	{
		property->CopyPropertyValue( ( byte* )this, InStack.localData + property->GetOffset() );
	}
}

//
// LifeScript native functions
//

/*
==================
Log
==================
*/
IMPLEMENT_FUNCTION( Log )
{
#if !NO_LOGGING
	STACKFRAME_GET_STRING( message );
	STACKFRAME_GET_FINISH;
	Logf( TEXT( "%s\n" ), message.c_str());
#endif // !NO_LOGGING
}

/*
==================
Warn
==================
*/
IMPLEMENT_FUNCTION( Warn )
{
#if !NO_LOGGING
	STACKFRAME_GET_STRING( message );
	STACKFRAME_GET_FINISH;
	Warnf( TEXT( "%s\n" ), message.c_str() );
#endif // !NO_LOGGING
}

/*
==================
Error
==================
*/
IMPLEMENT_FUNCTION( Error )
{
#if !NO_LOGGING
	STACKFRAME_GET_STRING( message );
	STACKFRAME_GET_FINISH;
	Errorf( TEXT( "%s\n" ), message.c_str() );
#endif // !NO_LOGGING
}

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