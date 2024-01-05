#include "Reflection/ReflectionEnvironment.h"
#include "Reflection/Function.h"
#include "Reflection/Property.h"

IMPLEMENT_CLASS( CClass )

/*
==================
CClass::StaticInitializeClass
==================
*/
void CClass::StaticInitializeClass()
{
	CClass*		theClass = StaticClass();
	theClass->EmitObjectReference( STRUCT_OFFSET( CClass, withinClass ) );
	theClass->EmitObjectArrayReference( STRUCT_OFFSET( CClass, functions ) );
}

/*
==================
CClass::Serialize
==================
*/
void CClass::Serialize( class CArchive& InArchive )
{
	Super::Serialize( InArchive );
	InArchive << classFlags;
	InArchive << classCastFlags;
	InArchive << functions;
	InArchive << withinClass;
}

/*
==================
CClass::AddFunction
==================
*/
void CClass::AddFunction( class CFunction* InFunction )
{
	functions.push_back( InFunction );
}

/*
==================
CClass::Bind
==================
*/
void CClass::Bind()
{
	// Do nothing if the class already binded
	if ( IsBinded() )
	{
		return;
	}

	Super::Bind();
	CClass*		superClass = GetSuperClass();
	AssertMsg( superClass || CObject::StaticClass() == this, TEXT( "Unable to bind %s" ), GetName().c_str() );
	if ( !ClassConstructor && HasAnyClassFlags( CLASS_Native ) )
	{
		Sys_Errorf( TEXT( "Can't bind to native class %s" ), GetName().c_str() );
	}

	if ( !ClassConstructor && superClass )
	{
		// We look for constructor in parent class
		superClass->Bind();
		ClassConstructor = superClass->ClassConstructor;

		// Get casting flags from the parent
		classCastFlags |= superClass->classCastFlags;
	}

	// If class constructor still isn't valid then its error
	Assert( ClassConstructor );

	// Propagate inherited flags
	if ( superClass )
	{
		classFlags |= ( superClass->classFlags & CLASS_Inherit );
		classCastFlags |= superClass->classCastFlags;
	}

	// Assembly the token stream for realtime garbage collection
	// BS yehor.pohuliaka - Maybe need move to another place
	if ( !bHasAssembledReferenceTokenStream )
	{
		std::vector<CProperty*>		properties;
		GetProperties( properties, false );
		for ( uint32 index = 0, count = properties.size(); index < count; ++index )
		{
			CProperty*				property = properties[index];
			property->EmitReferenceInfo( &referenceTokenStream, 0 );
		}

		AssembleReferenceTokenStream();
		bHasAssembledReferenceTokenStream = true;
	}
}

/*
==================
CClass::GetFunctions
==================
*/
void CClass::GetFunctions( std::vector<class CFunction*>& OutArrayFunctions, bool InFunctionsInParents /* = true */ ) const
{
	for ( const CClass* tempClass = this; tempClass; tempClass = InFunctionsInParents ? tempClass->GetSuperClass() : nullptr )
	{
		if ( !tempClass->functions.empty() )
		{
			uint32		offset = OutArrayFunctions.size();
			OutArrayFunctions.resize( tempClass->functions.size() + offset );
			memcpy( OutArrayFunctions.data() + offset, tempClass->functions.data(), tempClass->functions.size() * sizeof( CFunction* ) );
		}
	}
}

/*
==================
CClass::GetNumFunctions
==================
*/
uint32 CClass::GetNumFunctions( bool InFunctionsInParents /* = true */ ) const
{
	uint32		numFunctions = 0;
	for ( const CClass* tempClass = this; tempClass; tempClass = InFunctionsInParents ? tempClass->GetSuperClass() : nullptr )
	{
		numFunctions += tempClass->functions.size();
	}

	return numFunctions;
}

/*
==================
CClass::FindFunction
==================
*/
CFunction* CClass::FindFunction( const CName& InName, bool InFindInParents /* = true */ ) const
{
	for ( const CClass* tempClass = this; tempClass; tempClass = InFindInParents ? tempClass->GetSuperClass() : nullptr )
	{
		for ( uint32 index = 0, count = tempClass->functions.size(); index < count; ++index )
		{
			if ( tempClass->functions[index]->GetCName() == InName )
			{
				return tempClass->functions[index];
			}
		}
	}
	return nullptr;
}

/*
==================
CClass::AssembleReferenceTokenStream
==================
*/
void CClass::AssembleReferenceTokenStream()
{
	if ( !bHasAssembledReferenceTokenStream )
	{
		if ( GetSuperClass() )
		{
			// Make sure super class has valid token stream
			GetSuperClass()->AssembleReferenceTokenStream();

			// Prepend super's stream. This automatically handles removing the EOS token
			referenceTokenStream.PrependStream( GetSuperClass()->referenceTokenStream );
		}

		// Emit end of stream token
		referenceTokenStream.EmitReferenceInfo( GCReferenceInfo::endOfStreamToken );

		// Shrink reference token stream to proper size
		referenceTokenStream.Shrink();
		bHasAssembledReferenceTokenStream = true;
	}
}