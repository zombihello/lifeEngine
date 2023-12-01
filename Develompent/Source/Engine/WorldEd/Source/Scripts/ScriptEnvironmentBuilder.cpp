#include "Logger/LoggerMacros.h"
#include "Reflection/ReflectionEnvironment.h"
#include "Scripts/ScriptEnvironmentBuilder.h"

/*
==================
CScriptEnvironmentBuilder::CScriptEnvironmentBuilder
==================
*/
CScriptEnvironmentBuilder::CScriptEnvironmentBuilder( CScriptSystemStub& InStubs )
	: stubs( &InStubs )
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
		const std::vector<TSharedPtr<CScriptClassStub>>&	classes = stubs->GetClasses();
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
	std::vector<TSharedPtr<CScriptClassStub>>&	classes = stubs->GetClasses();
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
		TSharedPtr<CScriptClassStub>		foundClass = stubs->FindClass( theClass->GetSuperClassName() );
		while ( foundClass )
		{
			if ( foundClass->GetSuperClassName() == theClass->GetName() )
			{
				Errorf( TEXT( "%s: Super class '%s' creates cyclic dependency for '%s'\n" ), theClass->GetContext().ToString().c_str(), theClass->GetSuperClassName().c_str(), theClass->GetName().c_str() );
				return false;
			}

			foundClass = stubs->FindClass( foundClass->GetSuperClassName() );
		}
	}

	std::swap( orderedClasses, classes );
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
		const std::vector<TSharedPtr<CScriptClassStub>>&			classes = stubs->GetClasses();
		for ( uint32 index = 0, count = classes.size(); index < count; ++index )
		{
			TSharedPtr<CScriptClassStub>							classStub = classes[index];
			const std::vector<TSharedPtr<CScriptFunctionStub>>&		functions = classStub->GetFunctions();
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
		const std::vector<TSharedPtr<CScriptClassStub>>&	classes = stubs->GetClasses();
		for ( uint32 index = 0, count = classes.size(); index < count; ++index )
		{
			TSharedPtr<CScriptClassStub>							classStub = classes[index];
			const std::vector<TSharedPtr<CScriptFunctionStub>>&		functions = classStub->GetFunctions();
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
CScriptEnvironmentBuilder::CreateClass
==================
*/
bool CScriptEnvironmentBuilder::CreateClass( CScriptClassStub& InClassStub )
{
	CReflectionEnvironment&		reflectionEnvironment = CReflectionEnvironment::Get();
	const std::wstring&			className = InClassStub.GetName();
	const std::wstring&			superClassName = InClassStub.GetSuperClassName();
	const ScriptFileContext&	context = InClassStub.GetContext();

	// If type with same name already defined then its error.
	// Only one exception: native classes may be already defined in the system by *Classes.h. In this case
	// we check for missing flags 'CLASS_Parsed' and 'CLASS_Intrinsic'. 
	// If they aren't then all right. Otherwise this is redefinition and its wrong
	CClass*		theClass = reflectionEnvironment.FindClass( className.c_str() );
	{
		bool	bWrongClass = theClass && ( theClass->HasAnyClassFlags( CLASS_Parsed ) || theClass->HasAnyClassFlags( CLASS_Intrinsic ) );
		if ( bWrongClass || reflectionEnvironment.FindStruct( className.c_str() ) || reflectionEnvironment.FindEnum( className.c_str() ) )
		{
			Errorf( TEXT( "%s: Type '%s' is already defined\n" ), context.ToString().c_str(), className.c_str() );
			return false;
		}
	}

	// Find the super class as specified
	CClass*		superClass = reflectionEnvironment.FindClass( superClassName.c_str() );	

	// All classes must be inherit from CObject
	if ( superClassName.empty() || superClass && !superClass->IsA( CObject::StaticClass() ) )
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
	if ( InClassStub.IsNative() )
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
	uint32		propertiesSize	= theClass ? theClass->GetPropertiesSize() : 0;
	uint32		minAlignment	= theClass ? theClass->GetMinAlignment() : 1;
	if ( !theClass )
	{
		// For script class we manually calculate it	
		// TODO yehor.pohuliaka - When will be implemented support of properties in classes then need add they here
		propertiesSize = superClass->GetPropertiesSize();
		minAlignment = superClass->GetMinAlignment();

		theClass = new( nullptr, className ) CClass( 0, 0, propertiesSize, minAlignment, superClass, superClass->GetWithinClass() );
		reflectionEnvironment.AddClass( theClass );
	}

	// Check for cyclic dependencies
	if ( superClass->IsA( theClass ) )
	{
		Errorf( TEXT( "%s: Super class '%s' creates cyclic dependency for '%s'\n" ), context.ToString().c_str(), superClassName.c_str(), className.c_str() );
		return false;
	}

	// Find the class's native constructor and remember class in the stub
	theClass->Bind();
	theClass->AddClassFlag( CLASS_Parsed | ( InClassStub.IsNative() ? CLASS_Native : 0 ) );
	theClass->AddObjectFlag( OBJECT_NeedSave );
	InClassStub.SetCreatedClass( theClass );

	// Class created
	return true;
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
	if ( !InClassStub.IsNative() && InFunctionStub.IsNative() )
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
	CFunction*		function = new( theClass, functionName.c_str() ) CFunction( InFunctionStub.IsNative() ? FUNC_Native : 0 );
	function->Bind();
	function->AddObjectFlag( OBJECT_NeedSave );
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
	const std::wstring&			functionName = InFunctionStub.GetName();
	CClass*						theClass = InClassStub.GetCreatedClass();
	CFunction*					function = InFunctionStub.GetCreatedFunction();

	// Class failed to compile
	if ( !theClass || !function )
	{
		return false;
	}

	// Get the super class
	CClass*		superClass = theClass->GetSuperClass();
	if ( !superClass )
	{
		return true;
	}

	// Find the function with the same name in the super class
	CFunction*	superFunction = superClass->FindFunction( functionName.c_str() );
	if ( superFunction )
	{
		// TODO yehor.pohuliaka - Add here checks when will be implemented support of function's arguments and return type

		// Bind as super function
		function->SetSuperFunction( superFunction );
	}

	// Done
	return true;
}