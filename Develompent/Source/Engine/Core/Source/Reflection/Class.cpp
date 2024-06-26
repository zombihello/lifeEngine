#include "Reflection/Class.h"
#include "Reflection/Property.h"
#include "Reflection/Function.h"
#include "Reflection/FieldIterator.h"

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
	if ( !ClassConstructor && HasAnyClassFlags( CLASS_Native ) && !g_IsScriptCompiler )
	{
		Sys_Error( TEXT( "Can't bind to native class %s" ), GetName().c_str() );
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

	// Bind all functions and add it into map to fast find elements
	for ( TFieldIterator<CFunction> it( this, false ); it; ++it )
	{
		CFunction*		function = *it;
		function->Bind();
		functionsMap.insert( std::make_pair( function->GetName(), function ) );
	}

	// Assembly the token stream for realtime garbage collection
	if ( !bHasAssembledReferenceTokenStream )
	{
		for ( TFieldIterator<CProperty> it( this, false ); it; ++it )
		{
			it->EmitReferenceInfo( &referenceTokenStream, 0 );
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

/*
==================
CClass::AddFunction
==================
*/
void CClass::AddFunction( class CFunction* InFunction )
{
	InFunction->SetNextField( GetChildrenField() );
	SetChildrenField( InFunction );
	functionsMap.insert( std::make_pair( InFunction->GetName(), InFunction ) );
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
		auto	it = tempClass->functionsMap.find( InName );
		if ( it != tempClass->functionsMap.end() )
		{
			return it->second;
		}
	}
	return nullptr;
}