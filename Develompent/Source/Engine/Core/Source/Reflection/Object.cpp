#include "Logger/LoggerMacros.h"
#include "Reflection/ReflectionEnvironment.h"
#include "Reflection/Object.h"
#include "Reflection/Function.h"
#include "Reflection/ObjectPackage.h"
#include "Reflection/ObjectHash.h"
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
CObject::CObject( ENativeConstructor, const CName& InName, const tchar* InPackageName, ObjectFlags_t InFlags /* = OBJECT_None */ )
	: index( INDEX_NONE )
	, name( InName )
	, outer( nullptr )
	, flags( InFlags | OBJECT_Native | OBJECT_RootSet | OBJECT_DisregardForGC )
	, theClass( nullptr )
{
	Assert( sizeof( outer ) >= sizeof( InPackageName ) );
	*( const tchar** )&outer = InPackageName;
}

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
		ConditionalDestroy();

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
{
	// Remove from linker's export table
	SetLinker( nullptr, INDEX_NONE );
}

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
CObject::PreSave
==================
*/
void CObject::PreSave()
{}

/*
==================
CObject::MarkPackageDirty
==================
*/
void CObject::MarkPackageDirty() const
{
	// Since transient objects will never be saved into a package, there is no need to mark a package dirty
	// if we're transient
	if ( !HasAnyObjectFlags( OBJECT_Transient ) )
	{
		CObjectPackage*		package = GetOutermost();
		if ( package )
		{
			package->SetDirtyFlag( true );
		}
	}
}

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
	// Serialize name and outer
	if ( InArchive.IsLoading() )
	{
		CName		loadName = name;
		CObject*	loadOuter = outer;
		if ( InArchive.Ver() < VER_CObjectHasCName )
		{
			std::wstring	tempName;
			InArchive << tempName;
			loadName = tempName;
			Warnf( TEXT( "Deprecated package version (0x%X). Need to re-save the package '%s', because in the future it may not open\n" ), InArchive.Ver(), InArchive.GetPath().c_str() );
		}
		else
		{
			InArchive << loadName;
		}

		if ( InArchive.Ver() >= VER_OuterInCObject )
		{
			InArchive << outer;
		}

		// If the name we loaded is different from the current one,
		// unhash the object, change the name and hash it again
		bool	bDifferentName	= name != NAME_None && loadName != name;
		bool	bDifferentOuter = loadOuter != outer;
		if ( bDifferentName || bDifferentOuter )
		{
			UnhashObject( this );
			name = loadName;
			outer = loadOuter;
			HashObject( this );
		}
	}
	else
	{
		InArchive << name << outer;
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

	if ( InClass->GetIndex() == INDEX_NONE )
	{
		Sys_Errorf( TEXT( "Unregistered class for %s" ), InName.ToString().c_str() );
		return nullptr;
	}

	// If we try to create abstract object it's wrong and we call error
	if ( InClass->HasAnyClassFlags( CLASS_Abstract ) )
	{
		Sys_Errorf( TEXT( "Class which was marked abstract was trying to be allocated. %s/%s" ), InClass->GetName().c_str(), InName.ToString().c_str() );
		return nullptr;
	}

	if ( InOuter && !IsA( InOuter, InClass->GetWithinClass() ) )
	{
		Sys_Errorf( TEXT( "Object %s of class %s not within %s" ), InName.ToString().c_str(), InClass->GetName().c_str(), InClass->GetWithinClass()->GetName().c_str() );
		return nullptr;
	}

	// Compose name, if unnamed
	CObject*	object = nullptr;
	if ( InName == NAME_None )
	{
		InName = MakeUniqueObjectName( InOuter, InClass, InClass->GetCName() );
	}
	else
	{
		// See if object already exists
		object = FindObjectFast( InClass, InOuter, InName, true );
		Assert( !object );		// TODO yehor.pohuliaka: Need implement support of replacing an object
	}

	// Allocated data for a new object
	uint32		alignedSize = Align( InClass->GetPropertiesSize(), InClass->GetMinAlignment() );
	object 		= ( CObject* )malloc( alignedSize );
	Sys_Memzero( ( void* )object, InClass->GetPropertiesSize() );

	// Init object properties
	object->index		= INDEX_NONE;
	object->outer		= InOuter;
	object->name		= InName;
	object->flags		= InFlags;
	object->theClass	= InClass;

	// Add a new object to the GC and hash object to table
	CObjectGC::Get().AddObject( object );
	HashObject( object );
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
CObject::MakeUniqueObjectName
==================
*/
CName CObject::MakeUniqueObjectName( CObject* InOuter, CClass* InClass, CName InBaseName /* = NAME_None */ )
{
	Assert( InClass );
	uint32	baseNameIndex = InBaseName.GetIndex();
	CName	testName;
	do
	{
		// Create the next name in the sequence for this class
		testName = CName( ( EName )baseNameIndex, ++InClass->classUnique );
	}
	while ( FindObjectFast( nullptr, InOuter, testName, false, InOuter == ANY_PACKAGE ) );
	return testName;
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
CObject::InternalIsIn
==================
*/
bool CObject::InternalIsIn( const CObject* InOuter ) const
{
	if ( !InOuter )
	{
		return true;
	}

	for ( const CObject* curObject = GetOuter(); curObject; curObject = curObject->GetOuter() )
	{
		if ( curObject == InOuter )
		{
			return true;
		}
	}

	return false;
}

/*
==================
CObject::InternalIsInA
==================
*/
bool CObject::InternalIsInA( const CClass* InClass ) const
{
	if ( !InClass )
	{
		return true;
	}

	for ( const CObject* curObject = this; curObject; curObject = curObject->GetOuter() )
	{
		if ( IsA( ( CObject* )curObject, ( CClass* )InClass ) )
		{
			return true;
		}
	}

	return false;
}

/*
==================
CObject::GetFullName
==================
*/
std::wstring CObject::GetFullName( const CObject* InStopOuter /* = nullptr */ ) const
{
	std::wstring	result;
	if ( this )
	{
		result.reserve( 128 );
		GetClass()->AppendName( result );
		result += TEXT( " " );
		GetPathName( InStopOuter, result );
	}
	else
	{
		result += TEXT( "None" );
	}
	return result;
}

/*
==================
CObject::GetPathName
==================
*/
std::wstring CObject::GetPathName( const CObject* InStopOuter /* = nullptr */ ) const
{
	std::wstring	result;
	GetPathName( InStopOuter, result );
	return result;
}

/*
==================
CObject::GetPathName
==================
*/
void CObject::GetPathName( const CObject* InStopOuter, std::wstring& OutResult ) const
{
	if ( this != InStopOuter && this )
	{
		if ( outer && outer != InStopOuter )
		{
			outer->GetPathName( InStopOuter, OutResult );

			// SUBOBJECT_DELIMITER is used to indicate that this object's outer is not a CPackage
			if ( outer->GetClass() != CObjectPackage::StaticClass() && outer->GetOuter()->GetClass() == CObjectPackage::StaticClass() )
			{
				OutResult += SUBOBJECT_DELIMITER;
			}
			else
			{
				OutResult += TEXT( "." );
			}
		}
		AppendName( OutResult );
	}
	else
	{
		OutResult += TEXT( "None" );
	}
}

/*
==================
CObject::GetOutermost
==================
*/
CObjectPackage* CObject::GetOutermost() const
{
	CObject*	topObject = nullptr;
	for ( topObject = ( CObject* )this; topObject && topObject->GetOuter(); topObject = topObject->GetOuter() );
	
#if !SHIPPING_BUILD
	if ( !topObject || !IsA<CObjectPackage>( topObject ) )
	{
		Sys_Errorf( TEXT( "Cast ot %s to CObjectPackage failed" ), topObject ? topObject->GetName().c_str() : TEXT( "NULL" ) );
	}
#endif // !SHIPPING_BUILD

	return ( CObjectPackage* )topObject;
}

/*
==================
CObject::GetTypedOuter
==================
*/
CObject* CObject::GetTypedOuter( CClass* InTargetClass ) const
{
	CObject*	result = nullptr;
	for ( CObject* nextOuter = GetOuter(); !result && nextOuter; nextOuter = nextOuter->GetOuter() )
	{
		if ( IsA( nextOuter, InTargetClass ) )
		{
			result = nextOuter;
		}
	}

	return result;
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