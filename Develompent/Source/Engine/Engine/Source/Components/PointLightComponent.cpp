#include "Components/PointLightComponent.h"

IMPLEMENT_CLASS( CPointLightComponent )

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
CPointLightComponent::GetLightType
==================
*/
ELightType CPointLightComponent::GetLightType() const
{
	return LT_Point;
}