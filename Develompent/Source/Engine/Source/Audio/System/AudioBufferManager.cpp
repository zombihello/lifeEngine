/**
 * ************************************************************
 *                  This file is part of:
 *                      LIFEENGINE
 *          https://github.com/zombihello/lifeEngine
 * ************************************************************
 * Copyright (C) 2024 Yehor Pohuliaka.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "Audio/System/AudioBufferManager.h"

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