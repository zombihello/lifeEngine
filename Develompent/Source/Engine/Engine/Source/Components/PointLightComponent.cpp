#include "Components/PointLightComponent.h"

IMPLEMENT_CLASS( CPointLightComponent )

// WorldEd reflection
BEGIN_DATADESC( CPointLightComponent, CLightComponent )
	DEFINE_FIELD( radius, "Light", "Light radius", FT_Float )
END_DATADESC()

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