#include "Logger/LoggerMacros.h"
#include "Reflection/ObjectIterator.h"
#include "Scripts/ScriptEnvironmentBuilder.h"

/*
==================
CScriptEnvironmentBuilder::CScriptEnvironmentBuilder
==================
*/
CScriptEnvironmentBuilder::CScriptEnvironmentBuilder( CScriptSystemStub& InStubs, CObjectPackage* InScriptPackage )
	: stubs( InStubs )
	, scriptPackage( InScriptPackage )
{}

/*
==================
CScriptEnvironmentBuilder::Build
==================
*/
bool CScriptEnvironmentBuilder::Build()
{
	// Create top level types, this creates the basic class tree
	if ( !CreateTypes() )
	{
		return false;
	}

	// Done
	return true;
}

/*
==================
CScriptEnvironmentBuilder::CreateTypes
==================
*/
bool CScriptEnvironmentBuilder::CreateTypes()
{
	// Create hierarchy order
	bool	bNoErrors = CreateHierarchyOrder();
	
	// Create classes
	{
		const std::vector<TSharedPtr<CScriptClassStub>>&	classes = stubs.GetClasses();
		for ( uint32 index = 0, count = classes.size(); index < count; ++index )
		{
			bNoErrors &= CreateClass( *classes[index].Get() );
		}
	}

	// Done
	return bNoErrors;
}

/*
==================
CScriptEnvironmentBuilder::CreateHierarchyOrder
==================
*/
bool CScriptEnvironmentBuilder::CreateHierarchyOrder()
{
	uint32										reservedInheritanceLevel = 32;
	std::vector<TSharedPtr<CScriptClassStub>>	stack;
	std::vector<TSharedPtr<CScriptClassStub>>	orderedClasses;
	std::vector<TSharedPtr<CScriptClassStub>>&	classes = stubs.GetClasses();
	uint32										numClasses = classes.size();
	orderedClasses.reserve( numClasses );
	stack.reserve( reservedInheritanceLevel );

	// Fill map of unprocessed classes
	std::unordered_map<CName, TSharedPtr<CScriptClassStub>, CName::HashFunction>	unprocessedClasses;
	for ( uint32 index = 0; index < numClasses; ++index )
	{
		const TSharedPtr<CScriptClassStub>&		theClass = classes[index];
		if ( !unprocessedClasses.insert( std::make_pair( theClass->GetName(), theClass ) ).second )
		{
			return false;
		}
	}

	// Sort classes
	for ( uint32 index = 0; index < numClasses; ++index )
	{
		const TSharedPtr<CScriptClassStub>&			theClass = classes[index];
		auto										it = unprocessedClasses.find( theClass->GetName() );
		while ( it != unprocessedClasses.end() )
		{
			TSharedPtr<CScriptClassStub>			foundClass = it->second;
			unprocessedClasses.erase( it );
			stack.push_back( foundClass );
			it = unprocessedClasses.find( foundClass->GetSuperClassName() );
		}

		for ( int32 index = stack.size() - 1; index >= 0; --index )
		{
			orderedClasses.push_back( stack[index] );
		}
		stack.clear();

		// Check for cyclic dependencies
		TSharedPtr<CScriptClassStub>		foundClass = stubs.FindClass( theClass->GetSuperClassName() );
		while ( foundClass )
		{
			if ( foundClass->GetSuperClassName() == theClass->GetName() )
			{
				Errorf( TEXT( "%s: Super class '%s' creates cyclic dependency for '%s'\n" ), theClass->GetContext().ToString().c_str(), theClass->GetSuperClassName().c_str(), theClass->GetName().c_str() );
				return false;
			}

			foundClass = stubs.FindClass( foundClass->GetSuperClassName() );
		}
	}

	std::swap( orderedClasses, classes );
	return true;
}

