#include "Logger/LoggerMacros.h"
#include "Reflection/ReflectionEnvironment.h"
#include "Reflection/Object.h"
#include "Reflection/Function.h"
#include "System/ThreadingBase.h"
#include "System/ScriptFrame.h"

IMPLEMENT_CLASS( CObject )

/*
==================
CObject::CObject
==================
*/
CObject::CObject()
	: name( NoInit )
{}

/*
==================
CObject::CObject
==================
*/
CObject::CObject( const CName& InName, ObjectFlags_t InFlags /* = OBJECT_None */ )
	: index( INDEX_NONE )
	, name( InName )
	, outer( nullptr )
	, flags( InFlags )
	, theClass( nullptr )
{}

/*
==================
CObject::~CObject
==================
*/
CObject::~CObject()
{
	if ( index != INDEX_NONE )
	{
		// Destroy the object if necessary
		ConditionalBeginDestroy();

		// Remove object from the GC
		CObjectGC::Get().RemoveObject( this );
	}
}

/*
==================
CObject::operator delete
==================
*/
void CObject::operator delete( void* InObject, size_t InSize )
{
	// Do nothing if we're deleting NULL
	if ( InObject )
	{
		free( InObject );
	}
}

/*
==================
CObject::BeginDestroy
==================
*/
void CObject::BeginDestroy()
{}

/*
==================
CObject::FinishDestroy
==================
*/
void CObject::FinishDestroy()
{}

/*
==================
CObject::IsReadyForFinishDestroy
==================
*/
bool CObject::IsReadyForFinishDestroy() const
{
	return true;
}

/*
==================
CObject::AddReferencedObjects
==================
*/
void CObject::AddReferencedObjects( std::vector<CObject*>& InOutObjectArray )
{}

/*
==================
CObject::ConditionalDestroy
==================
*/
void CObject::ConditionalDestroy()
{
	// Check that the object hasn't been destroyed yet
	if ( !HasAnyObjectFlags( OBJECT_FinishDestroyed ) )
	{
		// Begin the asynchronous object cleanup
		ConditionalBeginDestroy();

		// Wait for the object's asynchronous cleanup to finish
		while ( !IsReadyForFinishDestroy() )
		{
			Sys_Sleep( 0.f );
		}

		// Finish destroying the object
		ConditionalFinishDestroy();
	}
}

/*
==================
CObject::Serialize
==================
*/
void CObject::Serialize( CArchive& InArchive )
{
	if ( InArchive.IsLoading() && InArchive.Ver() < VER_CObjectHasCName )
	{
		std::wstring	tempName;
		InArchive << tempName;
		name = tempName;
		Warnf( TEXT( "Deprecated package version (0x%X). Need to re-save the package '%s', because in the future it may not open\n" ), InArchive.Ver(), InArchive.GetPath().c_str() );
	}
	else
	{
		InArchive << name;
	}

	if ( InArchive.Ver() >= VER_OuterInCObject )
	{
		InArchive << outer;
	}
}

/*
==================
CObject::StaticAllocateObject
==================
*/
CObject* CObject::StaticAllocateObject( class CClass* InClass, CObject* InOuter /* = nullptr */, CName InName /* = NAME_None */, ObjectFlags_t InFlags /* = OBJECT_None */ )
{
	AssertMsg( IsInGameThread(), TEXT( "Can't create %s/%s while outside of game thread" ), InClass->GetName().c_str(), InName.ToString().c_str() );
	if ( !InClass )
	{
		Sys_Errorf( TEXT( "Empty class for object %s\n" ), InName.ToString().c_str() );
		return nullptr;
	}

	// If we try to create abstract object it's wrong and we call error
	if ( InClass->HasAnyClassFlags( CLASS_Abstract ) )
	{
		Sys_Errorf( TEXT( "Class which was marked abstract was trying to be allocated. %s/%s" ), InClass->GetName().c_str(), InName.ToString().c_str() );
		return nullptr;
	}

	// Compose name, if unnamed
	if ( InName == NAME_None )
	{
		InName = InClass->GetCName();
	}

	// Allocated data for a new object
	uint32		alignedSize = Align( InClass->GetPropertiesSize(), InClass->GetMinAlignment() );
	CObject*	object = ( CObject* )malloc( alignedSize );
	Sys_Memzero( ( void* )object, InClass->GetPropertiesSize() );

	// Init object properties
	object->index		= INDEX_NONE;
	object->outer		= InOuter;
	object->name		= InName;
	object->flags		= InFlags;
	object->theClass	= InClass;

	// Add a new object to the GC
	CObjectGC::Get().AddObject( object );
	return object;
}

