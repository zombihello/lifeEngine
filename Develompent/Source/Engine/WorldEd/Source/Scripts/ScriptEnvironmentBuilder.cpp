#include "Logger/LoggerMacros.h"
#include "Reflection/ReflectionEnvironment.h"
#include "Scripts/ScriptEnvironmentBuilder.h"
#include "System/Package.h"

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
	
	// Create classes, properties and bind it
	if ( bNoErrors )
	{
		const std::vector<TSharedPtr<CScriptClassStub>>&	classes = stubs->GetClasses();
		
		// Create a class
		for ( uint32 index = 0, count = classes.size(); index < count; ++index )
		{
			bNoErrors &= CreateClass( *classes[index].Get() );
		}

		// Create properties
		for ( uint32 index = 0, count = classes.size(); index < count; ++index )
		{
			bNoErrors &= CreateClassProperties( *classes[index].Get() );
		}

		// Bind the class
		for ( uint32 index = 0, count = classes.size(); index < count; ++index )
		{
			bNoErrors &= BindClass( *classes[index].Get() );
		}
	}

	// Done
	return bNoErrors;
}

/*
==================
CScriptEnvironmentBuilder::CreateClassProperties
==================
*/
bool CScriptEnvironmentBuilder::CreateClassProperties( CScriptClassStub& InClassStub )
{
	CClass*												theClass		= InClassStub.GetCreatedClass();
	const std::vector<TSharedPtr<CScriptPropertyStub>>&	propertyStubs	= InClassStub.GetProperties();

	// Class failed to compile
	if ( !theClass )
	{
		return false;
	}

	// Create class properties
	bool	bNoErrors = true;
	for ( uint32 index = 0, count = propertyStubs.size(); index < count; ++index )
	{
		bNoErrors &= CreateClassProperty( InClassStub, *propertyStubs[index].Get() );
	}

	// Done, all class properties has been created
	return bNoErrors;
}