/*
==================
CScriptEnvironmentBuilder::CreateClass
==================
*/
bool CScriptEnvironmentBuilder::CreateClass( CScriptClassStub& InClassStub )
{
	const std::wstring&			className		= InClassStub.GetName();
	const std::wstring&			superClassName	= InClassStub.GetSuperClassName();
	const ScriptFileContext&	context			= InClassStub.GetContext();

	// If type with same name already defined then its error.
	// Only one exception: native classes may be already defined in the system by *Classes.h. In this case
	// we check for missing flags 'CLASS_Parsed' and 'CLASS_Intrinsic'. 
	// If they aren't then all right. Otherwise this is redefinition and its wrong
	CClass*		theClass = nullptr;
	{
		// Find existing CClass, CStruct or CEnum with same name in script package
		bool	bExistingStructOrEnum = false;
		for ( TObjectIterator<CField> it; it; ++it )
		{
			CField*		curField = *it;
			if ( IsIn( curField, scriptPackage ) && curField->GetName() == className )
			{
				if ( curField->GetClass()->HasAnyCastFlags( CASTCLASS_CStruct | CASTCLASS_CEnum ) )
				{
					bExistingStructOrEnum = true;
					break;
				}
				else if ( curField->GetClass()->HasAnyCastFlags( CASTCLASS_CClass ) )
				{
					theClass = ( CClass* )curField;
					break;
				}
			}
		}

		if ( ( theClass && theClass->HasAnyClassFlags( CLASS_Parsed | CLASS_Intrinsic ) ) || bExistingStructOrEnum )
		{
			Errorf( TEXT( "%s: Type '%s' is already defined\n" ), context.ToString().c_str(), className.c_str() );
			return false;
		}
	}

	// Find the super class as specified
	CClass*		superClass = nullptr;
	for ( TObjectIterator<CClass> it; it; ++it )
	{
		CClass*		curClass = *it;
		if ( curClass->GetName() == superClassName )
		{
			superClass = ( CClass* )curClass;
			break;
		}
	}

	// All classes must be inherit from CObject
	if ( superClassName.empty() || superClass && !IsA<CObject>( superClass ) )
	{
		Errorf( TEXT( "%s: All classes must be inherit from CObject\n" ), context.ToString().c_str() );
		return false;
	}

	// If we not found the supper class print error message
	if ( !superClass )
	{
		Errorf( TEXT( "%s: Unknown super class '%s'\n" ), context.ToString().c_str(), superClassName.c_str() );
		return false;
	}

	// If this class is native but parent classes are not, it's wrong
	if ( InClassStub.HasAnyFlags( CLASS_Native ) )
	{
		bool	bAllClassesNative = true;
		for ( CClass* tempClass = superClass; tempClass; tempClass = tempClass->GetSuperClass() )
		{
			if ( !tempClass->HasAnyClassFlags( CLASS_Native ) )
			{
				bAllClassesNative = false;
				break;
			}
		}

		if ( !bAllClassesNative )
		{
			Errorf( TEXT( "%s: Native classes cannot expand non-native classes\n" ), context.ToString().c_str() );
			return false;
		}
	}

	// If its script class, we calculate properties size, min alignment and create a new CClass
	uint32		propertiesSize = theClass ? theClass->GetPropertiesSize() : 0;
	uint32		minAlignment = theClass ? theClass->GetMinAlignment() : 1;
	if ( !theClass )
	{
		// For script class we manually calculate it	
		// TODO yehor.pohuliaka - When will be implemented support of properties in classes then need add they here
		propertiesSize	= superClass->GetPropertiesSize();
		minAlignment	= superClass->GetMinAlignment();
		theClass		= new( scriptPackage, className, OBJECT_Public ) CClass( InClassStub.GetFlags(), 0, propertiesSize, minAlignment, superClass, superClass->GetWithinClass() );
	}

	// Check for cyclic dependencies
	if ( IsA( theClass, superClass ) )
	{
		Errorf( TEXT( "%s: Super class '%s' creates cyclic dependency for '%s'\n" ), context.ToString().c_str(), superClassName.c_str(), className.c_str() );
		return false;
	}

	// Find the class's native constructor and remember class in the stub
	theClass->Bind();
	theClass->AddClassFlag( CLASS_Parsed );
	InClassStub.SetCreatedClass( theClass );

	// Class created
	return true;
}