#include "Components/AudioComponent.h"

IMPLEMENT_CLASS( LAudioComponent )


LAudioComponent::LAudioComponent()
	: bIsLoop( false )
	, bIsUISound( false )
	, bIsAutoPlay( false )
	, volume( 100.f )
	, pitch( 1.f )
	, minDistance( 1.f )
	, attenuation( 1.f )
	, oldSourceLocation( FMath::vectorZero )
{}

void LAudioComponent::Serialize( class FArchive& InArchive )
{
	Super::Serialize( InArchive );	
	InArchive << bIsLoop;
	InArchive << bIsUISound;
	InArchive << bIsAutoPlay;
	InArchive << volume;
	InArchive << minDistance;
	InArchive << attenuation;
	InArchive << pitch;
	InArchive << buffer;

	// If we archive loading - init audio source
	if ( InArchive.IsLoading() )
	{
		source.SetLoop( bIsLoop );
		source.SetRelativeToListener( bIsUISound );
		source.SetPitch( pitch );
		source.SetVolume( volume );
		source.SetMinDistance( minDistance );
		source.SetAttenuation( attenuation );
		source.SetAudioBuffer( buffer );

		// If sound is UI - set location on {0,0,0}
		if ( bIsUISound )
		{
			oldSourceLocation = FMath::vectorZero;
			source.SetLocation( FMath::vectorZero );
		}
	}
}

void LAudioComponent::BeginPlay()
{
	Super::BeginPlay();

	// Play sound if flag 'bIsAutoPlay' is enabled
	if ( bIsAutoPlay )
	{
		Play();
	}

	// Init first location of audio source, if is not UI sound
	if ( !bIsUISound )
	{
		oldSourceLocation = GetComponentLocation();
		source.SetLocation( oldSourceLocation );
	}
}

void LAudioComponent::TickComponent( float InDeltaTime )
{
	Super::TickComponent( InDeltaTime );

	// If audio component is moved - update audio source location if this not UI sound
	if ( !bIsUISound )
	{
		FVector		location = GetComponentLocation();
		if ( location != oldSourceLocation )
		{
			oldSourceLocation = location;
			source.SetLocation( location );
		}
	}
}