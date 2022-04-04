/**
 * @file
 * @addtogroup Audio Audio
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef AUDIOBANK_H
#define AUDIOBANK_H

#include <vector>

#include "System/Package.h"
#include "System/AudioDevice.h"
#include "System/AudioBuffer.h"

 /**
  * @ingroup Audio
  * @brief Reference to FAudioBank
  */
typedef TRefCountPtr< class FAudioBank >				FAudioBankRef;

/**
 * @ingroup Audio
 * @brief Typedef of handle opened audio bank
 */
typedef void*											FAudioBankHandle;

/**
 * @ingroup Audio
 * @brief Audio bank info
 */
struct FAudioBankInfo
{
	ESampleFormat		format;			/**< Sample format */
	uint32				rate;			/**< Rate */
	uint32				numSamples;		/**< Number of samples */
	uint32				numChannels;	/**< Number of channels */
};

/**
 * @ingroup Audio
 * @brief Audio bank
 */
class FAudioBank : public FAsset
{
public:
	/**
	 * Constructor
	 */
	FAudioBank();

	/**
	 * Destructor
	 */
	~FAudioBank();

	/**
	 * Serialize
	 *
	 * @param[in] InArchive Archive
	 */
	virtual void Serialize( class FArchive& InArchive ) override;

	/**
	 * Open bank
	 * 
	 * @param OutBankInfo Output parameter, return info about bank
	 * @return Return handle of opened bank. If failed return nullptr
	 */
	FAudioBankHandle OpenBank( FAudioBankInfo& OutBankInfo );

	/**
	 * Close bank
	 * 
	 * @param InBankHandle Handle to opened bank
	 */
	void CloseBank( FAudioBankHandle InBankHandle );

	/**
	 * Read PCM from bank
	 * 
	 * @param InBankHandle Handle to opened bank
	 * @param InSamples Pointer to destination buffer
	 * @param InMaxSize Max size (in bytes) of destination buffer
	 * @return Return number readed bytes
	 */
	uint64 ReadBankPCM( FAudioBankHandle InBankHandle, byte* InSamples, uint64 InMaxSize );

	/**
	 * Seek PCM in bank
	 * 
	 * @param InBankHandle Handle to opened bank
	 * @param InSampleOffset Offset in PCM of bank
	 */
	void SeekBankPCM( FAudioBankHandle InBankHandle, uint64 InSampleOffset );

	/**
	 * Get PCM offset in bank
	 * 
	 * @param InBankHandle Handle to opened bank
	 * @return Return current offset PCM in bank
	 */
	uint64 GetOffsetBankPCM( FAudioBankHandle InBankHandle ) const;

#if WITH_EDITOR
	/**
	 * Set source OGG file
	 * This method takes an Ogg/Vorbis file, which it decodes and applies in the audio engine
	 *
	 * @param InPath Path to Ogg/Vorbis file
	 */
	void SetSourceOGGFile( const std::wstring& InPath );
#endif // WITH_EDITOR

	/**
	 * Is empty bank
	 * @return Return true if bank is empty, else return false
	 */
	FORCEINLINE bool IsEmpty() const
	{
		return rawDataSize == 0;
	}

	/**
	 * Get audio buffer
	 * @return Return audio buffer, if failed return nullptr
	 */
	FAudioBufferRef GetAudioBuffer();

private:
	uint64				offsetToRawData;	/**< Offset in archive to raw data */
	uint64				rawDataSize;		/**< Size in bytes of raw data */
	std::wstring		pathToArchive;		/**< Path to archive */
	FAudioBufferRef		audioBuffer;		/**< Audio buffer with fully loaded bank. Used only by FAudioSource */

#if WITH_EDITOR
	std::vector<byte>	rawData;			/**< Raw data of Ogg/Vorbis file */
#endif // WITH_EDITOR
};

//
// Serialization
//

FORCEINLINE FArchive& operator<<( FArchive& InArchive, FAudioBankRef& InValue )
{
	FAssetRef			asset = InValue;
	InArchive << asset;

	if ( InArchive.IsLoading() )
	{
		InValue = asset;
	}
	return InArchive;
}

FORCEINLINE FArchive& operator<<( FArchive& InArchive, const FAudioBankRef& InValue )
{
	check( InArchive.IsSaving() );
	InArchive << ( FAssetRef )InValue;
	return InArchive;
}

#endif // !AUDIOBANK_H