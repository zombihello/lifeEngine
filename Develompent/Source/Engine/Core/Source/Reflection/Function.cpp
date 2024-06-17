#include "Misc/StringConv.h"
#include "Reflection/Class.h"
#include "Reflection/Function.h"
#include "Scripts/ScriptVM.h"
#include "System/MemoryArchive.h"

IMPLEMENT_CLASS( CFunction )
IMPLEMENT_DEFAULT_INITIALIZE_CLASS( CFunction )

/*
==================
CFunction::CFunction
==================
*/
CFunction::CFunction()
	: functionFlags( FUNC_None )
	, FunctionFn( nullptr )
{}

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
	GetOuterCClass()->AddFunction( this );
}

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
				Memory::Memcpy( &function, &bytecode[index + 1], sizeof( uptrint ) );
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
CFunction::Bind
==================
*/
void CFunction::Bind()
{
	// If the function isn't native we use CObject's method ExecScript to execute bytecode
	Super::Bind();
	if ( !HasAnyFunctionFlags( FUNC_Native ) )
	{
		// Use LifeScript processing function
		FunctionFn = &CScriptVM::ProcessInternal;
	}
	// Otherwise we use native function
	else
	{
		CClass*		ownerClass = GetOwnerClass();
		FunctionFn	= CScriptVM::StaticFindNativeFunction( ownerClass->GetCName(), L_Sprintf( "%sexec%s", TCHAR_TO_ANSI( ownerClass->GetName().c_str() ), TCHAR_TO_ANSI( GetName().c_str() ) ).c_str() );
	}
}