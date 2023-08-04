#include "Components/DirectionalLightComponent.h"

IMPLEMENT_CLASS( CDirectionalLightComponent )
IMPLEMENT_DEFAULT_INITIALIZE_CLASS( CDirectionalLightComponent )

/*
==================
CDirectionalLightComponent::CDirectionalLightComponent
==================
*/
CDirectionalLightComponent::CDirectionalLightComponent()
{
	intensivity = 30.f;
	SetRelativeRotation( SMath::AnglesToQuaternionXYZ( Vector( 90.f, 0.f, 0.f ) ) );
}

/*
==================
CDirectionalLightComponent::GetLightType
==================
*/
ELightType CDirectionalLightComponent::GetLightType() const
{
	return LT_Directional;
}