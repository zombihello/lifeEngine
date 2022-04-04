#include "Components/AudioComponent.h"

IMPLEMENT_CLASS( LAudioComponent )


LAudioComponent::LAudioComponent()
	: bIsLoop( false )
	, bIsUISound( false )
	, bIsAutoPlay( false )
	, bIsStreamable( false )
	, volume( 100.f )
	, pitch( 1.f )
	, minDistance( 1.f )
	, attenuation( 1.f )
	, source( nullptr )
	, oldSourceLocation( FMath::vectorZero )
{
	UpdateAudioSourceType();
}

LAudioComponent::~LAudioComponent()
{
	if ( source )
	{
		delete source;
	}
}

void LAudioComponent::Serialize( class FArchive& InArchive )
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

	// Init first location of audio source
	if ( !bIsUISound )
	{
		oldSourceLocation = GetComponentLocation();
	}
	else
	{
		oldSourceLocation = FMath::vectorZero;
	}
	source->SetLocation( oldSourceLocation );
}
#include "Misc/EngineGlobals.h"
#include "System/InputSystem.h"
void LAudioComponent::TickComponent( float InDeltaTime )
{
	Super::TickComponent( InDeltaTime );

	if ( GInputSystem->IsKeyUp( BC_KeyF5 ) )
	{
		source->Play();
	}
	
	if ( GInputSystem->IsKeyUp( BC_KeyF6 ) )
	{
		source->Pause();
	}

	if ( GInputSystem->IsKeyUp( BC_KeyF7 ) )
	{
		source->Stop();
	}

	// If audio component is moved - update audio source location if this not UI sound
	if ( !bIsUISound )
	{
		FVector		location = GetComponentLocation();
		if ( location != oldSourceLocation )
		{
			oldSourceLocation = location;
			source->SetLocation( location );
		}
	}
}

void LAudioComponent::UpdateAudioSourceType()
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
		source = new FAudioStreamSource();
	}
	// Else this simple audio source
	else
	{
		source = new FAudioSource();
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
}