/*
==================
CObject::StaticConstructObject
==================
*/
CObject* CObject::StaticConstructObject( class CClass* InClass, CObject* InOuter /* = nullptr */, CName InName /* = NAME_None */, ObjectFlags_t InFlags /* = OBJECT_None */ )
{
	// Allocate a new object and call the class constructor
	CObject*	object = StaticAllocateObject( InClass, InOuter, InName, InFlags );
	if ( object )
	{
		InClass->ClassConstructor( object );
	}

	return object;
}

/*
==================
CObject::InternalIsA
==================
*/
bool CObject::InternalIsA( const CClass* InClass ) const
{
	if ( !InClass )
	{
		return false;
	}

	for ( const CClass* tempClass = theClass; tempClass; tempClass = tempClass->GetSuperClass() )
	{
		if ( tempClass == InClass )
		{
			return true;
		}
	}

	return false;
}

/*
==================
CObject::IsValid
==================
*/
bool CObject::IsValid() const
{
	CObjectGC&		objectGC = CObjectGC::Get();
	if ( !this )
	{
		Warnf( TEXT( "NULL object\n" ) );
		return false;
	}
	else if ( !objectGC.IsValidIndex( index ) )
	{
		Warnf( TEXT( "[%s] Invalid object index %i\n" ), GetName().c_str(), index );
		return false;
	}
	else if ( !objectGC.GetObject( index ) )
	{
		Warnf( TEXT( "[%s] Empty slot\n" ), GetName().c_str() );
		return false;
	}
	else if ( objectGC.GetObject( index ) != this )
	{
		Warnf( TEXT( "[%s] Other object in slot. Other is %s\n" ), GetName().c_str(), objectGC.GetObject( index )->GetName().c_str() );
		return false;
	}

	return true;
}

/*
==================
CObject::StaticInitializeClass
==================
*/
void CObject::StaticInitializeClass()
{}

#if WITH_EDITOR
/*
==================
CObject::PostEditChangeProperty
==================
*/
void CObject::PostEditChangeProperty( const PropertyChangedEvenet& InPropertyChangedEvenet )
{}

/*
==================
CObject::CanEditProperty
==================
*/
bool CObject::CanEditProperty( class CProperty* InProperty ) const
{
	return true;
}
#endif // WITH_EDITOR

/*
==================
CObject::CallFunction
==================
*/
void CObject::CallFunction( const CName& InFunctionName )
{
	Assert( theClass );
	CFunction*		function = theClass->FindFunction( InFunctionName );
	AssertMsg( function, TEXT( "Failed to find function %s in %s" ), InFunctionName.ToString().c_str(), GetName().c_str() );
	AssertMsg( IsInGameThread(), TEXT( "Calling %s from outside game thread" ), *function->GetName().c_str() );
	
	// Create a local execution stack
	ScriptFrame		stack( this );

	// Execute script function
	ExecScriptFunction( function, stack );
}

/*
==================
CObject::ExecScriptFunction
==================
*/
void CObject::ExecScriptFunction( class CFunction* InFunction, struct ScriptFrame& InStack )
{
	AssertMsg( InFunction, TEXT( "Invalid function to call in %s" ), GetName().c_str() );

	// Create a new local execution stack
	ScriptFrame			newStack( InStack.object, InFunction, &InStack );

	// Call native function or CObject::ExecScript
	ScriptFn_t		FunctionFn = InFunction->GetFunction();
	( InStack.object->*FunctionFn )( newStack );
}

/*
==================
CObject::ExecScript
==================
*/
void CObject::ExecScript( ScriptFrame& InStack )
{
	while ( *InStack.bytecode != OP_Return )
	{
		InStack.Step( InStack.object );
	}
}

/*
==================
CObject::scrOpcode_Nop
==================
*/
void CObject::scrOpcode_Nop( ScriptFrame& InStack )
{
	// Do nothing
}

/*
==================
CObject::scrOpcode_Call
==================
*/
void CObject::scrOpcode_Call( ScriptFrame& InStack )
{
	// Get pointer to function from bytecode
	uptrint				tmpCode = *( uptrint* )InStack.bytecode;
	CFunction*			function = ( CFunction* )tmpCode;
	InStack.bytecode += sizeof( uptrint );

	// Execute script function
	ExecScriptFunction( function, InStack );
}