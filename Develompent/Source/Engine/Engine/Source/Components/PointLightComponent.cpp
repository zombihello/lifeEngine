#include "Misc/EngineGlobals.h"
#include "Render/Scene.h"
#include "Render/SceneUtils.h"
#include "System/World.h"
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
CPointLightComponent::StaticInitializeClass
==================
*/
void CPointLightComponent::StaticInitializeClass()
{
	new( staticClass, TEXT( "Radius" ) ) CFloatProperty( CPP_PROPERTY( ThisClass, radius ), TEXT( "Light" ), TEXT( "Light radius" ), CPF_Edit );
}

/*
==================
CPointLightComponent::Serialize
==================
*/
void CPointLightComponent::Serialize( class CArchive& InArchive )
{
	Super::Serialize( InArchive );
	if ( InArchive.Ver() < VER_NewSeriallizeDataInLightComponents )
	{
		return;
	}

	InArchive << radius;
}

#if WITH_EDITOR
/*
==================
CPointLightComponent::DrawDebugComponent
==================
*/
void CPointLightComponent::DrawDebugComponent()
{
	if ( GetOwner() && GetOwner()->IsSelected() )
	{
		DrawWireSphere( ( ( CScene* )g_World->GetScene() )->GetSDG( SDG_WorldEdBackground ), GetComponentLocation(), DEC_LIGHT, radius, 50 );
	}
}
#endif // WITH_EDITOR

/*
==================
CPointLightComponent::GetLightType
==================
*/
ELightType CPointLightComponent::GetLightType() const
{
	return LT_Point;
}