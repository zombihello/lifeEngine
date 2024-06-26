#include "Reflection/Class.h"
#include "Reflection/Field.h"
#include "Reflection/Enum.h"
#include "Reflection/ObjectGlobals.h"
#include "Reflection/ObjectPackage.h"

IMPLEMENT_CLASS( CField )

/*
==================
CField::StaticInitializeClass
==================
*/
void CField::StaticInitializeClass()
{
	CClass*		theClass = StaticClass();
	theClass->EmitObjectReference( STRUCT_OFFSET( CField, nextField ) );
}

/*
==================
CField::Serialize
==================
*/
void CField::Serialize( class CArchive& InArchive )
{
	Super::Serialize( InArchive );
	InArchive << nextField;
}

/*
==================
CField::AddProperty
==================
*/
void CField::AddProperty( class CProperty* InProperty )
{
	Sys_Error( TEXT( "CField::AddProperty: Not implemented" ) );
}

/*
==================
CField::AddFunction
==================
*/
void CField::AddFunction( class CFunction* InFunction )
{
	Sys_Error( TEXT( "CField::AddFunction: Not implemented" ) );
}

/*
==================
CField::Bind
==================
*/
void CField::Bind()
{}

/*
==================
CField::GetOwnerClass
==================
*/
CClass* CField::GetOwnerClass() const
{
	const CObject*	result = this;
	while ( result->GetClass() != CClass::StaticClass() )
	{
		result = result->GetOuter();
	}

	return ( CClass* )result;
}

/*
==================
CField::PostLoad
==================
*/
void CField::PostLoad()
{
	Super::PostLoad();
	Bind();
}