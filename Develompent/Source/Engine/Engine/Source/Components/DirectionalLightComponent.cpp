#include "Components/DirectionalLightComponent.h"

IMPLEMENT_CLASS( CDirectionalLightComponent )

CDirectionalLightComponent::CDirectionalLightComponent()
{
	intensivity = 3.f;
}

ELightType CDirectionalLightComponent::GetLightType() const
{
	return LT_Directional;
}