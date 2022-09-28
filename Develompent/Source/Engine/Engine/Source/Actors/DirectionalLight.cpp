#include "Actors/DirectionalLight.h"

IMPLEMENT_CLASS( ADirectionalLight )

ADirectionalLight::ADirectionalLight()
{
	directionalLightComponent	= CreateComponent<CDirectionalLightComponent>( TEXT( "DirectionalLightComponent0" ) );

#if WITH_EDITOR
	gizmoComponent				= CreateComponent<CSpriteComponent>( TEXT( "GizmoComponent0" ), true );
	gizmoComponent->SetGizmo( true );
	gizmoComponent->SetType( ST_Rotating );
	gizmoComponent->SetSpriteSize( Vector2D( 64.f, 64.f ) );
	gizmoComponent->SetMaterial( GPackageManager->FindAsset( TEXT( "Material'EditorMaterials:ADirectionalLight_Gizmo_Mat" ), AT_Material ) );
#endif // WITH_EDITOR
}

#if WITH_EDITOR
std::wstring ADirectionalLight::GetActorIcon() const
{
	return TEXT( "Engine/Editor/Icons/Actor_DirectionalLight.png" );
}
#endif // WITH_EDITOR