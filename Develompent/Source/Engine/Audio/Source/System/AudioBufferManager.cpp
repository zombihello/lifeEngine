#include "System/AudioBufferManager.h"

FAudioBufferRef FAudioBufferManager::Find( FAudioBank* InAudioBank )
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
	FAudioBankInfo		audioBankInfo;
	FAudioBankHandle	audioBankHandle = InAudioBank->OpenBank( audioBankInfo );
	check( audioBankHandle );

	byte*				sampleData = ( byte* )malloc( audioBankInfo.numSamples );
	InAudioBank->ReadBankPCM( audioBankHandle, sampleData, audioBankInfo.numSamples );

	FAudioBufferRef		audioBuffer = new FAudioBuffer();
	audioBuffer->Append( audioBankInfo.format, sampleData, audioBankInfo.numSamples, audioBankInfo.rate );

	// Free all allocated temporary data and return audio buffer
	free( sampleData );
	InAudioBank->CloseBank( audioBankHandle );

	buffers.insert( std::make_pair( InAudioBank, audioBuffer ) );
	return audioBuffer;
}