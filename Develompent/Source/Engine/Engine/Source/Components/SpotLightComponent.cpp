#include "Components/SpotLightComponent.h"

IMPLEMENT_CLASS( CSpotLightComponent )

CSpotLightComponent::CSpotLightComponent()
	: radius( 850.f )
	, height( 300.f )
	, cutoff( 150.f )
{}

ELightType CSpotLightComponent::GetLightType() const
{
	return LT_Spot;
}