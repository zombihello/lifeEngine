/**
 * @file
 * @addtogroup Audio Audio
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef AUDIOBUFFER_H
#define AUDIOBUFFER_H

#include <vector>

#include "System/Package.h"
#include "Containers/BulkData.h"
#include "System/AudioDevice.h"

/**
 * @ingroup Audio
 * @brief Reference to FAudioBuffer
 */
typedef TRefCountPtr< class FAudioBuffer >				FAudioBufferRef;

/**
 * @ingroup Audio
 * @brief Audio buffer
 */
class FAudioBuffer : public FAsset
{
public:
	/**
	 * Constructor
	 */
	FAudioBuffer();

	/**
	 * Destructor
	 */
	~FAudioBuffer();

	/**
	 * Serialize
	 *
	 * @param[in] InArchive Archive
	 */
	virtual void Serialize( class FArchive& InArchive ) override;

	/**
	 * Set raw data
	 * This method takes an Ogg/Vorbis file, which it decodes and applies in the audio engine 
	 * 
	 * @param InData Pointer to raw data of Ogg/Vorbis file
	 * @param InSize Size of raw data
	 */
	void SetRawData( const byte* InData, uint64 InSize );

	/**
	 * Get OpenAL handle
	 * @return Return OpenAL handle to buffer. If not created return 0
	 */
	FORCEINLINE uint32 GetALHandle() const
	{
		return alHandle;
	}

private:
	/**
	 * Load bank from memory to OpenAL
	 */
	void LoadBank();

	/**
	 * Unload bank
	 */
	void UnloadBank();

	FBulkData<byte>		rawData;			/**< Raw data of Ogg/Vorbis file */
	ESampleFormat		format;				/**< Format of samples */
	uint32				rate;				/**< Sample rate */
	uint32				channelCount;		/**< Channel count */
	uint32				alHandle;			/**< Handle to OpenAL buffer */
};

//
// Serialization
//

FORCEINLINE FArchive& operator<<( FArchive& InArchive, FAudioBufferRef& InValue )
{
	FAssetRef			asset = InValue;
	InArchive << asset;

	if ( InArchive.IsLoading() )
	{
		InValue = asset;
	}
	return InArchive;
}

FORCEINLINE FArchive& operator<<( FArchive& InArchive, const FAudioBufferRef& InValue )
{
	check( InArchive.IsSaving() );
	InArchive << ( FAssetRef )InValue;
	return InArchive;
}

#endif // !AUDIOBUFFER_H