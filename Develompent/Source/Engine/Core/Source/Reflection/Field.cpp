#include "Reflection/ReflectionEnvironment.h"

IMPLEMENT_CLASS( CField )
IMPLEMENT_DEFAULT_INITIALIZE_CLASS( CField )

/*
==================
CField::AddProperty
==================
*/
void CField::AddProperty( class CProperty* InProperty )
{
	Sys_Errorf( TEXT( "CField::AddProperty: Not implemented" ) );
}

/*
==================
CField::AddFunction
==================
*/
void CField::AddFunction( class CFunction* InFunction )
{
	Sys_Errorf( TEXT( "CField::AddFunction: Not implemented" ) );
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