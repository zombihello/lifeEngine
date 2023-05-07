#include "Actors/SpotLight.h"

IMPLEMENT_CLASS( ASpotLight )

/*
==================
ASpotLight::ASpotLight
==================
*/
ASpotLight::ASpotLight()
{
	spotLightComponent	= CreateComponent<CSpotLightComponent>( TEXT( "SpotLightComponent0" ) );

#if WITH_EDITOR
	gizmoComponent		= CreateComponent<CSpriteComponent>( TEXT( "GizmoComponent0" ), true );
	gizmoComponent->SetGizmo( true );
	gizmoComponent->SetType( ST_Rotating );
	gizmoComponent->SetSpriteSize( Vector2D( 64.f, 64.f ) );
	gizmoComponent->SetMaterial( g_PackageManager->FindAsset( TEXT( "Material'EditorMaterials:ASpotLight_Gizmo_Mat" ), AT_Material ) );

	arrowComponent		= CreateComponent<CArrowComponent>( TEXT( "ArrowComponent0" ), true );
#endif // WITH_EDITOR
}

#if WITH_EDITOR
/*
==================
ASpotLight::GetActorIcon
==================
*/
std::wstring ASpotLight::GetActorIcon() const
{
	return TEXT( "Engine/Editor/Icons/Actor_SpotLight.png" );
}
#endif // WITH_EDITOR