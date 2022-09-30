#include "Components/PointLightComponent.h"

IMPLEMENT_CLASS( CPointLightComponent )

CPointLightComponent::CPointLightComponent()
	: radius( 200.f )
{}

ELightType CPointLightComponent::GetLightType() const
{
	return LT_Point;
}