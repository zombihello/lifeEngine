#include <AL/al.h>
#include <AL/alc.h>

#include "Misc/CoreGlobals.h"
#include "System/AudioDevice.h"
#include "Logger/LoggerMacros.h"
#include "System/Config.h"
#include "Containers/StringConv.h"
#include "Core.h"

/*
==================
Sys_SampleFormatToEngine
==================
*/
uint32 Sys_SampleFormatToEngine( ESampleFormat InSampleFormat )
{
	switch ( InSampleFormat )
	{
	case SF_Mono8:		return AL_FORMAT_MONO8;
	case SF_Mono16:		return AL_FORMAT_MONO16;
	case SF_Stereo8:	return AL_FORMAT_STEREO8;
	case SF_Stereo16:	return AL_FORMAT_STEREO16;
	case SF_Unknown:
	default:
		Sys_Errorf( TEXT( "Unknown sample format 0x%X" ), InSampleFormat );
		return 0;
	}
}

/*
==================
Sys_SampleFormatToText
==================
*/
std::wstring Sys_SampleFormatToText( ESampleFormat InSampleFormat )
{
	switch ( InSampleFormat )
	{
	case SF_Mono8:		return TEXT( "8 bit (Mono)" );
	case SF_Mono16:		return TEXT( "16 bit (Mono)" );
	case SF_Stereo8:	return TEXT( "8 bit (Stereo)" );
	case SF_Stereo16:	return TEXT( "16 bit (Stereo)" );
	case SF_Unknown:
	default:
		Sys_Errorf( TEXT( "Unknown sample format 0x%X" ), InSampleFormat );
		return TEXT( "Unknown");
	}
}

/*
==================
Sys_GetNumSampleBytes
==================
*/
uint32 Sys_GetNumSampleBytes( ESampleFormat InSampleFormat )
{
	switch ( InSampleFormat )
	{
	case SF_Mono8:		return 8;
	case SF_Mono16:		return 16;
	case SF_Stereo8:	return 16;
	case SF_Stereo16:	return 32;
	case SF_Unknown:
	default:
		Sys_Errorf( TEXT( "Unknown sample format 0x%X" ), InSampleFormat );
		return 0;
	}
}

/*
==================
CAudioDevice::CAudioDevice
==================
*/
CAudioDevice::CAudioDevice()
	: bIsMuted( false )
	, alDevice( nullptr )
	, alContext( nullptr )
	, globalVolume( 100.f )
	, platformAudioHeadroom( 1.f )
{}

/*
==================
CAudioDevice::~CAudioDevice
==================
*/
CAudioDevice::~CAudioDevice()
{
	Shutdown();
}

