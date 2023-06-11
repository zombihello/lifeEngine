#include "Components/SpotLightComponent.h"

IMPLEMENT_CLASS( CSpotLightComponent )

// WorldEd reflection
BEGIN_DATADESC( CSpotLightComponent, CLightComponent )
	DEFINE_FIELD( radius, "Light", "Light radius", FT_Float )
	DEFINE_FIELD( height, "Light", "Light height", FT_Float )
END_DATADESC()

/*
==================
CSpotLightComponent::CSpotLightComponent
==================
*/
CSpotLightComponent::CSpotLightComponent()
	: bNeedUpdateCutoff( true )
	, radius( 850.f )
	, height( 1500.f )
	, cutoff( 0.f )
{
	intensivity = 100400.f;
}

/*
==================
CSpotLightComponent::Serialize
==================
*/
void CSpotLightComponent::Serialize( class CArchive& InArchive )
{
	Super::Serialize( InArchive );
	if ( InArchive.Ver() < VER_NewSeriallizeDataInLightComponents )
	{
		return;
	}

	InArchive << radius;
	InArchive << height;
	
	if ( InArchive.IsLoading() )
	{
		bNeedUpdateCutoff = true;
	}
}

/*
==================
CSpotLightComponent::GetLightType
==================
*/
ELightType CSpotLightComponent::GetLightType() const
{
	return LT_Spot;
}