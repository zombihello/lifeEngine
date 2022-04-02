#include "System/AudioSource.h"

FAudioSource::FAudioSource()
	: alHandle( 0 )
	, volume( 100.f )
{
	alGenSources( 1, &alHandle );

	// Initialize properties of audio source
	SetLoop( false );
	SetRelativeToListener( false );
	SetVolume( 100.f );
	SetPitch( 1.f );
	SetMinDistance( 1.f );
	SetAttenuation( 1.f );
	SetLocation( FMath::vectorZero );
}

FAudioSource::~FAudioSource()
{
	alDeleteSources( 1, &alHandle );
}