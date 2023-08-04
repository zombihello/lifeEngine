#include "Components/PointLightComponent.h"

IMPLEMENT_CLASS( CPointLightComponent )

/*
==================
CPointLightComponent::CPointLightComponent
==================
*/
CPointLightComponent::CPointLightComponent()
	: radius( 850.f )
{}

/*
==================
CPointLightComponent::StaticInitializeClass
==================
*/
void CPointLightComponent::StaticInitializeClass()
{
	new CFloatProperty( staticClass, TEXT( "radius" ), TEXT( "Light" ), TEXT( "Light radius" ), CPP_PROPERTY( radius ), 0 );
}

/*
==================
CPointLightComponent::Serialize
==================
*/
void CPointLightComponent::Serialize( class CArchive& InArchive )
{
	Super::Serialize( InArchive );
	if ( InArchive.Ver() < VER_NewSeriallizeDataInLightComponents )
	{
		return;
	}

	InArchive << radius;
}

/*
==================
CPointLightComponent::GetLightType
==================
*/
ELightType CPointLightComponent::GetLightType() const
{
	return LT_Point;
}