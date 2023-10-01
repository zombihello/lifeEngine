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
AAudio::~AAudio
==================
*/
AAudio::~AAudio()
{}

/*
==================
AAudio::StaticInitializeClass
==================
*/
void AAudio::StaticInitializeClass()
{
	new( staticClass, TEXT( "Audio Component" ) ) CObjectProperty( TEXT( "Audio" ), TEXT( "Audio component" ), CPP_PROPERTY( audioComponent ), 0, CAudioComponent::StaticClass() );
}

#if WITH_EDITOR
#include "Commandlets/CookPackagesCommandlet.h"

/*
==================
AAudio::InitProperties
==================
*/
bool AAudio::InitProperties( const std::vector<CActorVar>& InActorVars, class CCookPackagesCommandlet* InCooker )
{
	if ( !Super::InitProperties( InActorVars, InCooker ) )
	{
		return false;
	}

	for ( uint32 index = 0, count = InActorVars.size(); index < count; ++index )
	{
		const CActorVar&		actorVar = InActorVars[ index ];

		// If property is path to sound
		if ( actorVar.GetName() == TEXT( "Source" ) )
		{
			Assert( actorVar.GetType() == AVT_String );
			std::wstring					audioBankName = actorVar.GetValueString();
			TAssetHandle<CAudioBank>		audioBank = g_PackageManager->FindAsset( audioBankName, AT_Unknown );
			if ( !audioBank.IsAssetValid() && !InCooker->CookAudioBank( audioBankName, audioBank ) )
			{
				return false;
			}

			audioComponent->SetAudioBank( audioBank );
		}

		// If property is flag loop sound
		else if ( actorVar.GetName() == TEXT( "IsLoop" ) )
		{
			Assert( actorVar.GetType() == AVT_Bool );
			audioComponent->SetLoop( actorVar.GetValueBool() );
		}

		// If property is flag 'Is UI sound'
		else if ( actorVar.GetName() == TEXT( "IsUISound" ) )
		{
			Assert( actorVar.GetType() == AVT_Bool );
			audioComponent->SetUISound( actorVar.GetValueBool() );
		}

		// If property is flag auto play
		else if ( actorVar.GetName() == TEXT( "IsAutoPlay" ) )
		{
			Assert( actorVar.GetType() == AVT_Bool );
			audioComponent->SetAutoPlay( actorVar.GetValueBool() );
		}

		// If property is flag streamable
		else if ( actorVar.GetName() == TEXT( "IsStreamable" ) )
		{
			Assert( actorVar.GetType() == AVT_Bool );
			audioComponent->SetStreamable( actorVar.GetValueBool() );
		}

		// If property is volume
		else if ( actorVar.GetName() == TEXT( "Volume" ) )
		{
			Assert( actorVar.GetType() == AVT_Float || actorVar.GetType() == AVT_Int );
			audioComponent->SetVolume( actorVar.GetValueNumber() );
		}

		// If property is pitch
		else if ( actorVar.GetName() == TEXT( "Pitch" ) )
		{
			Assert( actorVar.GetType() == AVT_Float || actorVar.GetType() == AVT_Int );
			audioComponent->SetPitch( actorVar.GetValueNumber() );
		}

		// If property is min distance
		else if ( actorVar.GetName() == TEXT( "MinDistance" ) )
		{
			Assert( actorVar.GetType() == AVT_Float || actorVar.GetType() == AVT_Int );
			audioComponent->SetMinDistance( actorVar.GetValueNumber() );
		}

		// If property is attenuation
		else if ( actorVar.GetName() == TEXT( "Attenuation" ) )
		{
			Assert( actorVar.GetType() == AVT_Float || actorVar.GetType() == AVT_Int );
			audioComponent->SetAttenuation( actorVar.GetValueNumber() );
		}
	}

	return true;
}

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
ActorRef_t AAudio::SpawnActorAsset( const TSharedPtr<CAsset>& InAsset, const Vector& InLocation, const Quaternion& InRotation )
{
	// If asset is not valid or not audio bank asset, we do nothing
	if ( !InAsset || InAsset->GetType() != AT_AudioBank )
	{
		return nullptr;
	}

	// Spawn new actor
	TRefCountPtr<AAudio>		audioActor = g_World->SpawnActor<AAudio>( InLocation, InRotation );
	if ( !audioActor )
	{
		return nullptr;
	}

	// Init asset in new audio actor
	audioActor->SetAudioBank( InAsset->GetAssetHandle() );
	return audioActor;
}
#endif // WITH_EDITOR