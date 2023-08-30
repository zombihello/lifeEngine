#include "System/AudioBufferManager.h"

/*
==================
CAudioBufferManager::Find
==================
*/
AudioBufferRef_t CAudioBufferManager::Find( const TAssetHandle<CAudioBank>& InAudioBank )
{
	// If already loaded buffer - return it
	{
		auto	it = buffers.find( InAudioBank );
		if ( it != buffers.end() )
		{
			return it->second;
		}
	}

	// Load samples and create buffer
	Assert( InAudioBank.IsAssetValid() );

	TSharedPtr<CAudioBank>		audioBankRef = InAudioBank.ToSharedPtr();
	AudioBankInfo				audioBankInfo;
	AudioBankHandle_t			audioBankHandle = audioBankRef->OpenBank( audioBankInfo );
	if ( audioBankHandle )
	{
		byte* sampleData = ( byte* ) malloc( audioBankInfo.numSamples );
		audioBankRef->ReadBankPCM( audioBankHandle, sampleData, audioBankInfo.numSamples );

		AudioBufferRef_t		audioBuffer = new CAudioBuffer();
		audioBuffer->Append( audioBankInfo.format, sampleData, audioBankInfo.numSamples, audioBankInfo.rate );

		// Free all allocated temporary data and return audio buffer
		free( sampleData );
		audioBankRef->CloseBank( audioBankHandle );

		buffers.insert( std::make_pair( InAudioBank, audioBuffer ) );
		return audioBuffer;
	}

	return nullptr;
}