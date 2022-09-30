#include "Components/SpotLightComponent.h"

IMPLEMENT_CLASS( CSpotLightComponent )

CSpotLightComponent::CSpotLightComponent()
{}

ELightType CSpotLightComponent::GetLightType() const
{
	return LT_Spot;
}