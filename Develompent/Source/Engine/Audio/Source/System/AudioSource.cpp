#include "Misc/AudioGlobals.h"
#include "System/AudioDevice.h"
#include "System/AudioSource.h"

/*
==================
CAudioSource::CAudioSource
==================
*/
CAudioSource::CAudioSource()
	: bMuted( false )
	, alHandle( 0 )
	, volume( 100.f )
#if WITH_EDITOR
	, audioBankUpdatedHandle( nullptr )
#endif // WITH_EDITOR
	, audioDeviceMutedHandle( nullptr )
	, audioBufferDestroyedHandle( nullptr )
	, audioBufferUpdatedHandle( nullptr )
{
	alGenSources( 1, &alHandle );

	// Initialize properties of audio source
	SetLoop( false );
	SetRelativeToListener( false );
	SetVolume( 100.f );
	SetPitch( 1.f );
	SetMinDistance( 1.f );
	SetAttenuation( 1.f );
	SetLocation( Math::vectorZero );

	// Subscribe to event of muted/unmuted audio device
	audioDeviceMutedHandle = g_AudioDevice.OnAudioDeviceMuted().Add( std::bind( &CAudioSource::OnAudioDeviceMuted, this, std::placeholders::_1 ) );
}

/*
==================
CAudioSource::~CAudioSource
==================
*/
CAudioSource::~CAudioSource()
{
	alDeleteSources( 1, &alHandle );

	// Unsubscribe from event of muted/unmuted audio device
	g_AudioDevice.OnAudioDeviceMuted().Remove( audioDeviceMutedHandle );

	// Unsubscribe from event of audio buffer if him is exist
	TSharedPtr<CAudioBank>		audioBankRef = audioBank.ToSharedPtr();
	if ( audioBankRef )
	{
		AudioBufferRef_t		audioBuffer = audioBankRef->GetAudioBuffer();
		if ( audioBuffer )
		{
			audioBuffer->OnAudioBufferDestroyed().Remove( audioBufferDestroyedHandle );
			audioBuffer->OnAudioBufferUpdated().Remove( audioBufferUpdatedHandle );
		}

#if WITH_EDITOR
		audioBankRef->OnAudioBankUpdated().Remove( audioBankUpdatedHandle );
#endif // WITH_EDITOR
	}
}

/*
==================
CAudioSource::Play
==================
*/
void CAudioSource::Play()
{
	if ( !bMuted )
	{
		alSourcePlay( alHandle );
	}
}

/*
==================
CAudioSource::Pause
==================
*/
void CAudioSource::Pause()
{
	if ( !bMuted )
	{
		alSourcePause( alHandle );
	}
}

/*
==================
CAudioSource::Stop
==================
*/
void CAudioSource::Stop()
{
	if ( !bMuted )
	{
		alSourceStop( alHandle );
	}
}

/*
==================
CAudioSource::SetLoop
==================
*/
void CAudioSource::SetLoop( bool InIsLoop )
{
	alSourcei( alHandle, AL_LOOPING, InIsLoop );
}

/*
==================
CAudioSource::SetRelativeToListener
==================
*/
void CAudioSource::SetRelativeToListener( bool InIsRelativeToListener )
{
	alSourcei( alHandle, AL_SOURCE_RELATIVE, InIsRelativeToListener );
}

/*
==================
CAudioSource::SetVolume
==================
*/
void CAudioSource::SetVolume( float InVolume )
{
	alSourcef( alHandle, AL_GAIN, InVolume * g_AudioDevice.GetPlatformAudioHeadroom() );
	volume = InVolume;
}

/*
==================
CAudioSource::SetPitch
==================
*/
void CAudioSource::SetPitch( float InPitch )
{
	alSourcef( alHandle, AL_PITCH, InPitch );
}

/*
==================
CAudioSource::SetMinDistance
==================
*/
void CAudioSource::SetMinDistance( float InMinDistance )
{
	alSourcef( alHandle, AL_REFERENCE_DISTANCE, InMinDistance );
}

/*
==================
CAudioSource::SetAttenuation
==================
*/
void CAudioSource::SetAttenuation( float InAttenuation )
{
	alSourcef( alHandle, AL_ROLLOFF_FACTOR, InAttenuation );
}

/*
==================
CAudioSource::SetAudioBank
==================
*/
void CAudioSource::SetAudioBank( const TAssetHandle<CAudioBank>& InAudioBank )
{
	AudioBufferRef_t		audioBuffer;

	// Unsubscribe from events of audio buffer if him is exist
	{
		TSharedPtr<CAudioBank>		oldAudioBankRef = audioBank.ToSharedPtr();
		if ( oldAudioBankRef )
		{
			audioBuffer = oldAudioBankRef->GetAudioBuffer();
			if ( audioBuffer )
			{
				audioBuffer->OnAudioBufferDestroyed().Remove( audioBufferDestroyedHandle );
				audioBuffer->OnAudioBufferUpdated().Remove( audioBufferUpdatedHandle );
				audioBuffer = nullptr;
			}

#if WITH_EDITOR
			if ( audioBank != InAudioBank )
			{
				oldAudioBankRef->OnAudioBankUpdated().Remove( audioBankUpdatedHandle );
			}
#endif // WITH_EDITOR
		}
	}

	// Getting audio buffer if bank is valid and subscribe to events of audio buffer
	TSharedPtr<CAudioBank>		audioBankRef = InAudioBank.ToSharedPtr();
	if ( audioBankRef )
	{
		audioBuffer = audioBankRef->GetAudioBuffer();
		if ( audioBuffer )
		{
			audioBufferDestroyedHandle	= audioBuffer->OnAudioBufferDestroyed().Add( std::bind( &CAudioSource::OnAudioBufferDestroyed, this, std::placeholders::_1 ) );
			audioBufferUpdatedHandle	= audioBuffer->OnAudioBufferUpdated().Add( std::bind( &CAudioSource::OnAudioBufferUpdated, this, std::placeholders::_1 ) );
		}

#if WITH_EDITOR
		if ( audioBank != InAudioBank )
		{
			audioBankUpdatedHandle		= audioBankRef->OnAudioBankUpdated().Add( std::bind( &CAudioSource::OnAudioBankUpdated, this, std::placeholders::_1 ) );
		}
#endif // WITH_EDITOR
	}

	// Stop playing audio
	EAudioSourceStatus		status = GetStatus();
	if ( status != ASS_Stoped )
	{
		Stop();
	}

	audioBank		= InAudioBank;
	alSourcei( alHandle, AL_BUFFER, audioBuffer ? audioBuffer->GetALHandle() : 0 );
	
	// If the early the audio was playing we turn on back
	if ( audioBank.IsAssetValid() && status == ASS_Playing )
	{
		Play();
	}
}

