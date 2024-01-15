#include "Reflection/Class.h"
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
	InArchive << withinClass;
}

/*
==================
CClass::Register
==================
*/
void CClass::Register()
{
	Super::Register();
	Bind();
}

/*
==================
CClass::Bind
==================
*/
void CClass::Bind()
{
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
	}

	// If class constructor still isn't valid then its error
	Assert( ClassConstructor );

	// Propagate inherited flags
	if ( superClass )
	{
		classFlags |= ( superClass->classFlags & CLASS_MASK_Inherit );
		classCastFlags |= superClass->classCastFlags;
	}

	// Assembly the token stream for realtime garbage collection
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