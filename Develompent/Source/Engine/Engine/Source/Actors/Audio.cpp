#include "Misc/EngineGlobals.h"
#include "Actors/Audio.h"
#include "System/AudioBuffer.h"
#include "System/Package.h"
#include "System/World.h"

IMPLEMENT_CLASS( AAudio )

/*
==================
AAudio::AAudio
==================
*/
AAudio::AAudio()
{
	audioComponent = CreateComponent<CAudioComponent>( TEXT( "AudioComponent0" ) );

#if WITH_EDITOR
	gizmoComponent = CreateComponent<CSpriteComponent>( TEXT( "GizmoComponent0" ), true );
	gizmoComponent->SetGizmo( true );
	gizmoComponent->SetType( ST_Rotating );
	gizmoComponent->SetSpriteSize( Vector2D( 64.f, 64.f ) );
	gizmoComponent->SetMaterial( g_PackageManager->FindAsset( TEXT( "Material'EditorMaterials:AAudio_Gizmo_Mat" ), AT_Material ) );
#endif // WITH_EDITOR
}

/*
==================
AAudio::StaticInitializeClass
==================
*/
void AAudio::StaticInitializeClass()
{
	// Native properties
	new( staticClass, TEXT( "Audio Component" ), OBJECT_Public ) CObjectProperty( CPP_PROPERTY( ThisClass, audioComponent ), TEXT( "Audio" ), TEXT( "Audio component" ), CPF_Edit, CAudioComponent::StaticClass() );

#if WITH_EDITOR
	new( staticClass, NAME_None ) CObjectProperty( CPP_PROPERTY( ThisClass, gizmoComponent ), NAME_None, TEXT( "" ), CPF_EditorOnly, CSpriteComponent::StaticClass() );
#endif // WITH_EDITOR
}

#if WITH_EDITOR
/*
==================
AAudio::GetActorIcon
==================
*/
std::wstring AAudio::GetActorIcon() const
{
	return TEXT( "Engine/Editor/Icons/Asset_Audio.png" );
}

/*
==================
AAudio::SpawnActorAsset
==================
*/
AActor* AAudio::SpawnActorAsset( const TSharedPtr<CAsset>& InAsset, const Vector& InLocation, const Quaternion& InRotation )
{
	// If asset is not valid or not audio bank asset, we do nothing
	if ( !InAsset || InAsset->GetType() != AT_AudioBank )
	{
		return nullptr;
	}

	// Spawn new actor
	AAudio*		audioActor = g_World->SpawnActor<AAudio>( InLocation, InRotation );
	if ( !audioActor )
	{
		return nullptr;
	}

	// Init asset in new audio actor
	audioActor->SetAudioBank( InAsset->GetAssetHandle() );
	return audioActor;
}
#endif // WITH_EDITOR