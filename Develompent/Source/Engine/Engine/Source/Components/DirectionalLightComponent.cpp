#include "Components/DirectionalLightComponent.h"

IMPLEMENT_CLASS( CDirectionalLightComponent )

CDirectionalLightComponent::CDirectionalLightComponent()
{}

ELightType CDirectionalLightComponent::GetLightType() const
{
	return LT_Directional;
}