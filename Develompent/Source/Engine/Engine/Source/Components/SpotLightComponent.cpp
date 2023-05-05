#include "Components/SpotLightComponent.h"

IMPLEMENT_CLASS( CSpotLightComponent )

CSpotLightComponent::CSpotLightComponent()
	: bNeedUpdateCutoff( true )
	, radius( 850.f )
	, height( 1500.f )
	, cutoff( 0.f )
{
	intensivity = 100400.f;
}

ELightType CSpotLightComponent::GetLightType() const
{
	return LT_Spot;
}