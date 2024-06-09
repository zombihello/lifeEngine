#include "Reflection/Class.h"
#include "Reflection/Field.h"
#include "Reflection/Enum.h"
#include "Reflection/ObjectGlobals.h"
#include "Reflection/ObjectPackage.h"

IMPLEMENT_CLASS( CField )
IMPLEMENT_DEFAULT_INITIALIZE_CLASS( CField )

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