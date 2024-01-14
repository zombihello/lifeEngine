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
	Sys_Errorf( TEXT( "CField::AddProperty: Not implemented" ) );
}

/*
==================
CField::Bind
==================
*/
void CField::Bind()
{
	// Do nothing if the field already binded
	if ( bBinded )
	{
		return;
	}

	// For native objects (only CClass, CStructs and CEnums) we have to create a package where this filed must be
	if ( HasAnyObjectFlags( OBJECT_Native ) && ( IsA<CStruct>( this ) || IsA<CEnum>( this ) ) )
	{
		CObjectPackage*		package = ( CObjectPackage* )CObjectPackage::CreatePackage( nullptr, ( const tchar* )GetOuter() );
		Assert( package );

		package->AddPackageFlag( PKG_InMemoryOnly );
		SetOuter( package );
	}

	bBinded = true;
}

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