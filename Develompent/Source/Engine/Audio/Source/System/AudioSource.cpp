#include "Misc/AudioGlobals.h"
#include "System/AudioDevice.h"
#include "System/AudioSource.h"

FAudioSource::FAudioSource()
	: bMuted( false )
	, alHandle( 0 )
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

	// Subscribe to event of muted/unmuted audio device
	GAudioDevice.OnAudioDeviceMuted().Add( this, &FAudioSource::OnAudioDeviceMuted );
}

FAudioSource::~FAudioSource()
{
	alDeleteSources( 1, &alHandle );

	// Unsubscribe from event of muted/unmuted audio device
	GAudioDevice.OnAudioDeviceMuted().Remove( this, &FAudioSource::OnAudioDeviceMuted );

	// Unsubscribe from event of audio buffer if him is exist
	TSharedPtr<FAudioBank>		audioBankRef = audioBank.Pin();
	if ( audioBankRef )
	{
		FAudioBufferRef		audioBuffer = audioBankRef->GetAudioBuffer();
		if ( audioBuffer )
		{
			audioBuffer->OnAudioBufferDestroyed().Remove( this, &FAudioSource::OnAudioBufferDestroyed );
			audioBuffer->OnAudioBufferUpdated().Remove( this, &FAudioSource::OnAudioBufferUpdated );
		}
	}
}

void FAudioSource::Play()
{
	alSourcePlay( alHandle );
}

void FAudioSource::Pause()
{
	alSourcePause( alHandle );
}

void FAudioSource::Stop()
{
	alSourceStop( alHandle );
}

void FAudioSource::SetLoop( bool InIsLoop )
{
	alSourcei( alHandle, AL_LOOPING, InIsLoop );
}

void FAudioSource::SetRelativeToListener( bool InIsRelativeToListener )
{
	alSourcei( alHandle, AL_SOURCE_RELATIVE, InIsRelativeToListener );
}

void FAudioSource::SetVolume( float InVolume )
{
	alSourcef( alHandle, AL_GAIN, InVolume * GAudioDevice.GetPlatformAudioHeadroom() );
	volume = InVolume;
}

void FAudioSource::SetPitch( float InPitch )
{
	alSourcef( alHandle, AL_PITCH, InPitch );
}

void FAudioSource::SetMinDistance( float InMinDistance )
{
	alSourcef( alHandle, AL_REFERENCE_DISTANCE, InMinDistance );
}

void FAudioSource::SetAttenuation( float InAttenuation )
{
	alSourcef( alHandle, AL_ROLLOFF_FACTOR, InAttenuation );
}

void FAudioSource::SetAudioBank( const FAudioBankPtr& InAudioBank )
{
	FAudioBufferRef		audioBuffer;

	// Unsubscribe from events of audio buffer if him is exist
	{
		TSharedPtr<FAudioBank>		oldAudioBankRef = audioBank.Pin();
		if ( oldAudioBankRef )
		{
			audioBuffer = oldAudioBankRef->GetAudioBuffer();
			if ( audioBuffer )
			{
				audioBuffer->OnAudioBufferDestroyed().Remove( this, &FAudioSource::OnAudioBufferDestroyed );
				audioBuffer->OnAudioBufferUpdated().Remove( this, &FAudioSource::OnAudioBufferUpdated );
				audioBuffer = nullptr;
			}
		}
	}

	audioBank = InAudioBank;

	// Getting audio buffer if bank is valid and subscribe to events of audio buffer
	TSharedPtr<FAudioBank>		audioBankRef = InAudioBank.Pin();
	if ( audioBankRef )
	{
		audioBuffer = audioBankRef->GetAudioBuffer();
		if ( audioBuffer )
		{
			audioBuffer->OnAudioBufferDestroyed().Add( this, &FAudioSource::OnAudioBufferDestroyed );
			audioBuffer->OnAudioBufferUpdated().Add( this, &FAudioSource::OnAudioBufferUpdated );
		}
	}

	alSourcei( alHandle, AL_BUFFER, audioBuffer ? audioBuffer->GetALHandle() : 0 );
}

void FAudioSource::SetLocation( const FVector& InLocation )
{
	alSource3f( alHandle, AL_POSITION, InLocation.x, InLocation.y, InLocation.z );
}

bool FAudioSource::IsLooped() const
{
	ALint			isLoop = 0;
	alGetSourcei( alHandle, AL_LOOPING, &isLoop );
	return isLoop != 0;
}

bool FAudioSource::IsRelativeToListener() const
{
	ALint			isRelativeToListener = 0;
	alGetSourcei( alHandle, AL_SOURCE_RELATIVE, &isRelativeToListener );
	return isRelativeToListener != 0;
}

float FAudioSource::GetVolume() const
{
	return volume;
}

float FAudioSource::GetPitch() const
{
	ALfloat			pitch = 0.f;
	alGetSourcef( alHandle, AL_PITCH, &pitch );
	return pitch;
}

float FAudioSource::GetMinDistance() const
{
	ALfloat			distance = 0.f;
	alGetSourcef( alHandle, AL_REFERENCE_DISTANCE, &distance );
	return distance;
}

float FAudioSource::GetAttenuation() const
{
	ALfloat			attenuation = 0.f;
	alGetSourcef( alHandle, AL_ROLLOFF_FACTOR, &attenuation );
	return attenuation;
}

EAudioSourceStatus FAudioSource::GetStatus() const
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

FAudioBankPtr FAudioSource::GetAudioBank() const
{
	return audioBank;
}

FVector FAudioSource::GetLocation() const
{
	FVector			location;
	alGetSource3f( alHandle, AL_POSITION, &location.x, &location.y, &location.z );
	return location;
}

void FAudioSource::OnAudioDeviceMuted( bool InIsAudioDeviceMuted )
{
	if ( InIsAudioDeviceMuted && GetStatus() == ASS_Playing )
	{
		Pause();
		bMuted = true;
	}
	else if ( !InIsAudioDeviceMuted && bMuted )
	{
		Play();
		bMuted = false;
	}
}

void FAudioSource::OnAudioBufferDestroyed( class FAudioBuffer* InAudioBuffer )
{
	// Reset OpenAL buffer for audio source
	alSourcei( alHandle, AL_BUFFER, 0 );
}

void FAudioSource::OnAudioBufferUpdated( class FAudioBuffer* InAudioBuffer )
{
	// When audio buffer is updated we recreating OpenAL buffer
	alSourcei( alHandle, AL_BUFFER, InAudioBuffer->GetALHandle() );
}