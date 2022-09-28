#include "Actors/PointLight.h"

IMPLEMENT_CLASS( APointLight )

APointLight::APointLight()
{
	pointLightComponent = CreateComponent<CPointLightComponent>( TEXT( "PointLightComponent0" ) );

#if WITH_EDITOR
	gizmoComponent		= CreateComponent<CSpriteComponent>( TEXT( "GizmoComponent0" ), true );
	gizmoComponent->SetGizmo( true );
	gizmoComponent->SetType( ST_Rotating );
	gizmoComponent->SetSpriteSize( Vector2D( 64.f, 64.f ) );
	gizmoComponent->SetMaterial( GPackageManager->FindAsset( TEXT( "Material'EditorMaterials:APointLight_Gizmo_Mat" ), AT_Material ) );
#endif // WITH_EDITOR
}

#if WITH_EDITOR
std::wstring APointLight::GetActorIcon() const
{
	return TEXT( "Engine/Editor/Icons/Actor_PointLight.png" );
}
#endif // WITH_EDITOR