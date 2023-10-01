#include "Actors/DirectionalLight.h"

IMPLEMENT_CLASS( ADirectionalLight )

/*
==================
ADirectionalLight::ADirectionalLight
==================
*/
ADirectionalLight::ADirectionalLight()
{
	directionalLightComponent	= CreateComponent<CDirectionalLightComponent>( TEXT( "DirectionalLightComponent0" ) );

#if WITH_EDITOR
	gizmoComponent				= CreateComponent<CSpriteComponent>( TEXT( "GizmoComponent0" ), true );
	gizmoComponent->SetGizmo( true );
	gizmoComponent->SetType( ST_Rotating );
	gizmoComponent->SetSpriteSize( Vector2D( 64.f, 64.f ) );
	gizmoComponent->SetMaterial( g_PackageManager->FindAsset( TEXT( "Material'EditorMaterials:ADirectionalLight_Gizmo_Mat" ), AT_Material ) );

	arrowComponent				= CreateComponent<CArrowComponent>( TEXT( "ArrowComponent0" ), true );
#endif // WITH_EDITOR
}

/*
==================
ADirectionalLight::StaticInitializeClass
==================
*/
void ADirectionalLight::StaticInitializeClass()
{
	new( staticClass, TEXT( "Directional Light Component" ) ) CObjectProperty( TEXT( "Light" ), TEXT( "Directional light component" ), CPP_PROPERTY( directionalLightComponent ), 0, CDirectionalLightComponent::StaticClass() );
}

#if WITH_EDITOR
/*
==================
ADirectionalLight::GetActorIcon
==================
*/
std::wstring ADirectionalLight::GetActorIcon() const
{
	return TEXT( "Engine/Editor/Icons/Actor_DirectionalLight.png" );
}
#endif // WITH_EDITOR