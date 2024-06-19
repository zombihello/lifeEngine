#include "Logger/LoggerMacros.h"
#include "Reflection/Enum.h"
#include "Reflection/ObjectIterator.h"
#include "Reflection/ObjectGlobals.h"
#include "Reflection/Object.h"
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

	// Create function wrappers (no inner properties yet)
	if ( !CreateFunctions() )
	{
		return false;
	}

	// Bind super functions to finish function graph
	if ( !BindSuperFunctions() )
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
		const std::vector<ScriptClassStubPtr_t>&	classes = stubs.GetClasses();
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
	uint32								reservedInheritanceLevel = 32;
	std::vector<ScriptClassStubPtr_t>	stack;
	std::vector<ScriptClassStubPtr_t>	orderedClasses;
	std::vector<ScriptClassStubPtr_t>&	classes		= stubs.GetClasses();
	uint32								numClasses	= classes.size();
	orderedClasses.reserve( numClasses );
	stack.reserve( reservedInheritanceLevel );

	// Fill map of unprocessed classes
	std::unordered_map<CName, ScriptClassStubPtr_t, CName::HashFunction>	unprocessedClasses;
	for ( uint32 index = 0; index < numClasses; ++index )
	{
		const ScriptClassStubPtr_t&		theClass = classes[index];
		if ( !unprocessedClasses.insert( std::make_pair( theClass->GetName(), theClass ) ).second )
		{
			return false;
		}
	}

	// Sort classes
	for ( uint32 index = 0; index < numClasses; ++index )
	{
		const ScriptClassStubPtr_t&			theClass = classes[index];
		auto								it = unprocessedClasses.find( theClass->GetName() );
		while ( it != unprocessedClasses.end() )
		{
			ScriptClassStubPtr_t			foundClass = it->second;
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
		ScriptClassStubPtr_t		foundClass = stubs.FindClass( theClass->GetSuperClassName() );
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
	bool						bIsCObjectClass	= className == CObject::StaticClass()->GetName();

	// If type with same name already defined then its error.
	// Only one exception: native classes may be already defined in the system by Classes/*.h. In this case
	// we check for missing flags 'CLASS_Parsed' and 'CLASS_Intrinsic'. 
	// If they aren't then all right. Otherwise this is redefinition and its wrong
	CClass*		theClass = nullptr;
	{
		// If we find existing CClass, CStruct or CEnum with same name in script package then it is compile error
		theClass = FindObjectFast<CClass>( scriptPackage, className.c_str(), true );
		if ( ( theClass && theClass->HasAnyClassFlags( CLASS_Parsed | CLASS_Intrinsic ) ) || FindObjectFast<CStruct>( scriptPackage, className.c_str(), true ) || FindObjectFast<CEnum>( scriptPackage, className.c_str(), true ) )
		{
			Errorf( TEXT( "%s: Type '%s' is already defined\n" ), context.ToString().c_str(), className.c_str() );
			return false;
		}
	}

	// If our class is a CObject, it must already exist in the system
	if ( bIsCObjectClass && !theClass )
	{
		Errorf( TEXT( "%s: In the system not found CObject class, something went wrong\n" ), context.ToString().c_str() );
		return false;
	}

	// Find the super class as specified (only for CObject class we set in to NULL)
	CClass*		superClass = !bIsCObjectClass ? FindObjectFast<CClass>( nullptr, superClassName.c_str(), true, true ) : nullptr;

	// Check a few expression if the class isn't CObject
	if ( !bIsCObjectClass )
	{
		// All classes must be inherit from CObject
		if ( superClassName.empty() || ( superClass && !IsA<CObject>( superClass ) ) )
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

	// Find the within class as specified
	CClass*		withinClass = nullptr;
	{
		std::wstring	withinClassName = InClassStub.GetWithinClassName();
		if ( !withinClassName.empty() )
		{
			// CObject class can not have a within class
			if ( bIsCObjectClass )
			{
				Errorf( TEXT( "%s: CObject class can not have a within class\n" ), context.ToString().c_str() );
				return false;
			}

			withinClass = FindObjectFast<CClass>( nullptr, withinClassName.c_str(), true, true );
			if ( !withinClass )
			{
				Errorf( TEXT( "%s: Within class '%s' not found\n" ), context.ToString().c_str(), withinClassName.c_str() );
				return false;
			}
		}
		else
		{
			// For usual classes without a within class we take it from the super
			if ( !bIsCObjectClass )
			{
				withinClass = superClass->GetWithinClass();
				if ( !withinClass )
				{
					Errorf( TEXT( "%s; Invalid within class in super class '%s'\n" ), context.ToString().c_str(), superClassName.c_str() );
					return false;
				}
			}
			// For CObject we use CObject::WithinClass class
			else
			{
				withinClass = CObject::WithinClass::StaticClass();
			}
		}

	}

	// If it is script class, we calculate properties size, min alignment and create a new CClass
	uint32		propertiesSize	= theClass ? theClass->GetPropertiesSize() : 0;
	uint32		minAlignment	= theClass ? theClass->GetMinAlignment() : 1;
	if ( !theClass )
	{
		// For script class we manually calculate it	
		// TODO yehor.pohuliaka - When will be implemented support of properties in classes then need add they here
		propertiesSize	= superClass->GetPropertiesSize();
		minAlignment	= superClass->GetMinAlignment();
		theClass		= new( scriptPackage, className, OBJECT_Public ) CClass();
	}

	// Check for cyclic dependencies
	if ( IsA( theClass, superClass ) )
	{
		Errorf( TEXT( "%s: Super class '%s' creates cyclic dependency for '%s'\n" ), context.ToString().c_str(), superClassName.c_str(), className.c_str() );
		return false;
	}

	// Update data in the class
	theClass->SetClassFlags( InClassStub.GetFlags() );
	theClass->SetPropertiesSize( propertiesSize );
	theClass->SetMinAlignment( minAlignment );
	theClass->SetSuperClass( superClass );
	theClass->SetWithinClass( withinClass );

	// Bind the class and remember one in the stub
	theClass->Bind();
	InClassStub.SetCreatedClass( theClass );

	// Remove 'OBJECT_Transient' flag so that native classes can be stored in a package
	// and mark the one as parsed
	theClass->RemoveObjectFlag( OBJECT_Transient );
	theClass->AddClassFlag( CLASS_Parsed );
	
	// Class created
	return true;
}

/*
==================
CScriptEnvironmentBuilder::CreateFunctions
==================
*/
bool CScriptEnvironmentBuilder::CreateFunctions()
{
	bool	bNoErrors = true;
	
	// Create class functions
	{
		const std::vector<ScriptClassStubPtr_t>&			classes = stubs.GetClasses();
		for ( uint32 index = 0, count = classes.size(); index < count; ++index )
		{
			ScriptClassStubPtr_t							classStub = classes[index];
			const std::vector<ScriptFunctionStubPtr_t>&		functions = classStub->GetFunctions();
			for ( uint32 funcIdx = 0, numFunctions = functions.size(); funcIdx < numFunctions; ++funcIdx )
			{
				bNoErrors &= CreateFunction( *classStub.Get(), *functions[funcIdx].Get() );
			}
		}
	}

	// Done
	return bNoErrors;
}

/*
==================
CScriptEnvironmentBuilder::BindSuperFunctions
==================
*/
bool CScriptEnvironmentBuilder::BindSuperFunctions()
{
	bool	bNoErrors = true;

	// Bind super class functions
	{
		const std::vector<ScriptClassStubPtr_t>&	classes = stubs.GetClasses();
		for ( uint32 index = 0, count = classes.size(); index < count; ++index )
		{
			ScriptClassStubPtr_t							classStub = classes[index];
			const std::vector<ScriptFunctionStubPtr_t>&		functions = classStub->GetFunctions();
			for ( uint32 funcIdx = 0, numFunctions = functions.size(); funcIdx < numFunctions; ++funcIdx )
			{
				bNoErrors &= BindSuperFunction( *classStub.Get(), *functions[funcIdx].Get() );
			}
		}
	}

	// Done
	return bNoErrors;
}

/*
==================
CScriptEnvironmentBuilder::CreateFunction
==================
*/
bool CScriptEnvironmentBuilder::CreateFunction( CScriptClassStub& InClassStub, CScriptFunctionStub& InFunctionStub )
{
	const std::wstring&			className = InClassStub.GetName();
	const std::wstring&			functionName = InFunctionStub.GetName();
	const ScriptFileContext&	context = InFunctionStub.GetContext();
	CClass*						theClass = InClassStub.GetCreatedClass();

	// Class failed to compile
	if ( !theClass )
	{
		return false;
	}

	// If function is native but the class isn't then this is error
	//if ( !InClassStub.IsNative() && InFunctionStub.IsNative() )
	if ( !InClassStub.HasAnyFlags( CLASS_Native ) && InFunctionStub.HasAnyFlags( FUNC_Native ) )
	{
		Errorf( TEXT( "%s: Native function '%s' can be only in a native class\n" ), context.ToString().c_str(), functionName.c_str() );
		return false;
	}

	// Make sure we don't override function in the same class
	if ( theClass->FindFunction( functionName.c_str(), false ) )
	{
		Errorf( TEXT( "%s: Function '%s' is already defined in class '%s'\n" ), context.ToString().c_str(), functionName.c_str(), className.c_str() );
		return false;
	}

	// Create function
	CFunction*		function = new( theClass, functionName.c_str(), OBJECT_Public ) CFunction( InFunctionStub.GetFlags() );
	function->Bind();
	InFunctionStub.SetCreatedFunction( function );

	// Done
	return true;
}

/*
==================
CScriptEnvironmentBuilder::CreateFunction
==================
*/
bool CScriptEnvironmentBuilder::BindSuperFunction( CScriptClassStub& InClassStub, CScriptFunctionStub& InFunctionStub )
{
	const std::wstring&		functionName = InFunctionStub.GetName();
	CClass*					theClass = InClassStub.GetCreatedClass();
	CFunction*				function = InFunctionStub.GetCreatedFunction();

	// Class failed to compile
	if ( !theClass || !function )
	{
		return false;
	}

	// Get the super class
	CClass* superClass = theClass->GetSuperClass();
	if ( !superClass )
	{
		return true;
	}

	// Find the function with the same name in the super class
	CFunction*		superFunction = superClass->FindFunction( functionName.c_str() );
	if ( superFunction )
	{
		// TODO yehor.pohuliaka - Add here checks when will be implemented support of function's arguments and return type

		// Bind as super function
		function->SetSuperFunction( superFunction );
	}

	// Done
	return true;
}