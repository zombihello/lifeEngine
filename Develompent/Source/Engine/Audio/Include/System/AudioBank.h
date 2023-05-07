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
#include <list>

#include "System/Package.h"
#include "System/AudioDevice.h"
#include "System/AudioBuffer.h"
#include "System/Delegate.h"
#include "Misc/SharedPointer.h"

/**
 * @ingroup Audio
 * @brief Typedef of handle opened audio bank
 */
typedef void*											AudioBankHandle_t;

#if WITH_EDITOR
/**
 * @ingroup Audio
 * @brief Delegate for called event when audio bank is updated
 */
DECLARE_MULTICAST_DELEGATE( COnAudioBankUpdated, class CAudioBank* );
#endif // WITH_EDITOR

/**
 * @ingroup Audio
 * @brief Audio bank info
 */
struct SAudioBankInfo
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
class CAudioBank : public CAsset, public TSharedFromThis<CAudioBank>
{
public:
	/**
	 * Constructor
	 */
	CAudioBank();

	/**
	 * Destructor
	 */
	~CAudioBank();

	/**
	 * Serialize
	 *
	 * @param[in] InArchive Archive
	 */
	virtual void Serialize( class CArchive& InArchive ) override;

	/**
	 * Open bank
	 * 
	 * @param OutBankInfo Output parameter, return info about bank
	 * @return Return handle of opened bank. If failed return nullptr
	 */
	AudioBankHandle_t OpenBank( SAudioBankInfo& OutBankInfo );

	/**
	 * Close bank
	 * 
	 * @param InBankHandle Handle to opened bank
	 */
	FORCEINLINE void CloseBank( AudioBankHandle_t InBankHandle )
	{
		CloseBankInternal( InBankHandle );
	} 

	/**
	 * Read PCM from bank
	 * 
	 * @param InBankHandle Handle to opened bank
	 * @param InSamples Pointer to destination buffer
	 * @param InMaxSize Max size (in bytes) of destination buffer
	 * @return Return number readed bytes
	 */
	uint64 ReadBankPCM( AudioBankHandle_t InBankHandle, byte* InSamples, uint64 InMaxSize );

	/**
	 * Seek PCM in bank
	 * 
	 * @param InBankHandle Handle to opened bank
	 * @param InSampleOffset Offset in PCM of bank
	 */
	void SeekBankPCM( AudioBankHandle_t InBankHandle, uint64 InSampleOffset );

	/**
	 * Get PCM offset in bank
	 * 
	 * @param InBankHandle Handle to opened bank
	 * @return Return current offset PCM in bank
	 */
	uint64 GetOffsetBankPCM( AudioBankHandle_t InBankHandle ) const;

	/**
	 * Set OGG file for this bank
	 * This method takes an Ogg/Vorbis file, which it decodes and applies in the audio engine
	 *
	 * @param InPath Path to Ogg/Vorbis file
	 */
	void SetOGGFile( const std::wstring& InPath );

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
	AudioBufferRef_t GetAudioBuffer();

#if WITH_EDITOR
	/**
	 * Get delegate of event when audio bank is updated
	 * @return Return delegate of event when audio bank is updated
	 */
	FORCEINLINE COnAudioBankUpdated& OnAudioBankUpdated() const
	{
		return onAudioBankUpdated;
	}
#endif // WITH_EDITOR

private:
	/**
	 * Close bank. Internal implementation
	 *
	 * @param InBankHandle			Handle to opened bank
	 * @param InNeedFreeFromList	If setted TRUE method remove item of opened handle from 'openedHandles'
	 */
	void CloseBankInternal( AudioBankHandle_t InBankHandle, bool InNeedFreeFromList = true );

	uint64							offsetToRawData;	/**< Offset in archive to raw data */
	uint64							rawDataSize;		/**< Size in bytes of raw data */
	std::wstring					pathToArchive;		/**< Path to archive */
	AudioBufferRef_t				audioBuffer;		/**< Audio buffer with fully loaded bank. Used only by CAudioSource */
	std::list<AudioBankHandle_t>	openedHandles;		/**< List opened handles of this audio bank */

#if WITH_EDITOR
	mutable COnAudioBankUpdated		onAudioBankUpdated;	/**< Event when audio bank is updated */
#endif // WITH_EDITOR
};

//
// Serialization
//

FORCEINLINE CArchive& operator<<( CArchive& InArchive, TAssetHandle<CAudioBank>& InValue )
{
	TAssetHandle<CAsset>	asset = InValue;
	InArchive << asset;

	if ( InArchive.IsLoading() )
	{
		InValue = asset;
	}
	return InArchive;
}

FORCEINLINE CArchive& operator<<( CArchive& InArchive, const TAssetHandle<CAudioBank>& InValue )
{
	Assert( InArchive.IsSaving() );
	InArchive << ( TAssetHandle<CAsset> )InValue;
	return InArchive;
}

#endif // !AUDIOBANK_H