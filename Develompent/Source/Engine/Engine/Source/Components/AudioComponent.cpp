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
	, oldSourceLocation( SMath::vectorZero )
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
	new CBoolProperty( staticClass, TEXT( "bIsLoop" ), TEXT( "Audio" ), TEXT( "Is looped" ), CPP_PROPERTY( bIsLoop ), 0 );
	new CBoolProperty( staticClass, TEXT( "bIsUISound" ), TEXT( "Audio" ), TEXT( "Is UI sound" ), CPP_PROPERTY( bIsUISound ), 0 );
	new CBoolProperty( staticClass, TEXT( "bIsAutoPlay" ), TEXT( "Audio" ), TEXT( "Is need auto play on begin play" ), CPP_PROPERTY( bIsAutoPlay ), 0 );
	new CBoolProperty( staticClass, TEXT( "bIsStreamable" ), TEXT( "Audio" ), TEXT( "Is streamable" ), CPP_PROPERTY( bIsStreamable ), 0 );
	new CFloatProperty( staticClass, TEXT( "volume" ), TEXT( "Audio" ), TEXT( "Volume of sound" ), CPP_PROPERTY( volume ), 0 );
	new CFloatProperty( staticClass, TEXT( "pitch" ), TEXT( "Audio" ), TEXT( "Pitch" ), CPP_PROPERTY( pitch ), 0 );
	new CFloatProperty( staticClass, TEXT( "minDistance" ), TEXT( "Audio" ), TEXT( "Min distance" ), CPP_PROPERTY( minDistance ), 0 );
	new CFloatProperty( staticClass, TEXT( "attenuation" ), TEXT( "Audio" ), TEXT( "Attenuation" ), CPP_PROPERTY( attenuation ), 0 );
	// TODO BS yehor.pohuliaka - Need implement asset property for 'bank'
}

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
		UpdateAudioSourceType();
		if ( source && bIsAutoPlay )
		{
			source->Play();
		}
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

	// Init first location of audio source
	if ( !bIsUISound )
	{
		oldSourceLocation = GetComponentLocation();
	}
	else
	{
		oldSourceLocation = SMath::vectorZero;
	}
	source->SetLocation( oldSourceLocation );
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