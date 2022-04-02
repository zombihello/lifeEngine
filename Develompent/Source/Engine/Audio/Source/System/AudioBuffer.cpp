#include <vorbis/vorbisfile.h>
#include <vorbis/vorbisenc.h>
#include <ogg/ogg.h>
#include <AL/al.h>
#include <AL/alc.h>

#include "Logger/LoggerMacros.h"
#include "Containers/StringConv.h"
#include "System/AudioBuffer.h"

/**
 * Struct of info about OGG file in memory
 */
struct FOGGRawData
{
	byte*		currentPtr;		/**< Current pointer in raw data */
	byte*		beginPtr;		/**< Begin of raw data in memory */
	uint64		size;			/**< Size of raw data in bytes */
};

/**
 * Read OGG file from memory
 */
size_t Memory_ReadOgg( void* InDst, size_t InSize1, size_t InSize2, void* InUser )
{
	FOGGRawData*	rawData = ( FOGGRawData* )InUser;
	size_t			size = InSize1 * InSize2;

	// Trim the number of bytes to read if we've reached the end of the file 
	if ( rawData->currentPtr + size > rawData->beginPtr + rawData->size )
	{
		size = rawData->beginPtr + rawData->size - rawData->currentPtr;
	}

	memcpy( InDst, rawData->currentPtr, size );
	rawData->currentPtr += size;
	return size;
}

/**
 * Seek bytes in OGG file in memory
 */
int Memory_SeekOgg( void* InUser, ogg_int64_t InTo, int InType )
{
	FOGGRawData*	rawData = ( FOGGRawData* )InUser;
	switch ( InType )
	{
	case SEEK_CUR:		rawData->currentPtr += InTo;										break;
	case SEEK_END:		rawData->currentPtr = rawData->beginPtr + rawData->size - InTo;		break;
	case SEEK_SET:		rawData->currentPtr = rawData->beginPtr + InTo;						break;
	default:			return -1;
	}

	if ( rawData->currentPtr < rawData->beginPtr )
	{
		rawData->currentPtr = rawData->beginPtr;
		return -1;
	}

	if ( rawData->currentPtr > rawData->beginPtr + rawData->size )
	{
		rawData->currentPtr = rawData->beginPtr + rawData->size;
		return -1;
	}

	return 0;
}

/**
 * Close OGG file in memory
 */
int Memory_CloseOgg( void* InUser )
{
	return 0;
}

/**
 * Tell OGG file in memory
 */
long Memory_TellOgg( void* InUser )
{
	FOGGRawData*		rawData = ( FOGGRawData* )InUser;
	return ( rawData->currentPtr - rawData->beginPtr );
}

FAudioBuffer::FAudioBuffer()
	: FAsset( AT_AudioBuffer )
	, format( SF_Unknown )
	, rate( 0 )
	, channelCount( 0 )
	, alHandle( 0 )
{}

FAudioBuffer::~FAudioBuffer()
{
	if ( alHandle > 0 )
	{
		UnloadBank();
	}
}

void FAudioBuffer::Serialize( class FArchive& InArchive )
{
	FAsset::Serialize( InArchive );

	InArchive << rawData;
	if ( rawData.Num() > 0 )
	{
		LoadBank();
	}
}

void FAudioBuffer::SetRawData( const byte* InData, uint64 InSize )
{
	rawData.SetElements( InData, InSize );
	if ( InSize > 0 )
	{
		LoadBank();
	}
}

