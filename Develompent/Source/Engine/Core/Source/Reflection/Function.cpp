#include "Misc/StringConv.h"
#include "Reflection/Class.h"
#include "Reflection/Function.h"
#include "Reflection/Property.h"
#include "Reflection/LinkerLoad.h"
#include "Reflection/LinkerSave.h"

IMPLEMENT_CLASS( CFunction )
IMPLEMENT_DEFAULT_INITIALIZE_CLASS( CFunction )

/*
==================
CFunction::CFunction
==================
*/
CFunction::CFunction()
	: functionFlags( FUNC_None )
	, paramsSize( 0 )
	, numParams( 0 )
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
	, paramsSize( 0 )
	, numParams( 0 )
	, FunctionFn( nullptr )
{}

/*
==================
CFunction::Serialize
==================
*/
void CFunction::Serialize( class CArchive& InArchive )
{
	Super::Serialize( InArchive );
	InArchive << functionFlags;
	InArchive << paramsSize;
	InArchive << numParams;

	// Serialize bytecode size
	uint32		bytecodeSize				= bytecode.size();
	InArchive << bytecodeSize;
	uint32		storageBytecodeSize			= 0;
	uint32		storageBytecodeSizeOffset	= InArchive.Tell();
	InArchive << storageBytecodeSize;

	// Serialize bytecode
	uint32		currentByteCodeIndex		= 0;
	uint32		bytecodeStartOffset			= InArchive.Tell();
	if ( bytecodeSize > 0 )
	{
		// Serialize bytecode with linker
		if ( InArchive.GetLinker() )
		{
			// Load bytecode
			if ( InArchive.IsLoading() )
			{
				// Preallocate size of bytecode array
				bytecode.resize( bytecodeSize );

				// Get linker to use it to load the byte code
				CLinkerLoad*		linkerLoad = ( CLinkerLoad* )InArchive.GetLinker();
				Assert( linkerLoad );

				// Remember original loader archive
				CArchive*			originalLoader = linkerLoad->GetLoader();
			 
				// Preload the bytecode
				std::vector<byte>	tmpBytecode;
				tmpBytecode.resize( storageBytecodeSize );
				InArchive.Serialize( tmpBytecode.data(), tmpBytecode.size() );

				// Force reading from the pre-serialized buffer
				CMemoryReading		memReader( tmpBytecode );
				linkerLoad->SetLoader( &memReader );

				// Serialize bytecode into memory
				while ( currentByteCodeIndex < bytecodeSize )
				{
					SerializeOpcode( currentByteCodeIndex, InArchive );
				}

				// Restore original loader
				linkerLoad->SetLoader( originalLoader );
			}
			// Save bytecode
			else
			{
				// Get linker to use it to save the byte code
				CLinkerSave*		linkerSave = ( CLinkerSave* )InArchive.GetLinker();
				Assert( linkerSave );

				// Remember original saver archive
				CArchive*			originalSaver = linkerSave->GetSaver();

				// Force writing to a buffer
				std::vector<byte>	tmpBytecode;
				CMemoryWriter		memWriter( tmpBytecode );
				linkerSave->SetSaver( &memWriter );

				// Serialize bytecode into memory
				while ( currentByteCodeIndex < bytecodeSize )
				{
					SerializeOpcode( currentByteCodeIndex, InArchive );
				}

				// Restore original saver
				linkerSave->SetSaver( originalSaver );

				// Serialize on HDD the memory bytes
				InArchive.Serialize( tmpBytecode.data(), tmpBytecode.size() );

				// Update storage bytecode size
				uint32			bytecodeEndOffset = InArchive.Tell();
				storageBytecodeSize = bytecodeEndOffset - bytecodeStartOffset;
				InArchive.Seek( storageBytecodeSizeOffset );
				InArchive << storageBytecodeSize;
				InArchive.Seek( bytecodeEndOffset );
			}
		}
		// Serialize bytecode for other things
		else
		{
			while ( currentByteCodeIndex < bytecodeSize )
			{
				SerializeOpcode( currentByteCodeIndex, InArchive );
			}
		}
	}

	// Check on valid bytecode size
	if ( currentByteCodeIndex != bytecodeSize )
	{
		Sys_Error( TEXT( "Script serialization mismatch: got %i, expected %i" ), currentByteCodeIndex, bytecodeSize );
	}
}

/*
==================
CFunction::SerializeOpcode
==================
*/
void CFunction::SerializeOpcode( uint32& InOutByteCodeIndex, CArchive& InArchive )
{
	// Serialize opcode
	SerializeOpcodeParam<byte>( InOutByteCodeIndex, InArchive );
	byte	opcode = bytecode[InOutByteCodeIndex-1];

	// Serialize parameters of opcode
	switch ( opcode )
	{
		// Call a function
	case OP_Call:
		SerializeOpcodeParamPtr<CFunction>( InOutByteCodeIndex, InArchive );	// Function to call
		break;

		// Integer constant
	case OP_IntConst:
		SerializeOpcodeParam<int32>( InOutByteCodeIndex, InArchive );			// Integer value
		break;

		// Local variable
		// Object variable
	case OP_LocalVariable:
	case OP_ObjectVariable:
		SerializeOpcodeParamPtr<CProperty>( InOutByteCodeIndex, InArchive );	// Property
		break;

		// This parameters no have any arguments
	case OP_Nop:
	case OP_Return:
	case OP_EndFunctionParms:
		break;

		// In default case we raise a critical error
	default:
		Sys_Error( TEXT( "Unknown opcode '0x%X' in function '%s'" ), opcode, GetPathName().c_str() );
		break;
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
		FunctionFn = &CObject::ProcessInternal;
	}
	// Otherwise we use native function
	else
	{
		CClass*		ownerClass = GetOwnerClass();
		FunctionFn	= CObject::StaticFindNativeFunction( ownerClass->GetCName(), L_Sprintf( "%sexec%s", TCHAR_TO_ANSI( ownerClass->GetName().c_str() ), TCHAR_TO_ANSI( GetName().c_str() ) ).c_str() );
	}
}