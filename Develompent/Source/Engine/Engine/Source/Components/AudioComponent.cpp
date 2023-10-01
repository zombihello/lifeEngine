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
CAudioComponent::~CAudioComponent
==================
*/
CAudioComponent::~CAudioComponent()
{
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
	new( staticClass, TEXT( "bIsLoop" ) )		CBoolProperty( TEXT( "Audio" ), TEXT( "Is looped" ), CPP_PROPERTY( bIsLoop ), 0 );
	new( staticClass, TEXT( "bIsUISound" ) )	CBoolProperty( TEXT( "Audio" ), TEXT( "Is UI sound" ), CPP_PROPERTY( bIsUISound ), 0 );
	new( staticClass, TEXT( "bIsAutoPlay" ) )	CBoolProperty( TEXT( "Audio" ), TEXT( "Is need auto play on begin play" ), CPP_PROPERTY( bIsAutoPlay ), 0 );
	new( staticClass, TEXT( "bIsStreamable" ) ) CBoolProperty( TEXT( "Audio" ), TEXT( "Is streamable" ), CPP_PROPERTY( bIsStreamable ), 0 );
	new( staticClass, TEXT( "Volume" ) )		CFloatProperty( TEXT( "Audio" ), TEXT( "Volume of sound" ), CPP_PROPERTY( volume ), 0 );
	new( staticClass, TEXT( "Pitch" ) )			CFloatProperty( TEXT( "Audio" ), TEXT( "Pitch" ), CPP_PROPERTY( pitch ), 0 );
	new( staticClass, TEXT( "Min Distance" ) )	CFloatProperty( TEXT( "Audio" ), TEXT( "Min distance" ), CPP_PROPERTY( minDistance ), 0 );
	new( staticClass, TEXT( "Attenuation" ) )	CFloatProperty( TEXT( "Audio" ), TEXT( "Attenuation" ), CPP_PROPERTY( attenuation ), 0 );
	new( staticClass, TEXT( "Audio Bank" ) )	CAssetProperty( TEXT( "Audio" ), TEXT( "Audio bank" ), CPP_PROPERTY( bank ), 0, AT_AudioBank );
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
CAudioComponent::Serialize
==================
*/
void CAudioComponent::Serialize( class CArchive& InArchive )
{
	Super::Serialize( InArchive );	
	InArchive << bIsLoop;
	InArchive << bIsUISound;
	InArchive << bIsAutoPlay;
	InArchive << bIsStreamable;
	InArchive << volume;
	InArchive << minDistance;
	InArchive << attenuation;
	InArchive << pitch;
	InArchive << bank;

	// If we archive loading - init audio source
	if ( InArchive.IsLoading() )
	{
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