#include "Misc/ScriptOpcodes.h"
#include "System/MemoryArchive.h"
#include "Containers/StringConv.h"
#include "Containers/String.h"
#include "Reflection/ReflectionEnvironment.h"
#include "Reflection/Object.h"
#include "Reflection/Function.h"

IMPLEMENT_CLASS( CFunction )
IMPLEMENT_DEFAULT_INITIALIZE_CLASS( CFunction )

// Init native functions to execute script opcode
void ( CObject::*CFunction::OpcodeFunctions[OP_Num] )( ScriptFrame& InStack ) =
{
	&CObject::scrOpcode_Nop,		// OP_Nop
	&CObject::scrOpcode_Call		// Op_Call
};


/*
==================
CFunction::Serialize
==================
*/
void CFunction::Serialize( class CArchive& InArchive )
{
	Super::Serialize( InArchive );
	InArchive << functionFlags;
	
	// Serialize bytecode
	CMemoryWriter	bytecodeProxy( bytecode );
	uint32			bytecodeSize = bytecode.size();
	InArchive << bytecodeSize;
	for ( uint32 index = 0; index < bytecodeSize; ++index )
	{
		// Serialize opcode
		byte	code = OP_Nop;
		uint32	offset = 0;
		if ( InArchive.IsSaving() )
		{
			code = bytecode[index];
		}
		InArchive << code;
		if ( InArchive.IsLoading() )
		{
			bytecodeProxy << code;
		}

		// Serialize parameters of opcode
		switch ( code )
		{
			// Call a function
		case OP_Call:
		{
			CFunction*	function = nullptr;
			if ( InArchive.IsSaving() )
			{
				memcpy( &function, &bytecode[ index+1 ], sizeof( uptrint ) );
			}		
			InArchive << function;
			if ( InArchive.IsLoading() )
			{
				bytecodeProxy << ( uptrint )function;
			}

			Assert( function );
			offset += sizeof( uptrint );
			break;
		}
		}

		index += offset;
	}
}

/*
==================
CFunction::CFunction
==================
*/
CFunction::CFunction( uint32 InFunctionFlags, CFunction* InSuperFunction /* = nullptr */ )
	: CStruct( InSuperFunction )
	, functionFlags( InFunctionFlags )
	, FunctionFn( nullptr )
{
	Cast<CClass>( GetOuter() )->AddFunction( this );
}

/*
==================
CFunction::Bind
==================
*/
void CFunction::Bind()
{
	// If the function isn't native we use CObject's method ExecScript to execute bytecode
	if ( !HasAnyFunctionFlags( FUNC_Native ) )
	{
		// Use LifeScript processing function
		FunctionFn = &CObject::ExecScript;
	}
	// Otherwise we use native function
	else
	{
		CClass*		ownerClass = GetOwnerClass();
		FunctionFn = CReflectionEnvironment::Get().FindNativeFunction( ownerClass->GetCName(), CString::Format( "%sscr%s", TCHAR_TO_ANSI( ownerClass->GetName().c_str() ), TCHAR_TO_ANSI( GetName().c_str() ) ).c_str() );
	}
}