#include "Components/AudioComponent.h"

IMPLEMENT_CLASS( CAudioComponent )

/*
==================
CAudioComponent::CAudioComponent
==================
*/
CAudioComponent::CAudioComponent()
	: bIsLoop( false )
	, bIsUISound( false )
	, bIsAutoPlay( true )
	, bIsStreamable( false )
	, volume( 100.f )
	, pitch( 1.f )
	, minDistance( 1.f )
	, attenuation( 1.f )
	, source( nullptr )
	, oldSourceLocation( Math::vectorZero )
{
	UpdateAudioSourceType();
}

/*
==================
CAudioComponent::BeginDestroy
==================
*/
void CAudioComponent::BeginDestroy()
{
	Super::BeginDestroy();
	if ( source )
	{
		delete source;
	}
}

/*
==================
CAudioComponent::StaticInitializeClass
==================
*/
void CAudioComponent::StaticInitializeClass()
{
	// Native properties
	new( staticClass, TEXT( "bIsLoop" ), OBJECT_Public )		CBoolProperty( CPP_PROPERTY( ThisClass, bIsLoop ), TEXT( "Audio" ), TEXT( "Is looped" ), CPF_Edit );
	new( staticClass, TEXT( "bIsUISound" ), OBJECT_Public )		CBoolProperty( CPP_PROPERTY( ThisClass, bIsUISound ), TEXT( "Audio" ), TEXT( "Is UI sound" ), CPF_Edit );
	new( staticClass, TEXT( "bIsAutoPlay" ), OBJECT_Public )	CBoolProperty( CPP_PROPERTY( ThisClass, bIsAutoPlay ), TEXT( "Audio" ), TEXT( "Is need auto play on begin play" ), CPF_Edit );
	new( staticClass, TEXT( "bIsStreamable" ), OBJECT_Public )	CBoolProperty( CPP_PROPERTY( ThisClass, bIsStreamable ), TEXT( "Audio" ), TEXT( "Is streamable" ), CPF_Edit );
	new( staticClass, TEXT( "Volume" ), OBJECT_Public )			CFloatProperty( CPP_PROPERTY( ThisClass, volume ), TEXT( "Audio" ), TEXT( "Volume of sound" ), CPF_Edit );
	new( staticClass, TEXT( "Pitch" ), OBJECT_Public )			CFloatProperty( CPP_PROPERTY( ThisClass, pitch ), TEXT( "Audio" ), TEXT( "Pitch" ), CPF_Edit );
	new( staticClass, TEXT( "Min Distance" ), OBJECT_Public )	CFloatProperty( CPP_PROPERTY( ThisClass, minDistance ), TEXT( "Audio" ), TEXT( "Min distance" ), CPF_Edit );
	new( staticClass, TEXT( "Attenuation" ), OBJECT_Public )	CFloatProperty( CPP_PROPERTY( ThisClass, attenuation ), TEXT( "Audio" ), TEXT( "Attenuation" ), CPF_Edit );
	new( staticClass, TEXT( "Audio Bank" ), OBJECT_Public )		CAssetProperty( CPP_PROPERTY( ThisClass, bank ), TEXT( "Audio" ), TEXT( "Audio bank" ), CPF_Edit, AT_AudioBank );
}

#if WITH_EDITOR
/*
==================
CAudioComponent::PostEditChangeProperty
==================
*/
void CAudioComponent::PostEditChangeProperty( const PropertyChangedEvenet& InPropertyChangedEvenet )
{
	CProperty*		changedProperty = InPropertyChangedEvenet.property;
	if ( changedProperty )
	{
		static const CName		property_bIsLoop( TEXT( "bIsLoop" ) );
		static const CName		property_bIsUISound( TEXT( "bIsUISound" ) );
		static const CName		property_volume( TEXT( "Volume" ) );
		static const CName		property_pitch( TEXT( "Pitch" ) );
		static const CName		property_minDistance( TEXT( "Min Distance" ) );
		static const CName		property_attenuation( TEXT( "Attenuation" ) );
		static const CName		property_audioBank( TEXT( "Audio Bank" ) );
		static const CName		property_bIsStreamable( TEXT( "bIsStreamable" ) );

		if ( changedProperty->GetCName() == property_bIsLoop )
		{
			SetLoop( bIsLoop );
		}
		else if ( changedProperty->GetCName() == property_bIsUISound )
		{
			SetUISound( bIsUISound );
		}
		else if ( changedProperty->GetCName() == property_volume )
		{
			SetVolume( volume );
		}
		else if ( changedProperty->GetCName() == property_pitch )
		{
			SetPitch( pitch );
		}
		else if ( changedProperty->GetCName() == property_minDistance )
		{
			SetMinDistance( minDistance );
		}
		else if ( changedProperty->GetCName() == property_attenuation )
		{
			SetAttenuation( attenuation );
		}
		else if ( changedProperty->GetCName() == property_audioBank )
		{
			SetAudioBank( bank );
			if ( bank.IsAssetValid() && GetStatus() != ASS_Playing && bIsAutoPlay )
			{
				Play();
			}
		}
		else if ( changedProperty->GetCName() == property_bIsStreamable )
		{
			UpdateAudioSourceType();
		}
	}

	Super::PostEditChangeProperty( InPropertyChangedEvenet );
}
#endif // WITH_EDITOR

/*
==================
CAudioComponent::PostLoad
==================
*/
void CAudioComponent::PostLoad()
{
	Super::PostLoad();
	
	// Init first location of audio source and update it type
	if ( !bIsUISound )
	{
		oldSourceLocation = GetComponentLocation();
	}
	else
	{
		oldSourceLocation = Math::vectorZero;
	}
	UpdateAudioSourceType();

#if WITH_EDITOR
	if ( g_IsEditor && bIsAutoPlay )
	{
		Play();
	}
#endif // WITH_EDITOR
}

/*
==================
CAudioComponent::BeginPlay
==================
*/
void CAudioComponent::BeginPlay()
{
	Super::BeginPlay();

	// Play sound if flag 'bIsAutoPlay' is enabled
	if ( bIsAutoPlay )
	{
		Play();
	}
}

/*
==================
CAudioComponent::TickComponent
==================
*/
void CAudioComponent::TickComponent( float InDeltaTime )
{
	Super::TickComponent( InDeltaTime );

	// If audio component is moved - update audio source location if this not UI sound
	if ( !bIsUISound )
	{
		Vector		location = GetComponentLocation();
		if ( location != oldSourceLocation )
		{
			oldSourceLocation = location;
			source->SetLocation( location );
		}
	}
}

/*
==================
CAudioComponent::UpdateAudioSourceType
==================
*/
void CAudioComponent::UpdateAudioSourceType()
{
	// If source is valid - we getting current status and free allocated memory
	EAudioSourceStatus		status = ASS_Stoped;
	if ( source )
	{
		status = source->GetStatus();
		delete source;
	}

	// If bIsStreamable is true - this audio is stream source
	if ( bIsStreamable )
	{
		source = new CAudioStreamSource();
	}
	// Else this simple audio source
	else
	{
		source = new CAudioSource();
	}

	// Reset set all values to source
	source->SetLoop( bIsLoop );
	source->SetRelativeToListener( bIsUISound );
	source->SetPitch( pitch );
	source->SetVolume( volume );
	source->SetMinDistance( minDistance );
	source->SetAttenuation( attenuation );
	source->SetAudioBank( bank );
	source->SetLocation( oldSourceLocation );

	if ( status == ASS_Playing )
	{
		source->Play();
	}
}