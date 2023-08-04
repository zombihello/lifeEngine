#include <vorbis/vorbisfile.h>
#include <vorbis/vorbisenc.h>
#include <ogg/ogg.h>

#include "Misc/AudioGlobals.h"
#include "Misc/CoreGlobals.h"
#include "System/BaseFileSystem.h"
#include "System/AudioBufferManager.h"
#include "System/MemoryArchive.h"
#include "Logger/LoggerMacros.h"
#include "System/AudioBank.h"

/**
 * Struct of info about OGG file in archive
 */
struct SArchiveOGGRawData
{
	CArchive*	archive;		/**< Archive */
	uint64		beginOffset;	/**< Begin of raw data in archive */	
	uint64		endOffset;		/**< Offset to end of raw data in archive */
};

/**
 * Struct of OGG parser from archive
 */
struct SAudioBankOGG
{
	/**
	 * Constructor
	 */
	SAudioBankOGG()
		: vorbisInfo( nullptr )
		, sampleOffset( 0 )
	{}

	OggVorbis_File		oggVorbisFile;		/**< Ogg/Vorbis file parser */
	vorbis_info*		vorbisInfo;			/**< Vorbis info */
	SAudioBankInfo		info;				/**< Info about bank */
	uint64				sampleOffset;		/**< Sample offset */
};

/*
==================
Archive_ReadOgg
==================
*/
size_t Archive_ReadOgg( void* InDst, size_t InSize1, size_t InSize2, void* InUser )
{
	SArchiveOGGRawData*		rawDataInfo		= ( SArchiveOGGRawData* )InUser;
	size_t					size			= InSize1 * InSize2;
	uint64					currentOffset	= rawDataInfo->archive->Tell();

	// Trim the number of bytes to read if we've reached the end of the file
	if ( currentOffset + size > rawDataInfo->endOffset )
	{
		size = rawDataInfo->endOffset - currentOffset;
	}

	if ( size > 0 )
	{
		rawDataInfo->archive->Serialize( InDst, size );
	}
	return size;
}

/*
==================
Archive_SeekOgg
==================
*/
int Archive_SeekOgg( void* InUser, ogg_int64_t InTo, int InType )
{
	SArchiveOGGRawData*		rawDataInfo		= ( SArchiveOGGRawData* )InUser;
	uint64					currentOffset	= rawDataInfo->archive->Tell();
	
	switch ( InType )
	{
	case SEEK_CUR:		currentOffset += InTo;								break;
	case SEEK_END:		currentOffset = rawDataInfo->endOffset - InTo;		break;
	case SEEK_SET:		currentOffset = rawDataInfo->beginOffset + InTo;	break;
	default:			return -1;
	}

	int		result = 0;
	if ( currentOffset < rawDataInfo->beginOffset )
	{
		currentOffset = rawDataInfo->beginOffset;
		result = -1;
	}

	if ( currentOffset > rawDataInfo->endOffset )
	{
		currentOffset = rawDataInfo->endOffset;
		result = -1;
	}

	rawDataInfo->archive->Seek( currentOffset );
	return result;
}

/*
==================
Archive_CloseOgg
==================
*/
int Archive_CloseOgg( void* InUser )
{
	SArchiveOGGRawData*		rawDataInfo = ( SArchiveOGGRawData* )InUser;
	delete rawDataInfo->archive;
	delete rawDataInfo;
	return 0;
}

/*
==================
Archive_TellOgg
==================
*/
long Archive_TellOgg( void* InUser )
{
	SArchiveOGGRawData*		rawDataInfo = ( SArchiveOGGRawData* )InUser;
	uint64					currentOffset = rawDataInfo->archive->Tell();
	return currentOffset - rawDataInfo->beginOffset;
}

/*
==================
CAudioBank::CAudioBank
==================
*/
CAudioBank::CAudioBank()
	: CAsset( AT_AudioBank )
	, offsetToRawData( -1 )
	, rawDataSize( 0 )
{}

/*
==================
CAudioBank::~CAudioBank
==================
*/
CAudioBank::~CAudioBank()
{
	if ( audioBuffer )
	{
		g_AudioBufferManager.Remove( GetAssetHandle() );
	}

	// Close all opened handles
	for ( auto itHandle = openedHandles.begin(), itHandleEnd = openedHandles.end(); itHandle != itHandleEnd; ++itHandle )
	{
		CloseBankInternal( *itHandle, false );
	}
}