/*
==================
CAudioDevice::Init
==================
*/
void CAudioDevice::Init()
{
	// Open audio device
	alDevice = alcOpenDevice( nullptr );
	if ( !alDevice )
	{
		Warnf( TEXT( "Failed to create the audio device\n" ) );
		return;
	}

	// Create and make current context
	alContext = alcCreateContext( alDevice, nullptr );
	if ( !alContext )
	{
		Warnf( TEXT( "Failed to create the audio context\n" ) );
		return;
	}

	alcMakeContextCurrent( alContext );

	// Print available playback devices
	Logf( TEXT( "Available playback devices: %s\n" ),		ANSI_TO_TCHAR( IsExtensionSupported( "ALC_ENUMERATE_ALL_EXT" ) ? alcGetString( nullptr, ALC_ALL_DEVICES_SPECIFIER ) : alcGetString( nullptr, ALC_DEVICE_SPECIFIER ) ) );
	Logf( TEXT( "Available capture devices: %s\n" ),		ANSI_TO_TCHAR( alcGetString( nullptr, ALC_CAPTURE_DEVICE_SPECIFIER ) ) );
	Logf( TEXT( "Default playback device: %s\n" ),			ANSI_TO_TCHAR( IsExtensionSupported( "ALC_ENUMERATE_ALL_EXT" ) ? alcGetString( nullptr, ALC_DEFAULT_ALL_DEVICES_SPECIFIER ) : alcGetString( nullptr, ALC_DEFAULT_DEVICE_SPECIFIER ) ) );
	Logf( TEXT( "Default capture device: %s\n" ),			ANSI_TO_TCHAR( alcGetString( nullptr, ALC_CAPTURE_DEFAULT_DEVICE_SPECIFIER ) ) );
	Logf( TEXT( "Selected audio device: %s\n" ),			ANSI_TO_TCHAR( IsExtensionSupported( "ALC_ENUMERATE_ALL_EXT" ) ? alcGetString( alDevice, ALC_ALL_DEVICES_SPECIFIER ) : alcGetString( alDevice, ALC_DEVICE_SPECIFIER ) ) );

	ALCint			major = 0;
	ALCint			minor = 0;
	alcGetIntegerv( alDevice, ALC_MAJOR_VERSION, 1, &major );
	alcGetIntegerv( alDevice, ALC_MINOR_VERSION, 1, &minor );
	Logf( TEXT( "ALC version %i.%i\n" ), major, minor );
	Logf( TEXT( "ALC extensions: %s\n" ),					ANSI_TO_TCHAR( alcGetString( alDevice, ALC_EXTENSIONS ) ) );

	// Print info by OpenAL and extensions		
	Logf( TEXT( "OpenAL vendor: %s\n" ),					ANSI_TO_TCHAR( alGetString( AL_VENDOR ) ) );
	Logf( TEXT( "OpenAL renderer: %s\n" ),					ANSI_TO_TCHAR( alGetString( AL_RENDERER ) ) );
	Logf( TEXT( "OpenAL version: %s\n" ),					ANSI_TO_TCHAR( alGetString( AL_VERSION ) ) );
	Logf( TEXT( "OpenAL extensions: %s\n" ),				ANSI_TO_TCHAR( alGetString( AL_EXTENSIONS ) ) );

	// Init platform audio headroom
	float		headroom = g_Config.GetValue( CT_Engine, TEXT( "Audio.Audio" ), TEXT( "PlatformHeadroomDB" ) ).GetNumber();
	if ( headroom != 0.f )
	{
		// Convert dB to linear volume
		platformAudioHeadroom = SMath::Pow( 10.f, headroom / 20.f );
	}
	else
	{
		platformAudioHeadroom = 1.f;
	}

	// Getting global volume from config
	float		globalVolume = 1.f;
	{
		CConfigValue		configGlobalVolume = g_Config.GetValue( CT_Engine, TEXT( "Audio.Audio" ), TEXT( "GlobalVolume" ) );
		if ( configGlobalVolume.IsValid() && ( configGlobalVolume.GetType() == CConfigValue::T_Int || configGlobalVolume.GetType() == CConfigValue::T_Float ) )
		{
			globalVolume = configGlobalVolume.GetNumber();
		}
	}

	// Initialize listener spatial
	SetListenerSpatial( SMath::vectorZero, SMath::vectorForward, SMath::vectorUp );
	SetGlobalVolume( globalVolume );
}

/*
==================
CAudioDevice::Shutdown
==================
*/
void CAudioDevice::Shutdown()
{
	alcMakeContextCurrent( nullptr );
	if ( alContext )
	{
		alcDestroyContext( alContext );
	}

	if ( alDevice )
	{
		alcCloseDevice( alDevice );
	}

	alContext = nullptr;
	alDevice = nullptr;
}

/*
==================
CAudioDevice::IsExtensionSupported
==================
*/
bool CAudioDevice::IsExtensionSupported( const std::string& InExtension ) const
{
	if ( InExtension.size() > 2 && InExtension.substr( 0, 3 ) == "ALC" )
	{
		Assert( alDevice );
		return alcIsExtensionPresent( alDevice, InExtension.c_str() ) != ALC_FALSE;
	}
	else
	{
		return alIsExtensionPresent( InExtension.c_str() ) != ALC_FALSE;
	}
}