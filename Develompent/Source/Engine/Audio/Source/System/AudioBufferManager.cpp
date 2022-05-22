#include "System/AudioBufferManager.h"

FAudioBufferRef FAudioBufferManager::Find( const TAssetHandle<FAudioBank>& InAudioBank )
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
	check( InAudioBank );

	TSharedPtr<FAudioBank>		audioBankRef = InAudioBank.ToSharedPtr();
	FAudioBankInfo				audioBankInfo;
	FAudioBankHandle			audioBankHandle = audioBankRef->OpenBank( audioBankInfo );
	check( audioBankHandle );

	byte*				sampleData = ( byte* )malloc( audioBankInfo.numSamples );
	audioBankRef->ReadBankPCM( audioBankHandle, sampleData, audioBankInfo.numSamples );

	FAudioBufferRef		audioBuffer = new FAudioBuffer();
	audioBuffer->Append( audioBankInfo.format, sampleData, audioBankInfo.numSamples, audioBankInfo.rate );

	// Free all allocated temporary data and return audio buffer
	free( sampleData );
	audioBankRef->CloseBank( audioBankHandle );

	buffers.insert( std::make_pair( InAudioBank, audioBuffer ) );
	return audioBuffer;
}