/*
==================
CAudioBank::Serialize
==================
*/
void CAudioBank::Serialize( class CArchive& InArchive )
{
	CAsset::Serialize( InArchive );
	InArchive << rawDataSize;

	// If archive loaded, we remembre offset to raw data and seek it
	if ( InArchive.IsLoading() && rawDataSize > 0 )
	{
		rawData.clear();
		offsetToRawData		= InArchive.Tell();
		pathToArchive		= InArchive.GetPath();
		InArchive.Seek( offsetToRawData + rawDataSize );

		// Broadcast event of updated audio bank (only with editor build)
#if WITH_EDITOR
		onAudioBankUpdated.Broadcast( this );
#endif // WITH_EDITOR
	}

	// Save OGG file to archive if rawDataSize more then 0
	else if ( rawDataSize > 0 )
	{
		Assert( !rawData.empty() );
		InArchive.Serialize( rawData.data(), rawDataSize );
	}
}

/*
==================
CAudioBank::OnFullyLoad
==================
*/
void CAudioBank::OnFullyLoad()
{
	CAsset::OnFullyLoad();
	
	if ( rawDataSize > 0 && rawData.empty() )
	{
		CArchive*	archive = g_FileSystem->CreateFileReader( pathToArchive, AR_NoFail );
		archive->Seek( offsetToRawData );		
		rawData.resize( rawDataSize );
		archive->Serialize( rawData.data(), rawDataSize );
		
		offsetToRawData = 0;
		delete archive;
	}
}

/*
==================
CAudioBank::OpenBank
==================
*/
AudioBankHandle_t CAudioBank::OpenBank( SAudioBankInfo& OutBankInfo )
{
	// If bank is empty - we exit from method
	if ( IsEmpty() )
	{
		return nullptr;
	}
	Assert( offsetToRawData != -1 );

	// Init callback for read OGG/Vorbis from raw data
	ov_callbacks				ovCallbacks;
	ovCallbacks.read_func		= Archive_ReadOgg;
	ovCallbacks.seek_func		= Archive_SeekOgg;
	ovCallbacks.close_func		= Archive_CloseOgg;
	ovCallbacks.tell_func		= Archive_TellOgg;

	// Init descriptor for read OGG/Vorbis from memory
	SArchiveOGGRawData*			oggRawDataDesc = new SArchiveOGGRawData();
	if ( rawData.empty() )
	{
		oggRawDataDesc->archive = g_FileSystem->CreateFileReader( pathToArchive );
	}
	else
	{
		oggRawDataDesc->archive = new CMemoryReading( rawData );
	}
	oggRawDataDesc->beginOffset	= offsetToRawData;
	oggRawDataDesc->endOffset	= offsetToRawData + rawDataSize;

	// If failed open archive - we set 'error' to OV_FALSE
	SAudioBankOGG*		audioBankOgg = nullptr;
	int32				error = 0;
	if ( !oggRawDataDesc->archive )
	{
		error = OV_FALSE;
	}
	else
	{
		audioBankOgg = new SAudioBankOGG();
		oggRawDataDesc->archive->Seek( offsetToRawData );
		error = ov_open_callbacks( oggRawDataDesc, &audioBankOgg->oggVorbisFile, nullptr, -1, ovCallbacks );
	}
			
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

		delete oggRawDataDesc;
		if ( audioBankOgg )
		{
			delete audioBankOgg;
		}
	
		Warnf( TEXT( "Failed loading bank: %s\n" ), strError.c_str() );
		return nullptr;
	}

	// Getting info about bank
	audioBankOgg->vorbisInfo = ov_info( &audioBankOgg->oggVorbisFile, -1 );
	Assert( audioBankOgg->vorbisInfo );

	OutBankInfo.numChannels = audioBankOgg->vorbisInfo->channels;
	OutBankInfo.format		= audioBankOgg->vorbisInfo->channels == 1 ? SF_Mono16 : SF_Stereo16;
	OutBankInfo.rate		= audioBankOgg->vorbisInfo->rate;
	OutBankInfo.numSamples	= ov_pcm_total( &audioBankOgg->oggVorbisFile, -1 ) * audioBankOgg->vorbisInfo->channels * 2;
	audioBankOgg->info		= OutBankInfo;
	
	openedHandles.push_back( audioBankOgg );
	return audioBankOgg;
}

