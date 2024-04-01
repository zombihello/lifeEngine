#include <vector>

#include "Logger/LoggerMacros.h"
#include "Misc/CoreGlobals.h"
#include "Reflection/Object.h"
#include "Reflection/ObjectPackage.h"
#include "Reflection/ObjectHash.h"
#include "Reflection/LinkerLoad.h"
#include "Reflection/Class.h"
#include "System/Threading.h"
#include "System/Config.h"

IMPLEMENT_CLASS( CObject )

/*
==================
GetCObjectSubsystemInitialised
==================
*/
static FORCEINLINE bool& GetCObjectSubsystemInitialised()
{
	static bool		s_ObjInitialized = false;
	return s_ObjInitialized;
}

/*
==================
GetObjAutoRegisters
==================
*/
static FORCEINLINE std::vector<CObject*>& GetObjAutoRegisters()
{
	static std::vector<CObject*>	s_ObjAutoRegisters;
	return s_ObjAutoRegisters;
}

/*
==================
GetAutoInitializeRegistrants
==================
*/
static FORCEINLINE std::vector<CObject* (*)()>& GetAutoInitializeRegistrants()
{
	static std::vector<CObject* (*)()>	s_AutoInitializeRegistrants;
	return s_AutoInitializeRegistrants;
}


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
	, flags( InFlags | OBJECT_Public | OBJECT_Native | OBJECT_Transient | OBJECT_RootSet )
	, theClass( nullptr )
{
	Assert( sizeof( outer ) >= sizeof( InPackageName ) );
	*( const tchar** )&outer = InPackageName;

	// Call native registration from constructor if CObject system already initialized
	if ( GetCObjectSubsystemInitialised() )
	{
		Register();
	}
	// Otherwise do it later in CObject::StaticInit
	else
	{
		GetObjAutoRegisters().push_back( this );
	}
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
CObject::PostLoad
==================
*/
void CObject::PostLoad()
{
	// Note that it has propagated
#if !SHIPPING_BUILD
	CObjectPackage::GetObjectSerializeContext().RemoveDebugPostLoad( this );
#endif // !SHIPPING_BUILD
}

/*
==================
CObject::ConditionalPostLoad
==================
*/
void CObject::ConditionalPostLoad()
{
	// PostLoad only if the object needs it and has already been serialized
	Assert( !HasAnyObjectFlags( OBJECT_NeedLoad ) );
	if ( HasAnyObjectFlags( OBJECT_NeedPostLoad ) )
	{
		Assert( IsInGameThread() );

#if !SHIPPING_BUILD
		ObjectSerializeContext&		objectSerializeContext = CObjectPackage::GetObjectSerializeContext();
		Assert( !objectSerializeContext.ContainsDebugPostLoad( this ) );
		objectSerializeContext.AddDebugPostLoad( this );
#endif // !SHIPPING_BUILD

		// Remove OBJECT_NeedPostLoad flag
		RemoveObjectFlag( OBJECT_NeedPostLoad );

		// Call PostLoad
		PostLoad();

		// Check PostLoad route
#if !SHIPPING_BUILD
		if ( objectSerializeContext.ContainsDebugPostLoad( this ) )
		{
			Sys_Errorf( TEXT( "%s failed to route PostLoad. Please call Super::PostLoad() in your <ClassName>::PostLoad() function" ), GetFullName().c_str() );
		}
#endif // !SHIPPING_BUILD
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
{
	if ( !HasAnyObjectFlags( OBJECT_FinishDestroyed ) )
	{
		Sys_Errorf( TEXT( "Trying to call CObject::FinishDestroy from outside of CObject::ConditionalFinishDestroy on object %s. Please fix up the calling code" ), GetFullName().c_str() );
	}

	Assert( !GetLinker() );
	Assert( GetLinkerIndex() == INDEX_NONE );
}

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
CObject::PreSaveRoot
==================
*/
bool CObject::PreSaveRoot( const tchar* InFilename )
{
	return false;
}

/*
==================
CObject::PostSaveRoot
==================
*/
void CObject::PostSaveRoot( bool InIsCleanupIsRequired )
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
	// Since transient objects will never be saved into a package, there is no need to mark a package dirty if we're transient
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
			InArchive << loadOuter;
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

	// Serialize object properties which are defined in the class
	if ( theClass != CClass::StaticClass() )
	{
		theClass->SerializeProperties( InArchive, ( byte* )this );
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

	// If we try to allocate object without class its error
	if ( !InClass )
	{
		Sys_Errorf( TEXT( "Empty class for object %s\n" ), InName.ToString().c_str() );
		return nullptr;
	}

	// If we try to allocate object with unregistered class its error
	if ( InClass->GetIndex() == INDEX_NONE && GetObjAutoRegisters().empty() )
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

	// Only packages can not have an outer, and they must be named explicitly
	if ( !InOuter )
	{
		if ( InClass != CObjectPackage::StaticClass() )
		{
			Sys_Errorf( TEXT( "Only packages can not have an outer. %s/%s" ), InClass->GetName().c_str(), InName.ToString().c_str() );
			return nullptr;
		}
		else if ( InName == NAME_None )
		{
			Sys_Errorf( TEXT( "Package name must be explicitly" ) );
			return nullptr;
		}
	}

	// InOuter must be in InClass->GetWithinClass class
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
	}

	CLinkerLoad*	linker						= nullptr;
	uint32			linkerIndex					= INDEX_NONE;
	bool			bWasConstructedOnOldObject	= false;

	// Create a new object if we not found exist object
	if ( !object )
	{
		// Figure out size, alignment and aligned size of object
		uint32		size = InClass->GetPropertiesSize();

		// Enforce 4 byte alignment to ensure ObjectFlags are never accessed misaligned
		uint32		alignment = Max<uint32>( 4, InClass->GetMinAlignment() );
		uint32		alignedSize = Align( size, alignment );

		//Allocate new memory of the appropriate size and alignment
		object = ( CObject* )malloc( alignedSize );
	}
	// Otherwise we replace an existing object without affecting the original's address
	else
	{
		// We can't use unreachable objects
		Assert( !object->IsUnreachable() );
		Logf( TEXT( "Replacing %s\n" ), object->GetFullName().c_str() );

		// If the object we found is of a different class, can't replace it
		if ( !object->GetClass()->IsChildOf( InClass ) )
		{
			Sys_Errorf( TEXT( "Objects have the same fully qualified name but different paths.\n" )
						TEXT( "\tNew Object: %s %s.%s\n" ) 
						TEXT( "\tExisting Object: %s" ), 
						InClass->GetName().c_str(), InOuter ? InOuter->GetPathName().c_str() : TEXT( "" ), InName.ToString().c_str(),
						object->GetFullName().c_str() );
			return nullptr;
		}

		// Remember linker and flags
		linker		= object->GetLinker();
		linkerIndex = object->GetLinkerIndex();
		InFlags		|= object->GetObjectFlags() & OBJECT_MASK_Keep;
		
		// Destroy the object
		if ( !object->HasAnyObjectFlags( OBJECT_FinishDestroyed ) )
		{
			AssertMsg( !object->HasAnyObjectFlags( OBJECT_NeedLoad | OBJECT_NeedPostLoad ), TEXT( "Replacing a loaded object is not supported: %s (Flags=0x%X)" ),
					   object->GetFullName().c_str(),
					   object->GetObjectFlags() );

			// Get the name before we start the destroy, as destroy renames it
			std::wstring	oldName = object->GetFullName();

			// Begin the asynchronous object cleanup
			object->ConditionalBeginDestroy();

			// Wait for the object's asynchronous cleanup to finish
			bool	bNeedPrintMsg	= false;
			double	stallStart		= 0.0;
			while ( !object->IsReadyForFinishDestroy() )
			{
				// If we're not in the editor, this is fatal
				if ( !bNeedPrintMsg && !g_IsEditor )
				{
					stallStart = Sys_Seconds();
					bNeedPrintMsg = true;
				}
				Sys_Sleep( 0.f );
			}

			// Print warning message if it need
			if ( bNeedPrintMsg )
			{
				double	deltaTime = Sys_Seconds() - stallStart;
				Warnf( TEXT( "Game Thread hitch waiting for resource cleanup on a CObject (%s) overwrite took %6.2fms. Fix the higher level code so that this does not happen\n" ),
					   oldName.c_str(),
					   deltaTime * 1000.f );
			}

			// Finish destroying the object
			object->ConditionalFinishDestroy();
		}

		// Destroy the object
		object->~CObject();
		bWasConstructedOnOldObject = true;
	}

	// If class is transient, objects must be transient
	if ( InClass->classFlags & CLASS_Transient )
	{
		InFlags |= OBJECT_Transient;
	}

	// Clean the object's memory
	Sys_Memzero( ( void* )object, InClass->GetPropertiesSize() );

	// Init object properties
	object->index		= INDEX_NONE;
	object->outer		= InOuter;
	object->name		= InName;
	object->flags		= InFlags;
	object->theClass	= InClass;

	// Reassociate the object with it's linker
	if ( bWasConstructedOnOldObject )
	{
		object->SetLinker( linker, linkerIndex, false );
		if ( linker )
		{
			std::vector<ObjectExport>&		exportMap = linker->GetExports();
			Assert( !exportMap[linkerIndex].object );
			exportMap[linkerIndex].object = object;
		}
	}

	// Add a new object to the GC and hash object to table
	CObjectGC::Get().AddObject( object );
	HashObject( object );
	Assert( object->IsValid() );
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
CObject::AddToAutoInitializeRegistrants
==================
*/
void CObject::AddToAutoInitializeRegistrants( CObject* ( *InStaticInitializeFn )() )
{
	GetAutoInitializeRegistrants().push_back( InStaticInitializeFn );
}

/*
==================
CObject::StaticInit
==================
*/
void CObject::StaticInit()
{
	Assert( !GetCObjectSubsystemInitialised() );

	// Get values from .ini so it is overridable per game/platform and allocate object pool
	uint32 maxObjectsNotConsideredByGC	= g_Config.GetValue( CT_Engine, TEXT( "Engine.GarbageCollectionSettings" ), TEXT( "MaxObjectsNotConsideredByGC" ) ).GetNumber( 0 );
	uint32 maxCObjects					= g_Config.GetValue( CT_Engine, TEXT( "Engine.GarbageCollectionSettings" ), TEXT( "MaxObjectsInGame" ) ).GetNumber( 2 * 1024 * 1024 ) ;	// Default to ~2M CObjects

	// Log what we're doing to track down what really happens
	CObjectGC::Get().AllocateObjectPool( maxCObjects, maxObjectsNotConsideredByGC );
	Logf( TEXT( "Presizing for max %d objects, including %i objects not considered by GC\n" ), maxCObjects, maxObjectsNotConsideredByGC );

	// If statically linked, initialize registrants
	std::vector<CObject* (*)()>		autoInitializeRegistrants = GetAutoInitializeRegistrants();
	for ( uint32 index = 0, count = autoInitializeRegistrants.size(); index < count; ++index )
	{
		autoInitializeRegistrants[index]();
	}
	autoInitializeRegistrants.clear();

	// Mark CObject system as initialized
	GetCObjectSubsystemInitialised() = true;

	// Register all native classes
	std::vector<CObject*>&	objAutoRegisters = GetObjAutoRegisters();
	for ( uint32 index = 0, count = objAutoRegisters.size(); index < count; ++index )
	{
		objAutoRegisters[index]->Register();
	}
	objAutoRegisters.clear();

	Logf( TEXT( "Object subsystem initialized\n" ) );
}

/*
==================
CObject::StaticExit
==================
*/
void CObject::StaticExit()
{
	Assert( GetCObjectSubsystemInitialised() );
	CObjectGC::Get().CollectGarbage( OBJECT_None );

	GetCObjectSubsystemInitialised() = false;
	Logf( TEXT( "Object subsystem successfully closed\n" ) );
}

/*
==================
CObject::StaticIsCObjectInitialized
==================
*/
bool CObject::StaticIsCObjectInitialized()
{
	return GetCObjectSubsystemInitialised();
}

/*
==================
CObject::Register
==================
*/
void CObject::Register()
{
	Assert( GetCObjectSubsystemInitialised() );

	// Create package
	CObjectPackage*		package = ( CObjectPackage* )CObjectPackage::CreatePackage( nullptr, ( const tchar* )GetOuter() );
	Assert( package );
	package->AddPackageFlag( PKG_CompiledIn );
	outer = package;

	// Add to the global object table
	CObjectGC::Get().AddObject( this );

	// Hashing the object
	HashObject( this );

	// Make sure that objects disregarded for GC are part of root set
	Assert( !IsDisregardedForGC() || IsRootSet() );
	Logf( TEXT( "Registered %s\n" ), GetFullName().c_str() );
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
{
	// Mark CObject class reference as persistent object reference so that it (TheClass) doesn't get nulled when a class
	// is marked as pending kill. Nulling TheClass may leave the object in a broken state if it doesn't get GC'd in the same
	// GC call as its class. And even if it gets GC'd in the same call as its class it may break inside of GC
	CClass*		theClass = StaticClass();
	theClass->EmitObjectReference( STRUCT_OFFSET( CObject, outer ), true );
	theClass->EmitObjectReference( STRUCT_OFFSET( CObject, theClass ), true );
}

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

/*
==================
CObject::PostLinkerChange
==================
*/
void CObject::PostLinkerChange()
{}
#endif // WITH_EDITOR