/*
==================
CScriptEnvironmentBuilder::BindClass
==================
*/
bool CScriptEnvironmentBuilder::BindClass( CScriptClassStub& InClassStub )
{
	CClass*		theClass = InClassStub.GetCreatedClass();

	// Class failed to compile
	if ( !theClass )
	{
		return false;
	}

	// Bind the class
	theClass->Bind();
	return true;
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
			Sys_Errorf( TEXT( "Failed to instert a pair<CName, TSharedPtr<CScriptClassStub>> into unprocessedClasses" ) );
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
				Errorf( TEXT( "Super class '%s' creates cyclic dependency for '%s'\n" ), theClass->GetSuperClassName().c_str(), theClass->GetName().c_str() );
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
CScriptEnvironmentBuilder::CreateClass
==================
*/
bool CScriptEnvironmentBuilder::CreateClass( CScriptClassStub& InClassStub )
{
	CReflectionEnvironment&		reflectionEnvironment = CReflectionEnvironment::Get();
	const std::wstring&			className = InClassStub.GetName();
	const std::wstring&			superClassName = InClassStub.GetSuperClassName();

	// If type with same name already defined then its error
	if ( reflectionEnvironment.FindClass( className.c_str() ) || reflectionEnvironment.FindStruct( className.c_str() ) || reflectionEnvironment.FindEnum( className.c_str() ) )
	{
		Errorf( TEXT( "Type '%s' is already defined\n" ), className.c_str() );
		return false;
	}

	// Find the super class as specified
	CClass*		superClass = reflectionEnvironment.FindClass( superClassName.c_str() );	

	// All classes must be inherit from CObject
	if ( superClassName.empty() || superClass && !superClass->IsA( CObject::StaticClass() ) )
	{
		Errorf( TEXT( "%s: All classes must be inherit from CObject\n" ), className.c_str() );
		return false;
	}

	// If we not found the supper class its error
	if ( !superClass )
	{
		Errorf( TEXT( "%s: Unknown super class '%s'\n" ), className.c_str(), superClassName.c_str() );
		return false;
	}

	// Create a new CClass
	// The class's properties size and min alignment we will recalculate later in CScriptEnvironmentBuilder::CreateClassProperties
	CClass*		theClass = new( stubs->GetPackage(), className ) CClass( 0, 0, superClass->GetPropertiesSize(), superClass->GetMinAlignment(), superClass, superClass->GetWithinClass() );
	reflectionEnvironment.AddClass( theClass );

	// Remember class in the stub
	InClassStub.SetCreatedClass( theClass );

	// Class created
	return true;
}

/*
==================
CScriptEnvironmentBuilder::CreateClassProperty
==================
*/
bool CScriptEnvironmentBuilder::CreateClassProperty( CScriptClassStub& InClassStub, CScriptPropertyStub& InPropertyStub )
{
	CReflectionEnvironment& reflectionEnvironment = CReflectionEnvironment::Get();
	CClass*					theClass = InClassStub.GetCreatedClass();
	const std::wstring&		className = InClassStub.GetName();
	const std::wstring&		propertyName = InPropertyStub.GetName();

	// Class failed to compile
	if ( !theClass )
	{
		return false;
	}

	// If property with same name already defined then its error
	{
		std::vector<CProperty*>		fullClassProperties;
		theClass->GetProperties( fullClassProperties );
		for ( uint32 index = 0, count = fullClassProperties.size(); index < count; ++index )
		{
			CProperty*	property = fullClassProperties[index];
			if ( property->GetName() == propertyName )
			{
				Errorf( TEXT( "%s: Property with name '%s' is already defined in the class or super class\n" ), className.c_str(), propertyName.c_str() );
				return false;
			}
		}
	}

	// Create property
	CProperty*				property			= nullptr;
	uint32					propertiesSize		= theClass->GetPropertiesSize();
	const std::wstring&		propertySubType		= InPropertyStub.GetSubType();
	const std::wstring&		propertyCategory	= InPropertyStub.GetCategory();
	const std::wstring&		propertyDescription = InPropertyStub.GetDescription();
	uint32					propertyFlags		= InPropertyStub.GetFlags();
	uint32					propertyArraySize	= InPropertyStub.GetArraySize();
	switch ( InPropertyStub.GetType() )
	{
		// Byte
	case SPT_Byte:
		property = new( theClass, propertyName ) CByteProperty( propertiesSize, propertyCategory, propertyDescription, propertyFlags, nullptr, propertyArraySize );
		break;

		// Enum
	case SPT_Enum:
	{
		CEnum*	cenum = reflectionEnvironment.FindEnum( propertySubType.c_str() );
		if ( !cenum )
		{
			Errorf( TEXT( "%s: Unknown property type '%s' in '%s'\n" ), className.c_str(), propertySubType.c_str(), propertyName.c_str() );
			return false;
		}

		property = new( theClass, propertyName ) CByteProperty( propertiesSize, propertyCategory, propertyDescription, propertyFlags, cenum, propertyArraySize );
		break;
	}

		// Int
	case SPT_Int:
		property = new( theClass, propertyName ) CIntProperty( propertiesSize, propertyCategory, propertyDescription, propertyFlags, propertyArraySize );
		break;

		// Bool
	case SPT_Bool:
		property = new( theClass, propertyName ) CBoolProperty( propertiesSize, propertyCategory, propertyDescription, propertyFlags, propertyArraySize );
		break;

		// Float
	case SPT_Float:
		property = new( theClass, propertyName ) CFloatProperty( propertiesSize, propertyCategory, propertyDescription, propertyFlags, propertyArraySize );
		break;

		// Color
	case SPT_Color:
		property = new( theClass, propertyName ) CColorProperty( propertiesSize, propertyCategory, propertyDescription, propertyFlags, propertyArraySize );
		break;

		// Vector
	case SPT_Vector:
		property = new( theClass, propertyName ) CVectorProperty( propertiesSize, propertyCategory, propertyDescription, propertyFlags, 0.f, propertyArraySize );
		break;

		// Rotator
	case SPT_Rotator:
		property = new( theClass, propertyName ) CRotatorProperty( propertiesSize, propertyCategory, propertyDescription, propertyFlags, propertyArraySize );
		break;

		// String
	case SPT_String:
		property = new( theClass, propertyName ) CStringProperty( propertiesSize, propertyCategory, propertyDescription, propertyFlags, propertyArraySize );
		break;

		// Object
	case SPT_Object:
	{
		CClass*		propertyClass = reflectionEnvironment.FindClass( propertySubType.c_str() );
		if ( !propertyClass )
		{
			Errorf( TEXT( "%s: Unknown property type '%s' in '%s'\n" ), className.c_str(), propertySubType.c_str(), propertyName.c_str() );
			return false;
		}

		property = new( theClass, propertyName ) CObjectProperty( propertiesSize, propertyCategory, propertyDescription, propertyFlags, propertyClass, propertyArraySize );
		break;
	}

		// Asset
	case SPT_Asset:
	{
		EAssetType		assetType = ConvertTextToAssetType( propertySubType );
		if ( assetType == AT_Unknown )
		{
			Errorf( TEXT( "%s: Unknown asset type '%s' in '%s'\n" ), className.c_str(), propertySubType.c_str(), propertyName.c_str() );
			return false;
		}

		property = new( theClass, propertyName ) CAssetProperty( propertiesSize, propertyCategory, propertyDescription, propertyFlags, assetType, propertyArraySize );
		break;
	}

		// Struct
	case SPT_Struct:
	{
		CStruct*	propertyStruct = reflectionEnvironment.FindStruct( propertySubType.c_str() );
		if ( !propertyStruct )
		{
			Errorf( TEXT( "%s: Unknown property type '%s' in '%s'\n" ), className.c_str(), propertySubType.c_str(), propertyName.c_str() );
			return false;
		}

		property = new( theClass, propertyName ) CStructProperty( propertiesSize, propertyCategory, propertyDescription, propertyFlags, propertyStruct, propertyArraySize );
		break;
	}

		// Unknown and unsupported property types
	case SPT_Array:
	default:
		Errorf( TEXT( "%s: Unsupported property type in '%s'\n" ), className.c_str(), propertyName.c_str() );
		return false;
	}

	// Update the class's properties size and min alignment
	theClass->SetPropertiesSize( propertiesSize + property->GetElementSize() * propertyArraySize );
	if ( property->GetMinAlignment() > theClass->GetMinAlignment() )
	{
		theClass->SetMinAlignment( property->GetMinAlignment() );
	}

	// Done
	return true;
}