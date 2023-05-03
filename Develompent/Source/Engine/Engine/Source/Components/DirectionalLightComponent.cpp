#include "Components/DirectionalLightComponent.h"

IMPLEMENT_CLASS( CDirectionalLightComponent )

CDirectionalLightComponent::CDirectionalLightComponent()
{
	intensivity = 30.f;
	SetRelativeRotation( SMath::AnglesToQuaternionXYZ( Vector( 90.f, 0.f, 0.f ) ) );
}

ELightType CDirectionalLightComponent::GetLightType() const
{
	return LT_Directional;
}