/*
==================
CAudioBank::CloseBankInternal
==================
*/
void CAudioBank::CloseBankInternal( AudioBankHandle_t InBankHandle, bool InNeedFreeFromList /* = true */ )
{
	if ( !InBankHandle )
	{
		return;
	}

	SAudioBankOGG*		audioBankOgg = ( SAudioBankOGG* )InBankHandle;
	vorbis_info_clear( audioBankOgg->vorbisInfo );
	ov_clear( &audioBankOgg->oggVorbisFile );
	delete audioBankOgg;

	// Remove from list opened handle
	if ( InNeedFreeFromList )
	{
		for ( auto itHandle = openedHandles.begin(), itHandleEnd = openedHandles.end(); itHandle != itHandleEnd; ++itHandle )
		{
			if ( *itHandle == InBankHandle )
			{
				openedHandles.erase( itHandle );
				return;
			}
		}
	}
}

/*
==================
CAudioBank::ReadBankPCM
==================
*/
uint64 CAudioBank::ReadBankPCM( AudioBankHandle_t InBankHandle, byte* InSamples, uint64 InMaxSize )
{
	Assert( InBankHandle && InSamples );
	SAudioBankOGG*		audioBankOgg = ( SAudioBankOGG* )InBankHandle;

	// Try to read the requested number of samples, stop only on error or end of file
	uint64			size = 0;
	while ( size < InMaxSize )
	{
		uint64			bytesToRead = ( InMaxSize - size ) * sizeof( byte );
		long			bytesRead = ov_read( &audioBankOgg->oggVorbisFile, ( char* )InSamples, bytesToRead, 0, 2, 1, nullptr );
		
		if ( bytesRead > 0 )
		{
			long		samplesRead		= bytesRead / sizeof( byte );
			size						+= samplesRead;
			InSamples					+= samplesRead;
			audioBankOgg->sampleOffset	+= samplesRead;
		}

		// Error or end of file
		else
		{		
			size = 0;

			// If less 0 - this is error
			if ( bytesToRead < 0 )
			{
				Sys_Errorf( TEXT( "Failed read from bank. Vorbisfile error code: 0x%X" ), bytesToRead );
			}
			break;
		}
	}

	return size;
}

/*
==================
CAudioBank::SeekBankPCM
==================
*/
void CAudioBank::SeekBankPCM( AudioBankHandle_t InBankHandle, uint64 InSampleOffset )
{
	Assert( InBankHandle );
	SAudioBankOGG*		audioBankOgg = ( SAudioBankOGG* )InBankHandle;
	ov_pcm_seek( &audioBankOgg->oggVorbisFile, InSampleOffset / audioBankOgg->info.numChannels );
	audioBankOgg->sampleOffset = InSampleOffset;
}

/*
==================
CAudioBank::GetOffsetBankPCM
==================
*/
uint64 CAudioBank::GetOffsetBankPCM( AudioBankHandle_t InBankHandle ) const
{
	Assert( InBankHandle );
	SAudioBankOGG*		audioBankOgg = ( SAudioBankOGG* )InBankHandle;
	return audioBankOgg->sampleOffset;
}

/*
==================
CAudioBank::SetOGGFile
==================
*/
void CAudioBank::SetOGGFile( const std::wstring& InPath )
{
	if ( pathToArchive == InPath )
	{
		return;
	}

	// Read raw data of Ogg/Vorbis file
	CArchive*		archive = g_FileSystem->CreateFileReader( InPath.c_str() );
	if ( !archive )
	{
		Warnf( TEXT( "Failed open archive '%s'\n" ), InPath.c_str() );
		return;
	}

	// Update info about raw data
	rawData.clear();
	offsetToRawData		= 0;
	pathToArchive		= InPath;
	rawDataSize			= archive->GetSize();

	// Serialize new data
	rawData.resize( rawDataSize );
	archive->Serialize( rawData.data(), rawDataSize );

	// Mark dirty asset and close archive
	MarkDirty();
	delete archive;

	// Remove loaded old audio buffer
	if ( audioBuffer )
	{
		g_AudioBufferManager.Remove( GetAssetHandle() );
		audioBuffer.SafeRelease();
	}

	// Broadcast event of updated audio bank (only with editor build)
#if WITH_EDITOR
	onAudioBankUpdated.Broadcast( this );
#endif // WITH_EDITOR
}

/*
==================
CAudioBank::GetAudioBuffer
==================
*/
AudioBufferRef_t CAudioBank::GetAudioBuffer()
{
	if ( audioBuffer )
	{
		return audioBuffer;
	}

	audioBuffer = g_AudioBufferManager.Find( GetAssetHandle() );
	return audioBuffer;
}