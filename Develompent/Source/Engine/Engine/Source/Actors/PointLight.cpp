#include "Actors/PointLight.h"

IMPLEMENT_CLASS( APointLight )

/*
==================
APointLight::APointLight
==================
*/
APointLight::APointLight()
{
	pointLightComponent = CreateComponent<CPointLightComponent>( TEXT( "PointLightComponent0" ) );

#if WITH_EDITOR
	gizmoComponent		= CreateComponent<CSpriteComponent>( TEXT( "GizmoComponent0" ), true );
	gizmoComponent->SetGizmo( true );
	gizmoComponent->SetType( ST_Rotating );
	gizmoComponent->SetSpriteSize( Vector2D( 64.f, 64.f ) );
	gizmoComponent->SetMaterial( g_PackageManager->FindAsset( TEXT( "Material'EditorMaterials:APointLight_Gizmo_Mat" ), AT_Material ) );
#endif // WITH_EDITOR
}

/*
==================
APointLight::StaticInitializeClass
==================
*/
void APointLight::StaticInitializeClass()
{
	// Native properties
	new( staticClass, TEXT( "Point Light Component" ), OBJECT_Public ) CObjectProperty( CPP_PROPERTY( ThisClass, pointLightComponent ), TEXT( "Light" ), TEXT( "Point light component" ), CPF_Edit, CPointLightComponent::StaticClass() );
	
#if WITH_EDITOR
	new( staticClass, NAME_None ) CObjectProperty( CPP_PROPERTY( ThisClass, gizmoComponent ), NAME_None, TEXT( "" ), CPF_None, CSpriteComponent::StaticClass() );
#endif // WITH_EDITOR
}

#if WITH_EDITOR
/*
==================
APointLight::GetActorIcon
==================
*/
std::wstring APointLight::GetActorIcon() const
{
	return TEXT( "Engine/Editor/Icons/Actor_PointLight.png" );
}
#endif // WITH_EDITOR