/*
==================
CAudioSource::SetLocation
==================
*/
void CAudioSource::SetLocation( const Vector& InLocation )
{
	alSource3f( alHandle, AL_POSITION, InLocation.x, InLocation.y, InLocation.z );
}

/*
==================
CAudioSource::IsLooped
==================
*/
bool CAudioSource::IsLooped() const
{
	ALint			isLoop = 0;
	alGetSourcei( alHandle, AL_LOOPING, &isLoop );
	return isLoop != 0;
}

/*
==================
CAudioSource::IsRelativeToListener
==================
*/
bool CAudioSource::IsRelativeToListener() const
{
	ALint			isRelativeToListener = 0;
	alGetSourcei( alHandle, AL_SOURCE_RELATIVE, &isRelativeToListener );
	return isRelativeToListener != 0;
}

/*
==================
CAudioSource::GetVolume
==================
*/
float CAudioSource::GetVolume() const
{
	return volume;
}

/*
==================
CAudioSource::GetPitch
==================
*/
float CAudioSource::GetPitch() const
{
	ALfloat			pitch = 0.f;
	alGetSourcef( alHandle, AL_PITCH, &pitch );
	return pitch;
}

/*
==================
CAudioSource::GetMinDistance
==================
*/
float CAudioSource::GetMinDistance() const
{
	ALfloat			distance = 0.f;
	alGetSourcef( alHandle, AL_REFERENCE_DISTANCE, &distance );
	return distance;
}

/*
==================
CAudioSource::GetAttenuation
==================
*/
float CAudioSource::GetAttenuation() const
{
	ALfloat			attenuation = 0.f;
	alGetSourcef( alHandle, AL_ROLLOFF_FACTOR, &attenuation );
	return attenuation;
}

/*
==================
CAudioSource::GetStatus
==================
*/
EAudioSourceStatus CAudioSource::GetStatus() const
{
	ALint			alStatus = 0;
	alGetSourcei( alHandle, AL_SOURCE_STATE, &alStatus );

	switch ( alStatus )
	{
	case AL_PLAYING:	return ASS_Playing;
	case AL_PAUSED:		return ASS_Paused;
	case AL_STOPPED:
	default:			return ASS_Stoped;
	}
}

/*
==================
CAudioSource::GetAudioBank
==================
*/
TAssetHandle<CAudioBank> CAudioSource::GetAudioBank() const
{
	return audioBank;
}

/*
==================
CAudioSource::GetLocation
==================
*/
Vector CAudioSource::GetLocation() const
{
	Vector			location;
	alGetSource3f( alHandle, AL_POSITION, &location.x, &location.y, &location.z );
	return location;
}

/*
==================
CAudioSource::OnAudioDeviceMuted
==================
*/
void CAudioSource::OnAudioDeviceMuted( bool InIsAudioDeviceMuted )
{
	if ( InIsAudioDeviceMuted && GetStatus() == ASS_Playing )
	{
		Pause();
		bMuted = true;
	}
	else if ( !InIsAudioDeviceMuted && bMuted )
	{
		bMuted = false;
		Play();
	}
}

/*
==================
CAudioSource::OnAudioBufferDestroyed
==================
*/
void CAudioSource::OnAudioBufferDestroyed( class CAudioBuffer* InAudioBuffer )
{
	// Reset OpenAL buffer for audio source and stop playing
	alSourcei( alHandle, AL_BUFFER, 0 );
	if ( GetStatus() != ASS_Stoped )
	{
		Stop();
	}
}

/*
==================
CAudioSource::OnAudioBufferUpdated
==================
*/
void CAudioSource::OnAudioBufferUpdated( class CAudioBuffer* InAudioBuffer )
{
	uint32		alBufferHandle = InAudioBuffer->GetALHandle();

	// Stop playing audio
	EAudioSourceStatus		status = GetStatus();
	if ( status != ASS_Stoped )
	{
		Stop();
	}

	// When audio buffer is updated we recreating OpenAL buffer
	alSourcei( alHandle, AL_BUFFER, alBufferHandle );

	// If the early the audio was playing we turn on back
	if ( alBufferHandle && status == ASS_Playing )
	{
		Play();
	}
}

#if WITH_EDITOR
/*
==================
CAudioSource::OnAudioBankUpdated
==================
*/
void CAudioSource::OnAudioBankUpdated( class CAudioBank* InAudioBank )
{
	// Update data from audio bank
	Assert( InAudioBank );
	SetAudioBank( InAudioBank->GetAssetHandle() );
}
#endif // WITH_EDITOR