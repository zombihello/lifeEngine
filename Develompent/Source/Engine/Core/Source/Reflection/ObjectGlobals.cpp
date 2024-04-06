#include "Misc/StringTools.h"
#include "Reflection/ObjectGlobals.h"
#include "Reflection/Class.h"

/*
==================
ResolvePackageAndName
==================
*/
bool ResolvePackageAndName( CObject*& InOutPackage, std::wstring& InOutName, bool InIsCreate )
{
	// To make parsing the name easier, replace the subobject delimiter with an extra dot
	{
		size_t		index = 0;
		while ( ( index = InOutName.find( SUBOBJECT_DELIMITER, index ) ) != std::wstring::npos )
		{
			// Make the replacement
			InOutName.replace( index, 1, TEXT( "." ) );
			InOutName.insert( index, TEXT( "." ) );

			// Advance index forward so the next iteration doesn't pick it up as well
			index += 2;
		}
	}

	// Resolve the name
	bool		bSubobjectPath = false;
	size_t		idCurrent = 0;
	while ( ( idCurrent = InOutName.find_first_of( TEXT( "." ), 0 ) ) != std::wstring::npos )
	{
		std::wstring	partialName	= InOutName.substr( 0, idCurrent );

		// If the next part of the name ends in two dots, it indicates that the next object in the path name
		// isn't a top-level object (it's a subobject).
		// For example: Package.Group.Object..Subobject
		if ( InOutName.substr( idCurrent + 1, 1 ) == TEXT( "." ) )
		{
			InOutName		= partialName + InOutName.substr( idCurrent + 1 );
			bSubobjectPath	= true;
			InIsCreate		= false;
		}

		// Create package if we want it
		if ( InIsCreate )
		{
			InOutPackage = CObjectPackage::CreatePackage( InOutPackage, partialName.c_str() );
		}
		// Otherwise try find the package in the memory
		else
		{
			CObject*	newPackage = FindObject<CObjectPackage>( InOutPackage, partialName.c_str() );
			if ( !newPackage )
			{
				newPackage = FindObject<CObject>( InOutPackage == nullptr ? ANY_PACKAGE : InOutPackage, partialName.c_str() );
				if ( !newPackage )
				{
					return bSubobjectPath;
				}
			}

			InOutPackage = newPackage;
		}

		// Advance the name forward, here at the end will be subobject name
		InOutName = InOutName.substr( idCurrent + 1 );
	}

	return true;
}

/*
==================
FindObject
==================
*/
CObject* FindObject( CClass* InClass, CObject* InOuter, const tchar* InName, bool InIsExactClass /*= false*/ )
{
	CObjectGC&		objectGC = CObjectGC::Get();
	if ( CObjectPackage::IsSavingPackage() || objectGC.IsGarbageCollecting() )
	{
		Sys_Error( TEXT( "Illegal call to FindObject() while serializing object data or garbage collecting!" ) );
	}

	// Resolve the object and package name
	CObject*		package = InOuter != ANY_PACKAGE ? InOuter : nullptr;
	std::wstring	name = InName;
	if ( !ResolvePackageAndName( package, name, false ) )
	{
		return nullptr;
	}

	// Find object in the memory
	return FindObjectFast( InClass, package, name, InIsExactClass, InOuter == ANY_PACKAGE );
}

/*
==================
LoadObject
==================
*/
CObject* LoadObject( CClass* InClass, CObject* InOuter, const tchar* InName, const tchar* InFilename /*= nullptr*/, uint32 InLoadFlags /*= LOAD_None*/, bool InIsAllowObjectReconciliation /*= true*/ )
{
	Assert( InClass );
	Assert( InName );
	std::wstring	name = InName;
	CObject*		result = nullptr;
	const bool		bContainsObjectName = !!L_Strstr( InName, TEXT( "." ) );

	// Break up the name into packages, returning the innermost name and its outer
	ResolvePackageAndName( InOuter, name, true );
	if ( InOuter )
	{
		// If we have a full object name then attempt to find the object in memory first
		if ( InIsAllowObjectReconciliation && bContainsObjectName )
		{
			result = FindObjectFast( InClass, InOuter, name.c_str() );

			// Object needs loading so load it before returning
			if ( result && result->HasAnyObjectFlags( OBJECT_NeedLoad | OBJECT_NeedPostLoad ) )
			{
				result = nullptr;
			}
		}

		// Load the object from a package
		if ( !result )
		{
			CObjectPackage::BeginLoadPackage();

			// The outermost will be the root package for the file
			CObjectPackage*		topOuter = InOuter->GetOutermost();

			// Find or create a liner object which contains the object being loaded
			CLinkerLoad*		linker = CLinkerLoad::GetPackageLinker( topOuter, InFilename, InLoadFlags );

			// If we have a linker for the outermost package, use it to get the object from its export table
			if ( linker )
			{
				// Make sure the InOuter has been loaded off disk so that it has a LinkerIndex
				bool	bHaveError = false;
				if ( InOuter != topOuter && InOuter->GetLinkerIndex() == INDEX_NONE )
				{
					CObject*	loadedObject = LoadObject( InOuter->GetClass(), nullptr, InOuter->GetPathName().c_str(), InFilename, InLoadFlags, false );

					// If the outer failed to load, or was otherwise a different object than what ResolvePackageAndName made, 
					// this is error
					if ( loadedObject != InOuter || loadedObject->GetLinkerIndex() == INDEX_NONE )
					{
						// We can't call create without a properly loaded outer
						Warnf( TEXT( "The outer object '%s' for '%s' couldn't be loaded (while loading package '%s'): %s\n" ),
							   InOuter->GetFullName().c_str(), InName, topOuter->GetName().c_str(),
							   loadedObject != InOuter 
							   ? L_Sprintf( TEXT( "Incorrect class for outer, found object '%s'" ), loadedObject->GetFullName().c_str() ).c_str()
							   : L_Sprintf( TEXT( "Invalid linker index, couldn't load '%s'?" ), loadedObject->GetFullName().c_str() ).c_str() );
						bHaveError = true;
					}
				}

				// If no error yet, create it
				if ( !bHaveError )
				{
					CObject* newOuter	= InOuter;
					size_t	 idCurrent	= 0;
					while ( ( idCurrent = name.find_first_of( TEXT( "." ), 0 ) ) != std::wstring::npos )
					{
						std::wstring	partialName = name.substr( 0, idCurrent );
						name			= name.substr( idCurrent + 1 );
						newOuter		= linker->CreateObject( CObject::StaticClass(), partialName.c_str(), newOuter );
					}

					// InOuter should now be properly loaded with a linker index, so we can use it to create the actual desired object
					result = linker->CreateObject( InClass, name.c_str(), newOuter ? newOuter : InOuter );
				}
			}

			CObjectPackage::EndLoadPackage();
		}
	}

	return result;
}

/*
==================
LoadClass
==================
*/
CClass* LoadClass( CClass* InBaseClass, CObject* InOuter, const tchar* InName, const tchar* InFilename /*= nullptr*/, uint32 InLoadFlags /*= LOAD_None*/ )
{
	Assert( InBaseClass );
	CClass*		theClass = LoadObject<CClass>( InOuter, InName, InFilename, InLoadFlags );
	if ( theClass && !theClass->IsChildOf( InBaseClass ) )
	{
		// Return NULL class because it's error
		Warnf( TEXT( "Failed to load class '%s' because it is not a child class of '%s'\n" ), theClass->GetFullName().c_str(), InBaseClass->GetFullName().c_str() );
		theClass = nullptr;
	}

	return theClass;
}