void FAudioBuffer::LoadBank()
{
	// If already loaded bank to OpenAL - destroy it
	if ( alHandle > 0 )
	{
		UnloadBank();
	}

	// Init callback for read OGG/Vorbis from raw data
	ov_callbacks				ovCallbacks;
	ovCallbacks.read_func		= Memory_ReadOgg;
	ovCallbacks.seek_func		= Memory_SeekOgg;
	ovCallbacks.close_func		= Memory_CloseOgg;
	ovCallbacks.tell_func		= Memory_TellOgg;

	// Init descriptor for read OGG/Vorbis from memory
	FOGGRawData					oggRawDataDesc;
	oggRawDataDesc.beginPtr		= rawData.GetData();
	oggRawDataDesc.currentPtr	= oggRawDataDesc.beginPtr;
	oggRawDataDesc.size			= rawData.Num();

	OggVorbis_File		oggVorbisFile;
	int32				error = ov_open_callbacks( &oggRawDataDesc, &oggVorbisFile, nullptr, -1, ovCallbacks );
	if ( error < 0 )
	{
		std::wstring				strError = TEXT( "unknow error" );
		switch ( error )
		{
		case OV_FALSE:				strError = TEXT( "not found" );															break;
		case OV_EREAD:				strError = TEXT( "a read from media returned an error" );								break;
		case OV_ENOTVORBIS:			strError = TEXT( "bitstream does not contain any Vorbis data" );						break;
		case OV_EVERSION:			strError = TEXT( "vorbis version mismatch" );											break;
		case OV_EBADHEADER:			strError = TEXT( "invalid Vorbis bitstream header" );									break;
		case OV_EFAULT:				strError = TEXT( "internal logic fault; indicates a bug or heap/stack corruption" );	break;
		}

		LE_LOG( LT_Warning, LC_Audio, TEXT( "Failed loading bank: %s" ), strError.c_str() );
		return;
	}

	// Getting info about audio buffer
	vorbis_info*		vorbisInfo = ov_info( &oggVorbisFile, -1 );
	check( vorbisInfo );
	channelCount	= vorbisInfo->channels;
	format			= vorbisInfo->channels == 1 ? SF_Mono16 : SF_Stereo16;
	rate			= vorbisInfo->rate;
	
	uint32				sampleSize = ov_pcm_total( &oggVorbisFile, -1 ) * vorbisInfo->channels * 2;
	byte*				sampleData = ( byte* )malloc( sampleSize );
	check( sampleData );

	// Try to read the requested number of samples, stop only on error or end of file
	{
		uint64		readedSize = 0;
		byte*		readSampleData = sampleData;
		while ( readedSize < sampleSize )
		{
			uint64		bytesToRead = ( sampleSize - readedSize ) * sizeof( byte );
			long		bytesRead = ov_read( &oggVorbisFile, ( char* )readSampleData, bytesToRead, 0, 2, 1, nullptr );
			if ( bytesRead > 0 )
			{
				long		samplesRead = bytesRead / sizeof( byte );
				readedSize += samplesRead;
				readSampleData += samplesRead;
			}
			else
			{
				// Error or end of file
				break;
			}
		}

		if ( readedSize == 0 )
		{
			LE_LOG( LT_Warning, LC_Audio,  TEXT( "Failed read samples data in '%s'" ), GetAssetName().c_str() );
			return;
		}
		else
		{
			LE_LOG( LT_Log, LC_Audio, TEXT( "In audio buffer '%s' readed bank %.2f/%.2f kb" ), GetAssetName().c_str(), readedSize / 1024.f, sampleSize / 1024.f );
			sampleSize = readedSize;
		}
	}

	// Create OpenAL buffer and copy samples
	alGenBuffers( 1, &alHandle );
	alBufferData( alHandle, appSampleFormatToEngine( format ), sampleData, sampleSize, rate );

	// Free all allocated memory
	vorbis_info_clear( vorbisInfo );
	ov_clear( &oggVorbisFile );
	free( sampleData );

	// If we in game - remove raw data
	if ( !GIsEditor && !GIsCommandlet )
	{
		rawData.RemoveAllElements();
	}
}

void FAudioBuffer::UnloadBank()
{
	if ( alHandle == 0 )
	{
		return;
	}

	alDeleteBuffers( 1, &alHandle );
	alHandle = 0;
}