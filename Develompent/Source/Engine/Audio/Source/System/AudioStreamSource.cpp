#include "System/AudioStreamSource.h"
#include "Logger/LoggerMacros.h"

FAudioStreamRunnable::FAudioStreamRunnable( FAudioStreamSource* InStreamSource )
	: streamSource( InStreamSource )
{}

bool FAudioStreamRunnable::Init()
{
	return true;
}

uint32 FAudioStreamRunnable::Run()
{
	bool			bRequestStop = false;
	check( streamSource );

	{
		// Check if the thread was launched is stopped
		FScopeLock		scopeLock( &streamSource->csStreamData );
		if ( streamSource->status == ASS_Stoped )
		{
			streamSource->bIsStreaming = false;
			return 0;
		}
	}

	// Resize sample buffer for streaming audio
	samples.resize( streamSource->audioBankInfo.rate * streamSource->audioBankInfo.numChannels );

	// Create the buffers
	alGenBuffers( BufferCount, alBuffers );

	// Fill the queue
	bRequestStop = FillQueue();

	// Play the sound
	alSourcePlay( streamSource->GetALHandle() );

	{
		// Check if the thread was launched is paused
		FScopeLock		scopeLock( &streamSource->csStreamData );	
		if ( streamSource->status == ASS_Paused )
		{
			alSourcePause( streamSource->GetALHandle() );
		}
	}

	while ( true )
	{
		TSharedPtr<FAudioBank>		audioBankRef = streamSource->audioBank.Pin();		// Lock audio bank for him not unloaded ahead of time
		{
			FScopeLock		scopeLock( &streamSource->csStreamData );
			if ( !streamSource->bIsStreaming || !streamSource->audioBank )
			{
				break;
			}
		}

		// The stream has been interrupted!
		if ( streamSource->GetStatus() == ASS_Stoped )
		{
			// If not request stop - just continue
			if ( !bRequestStop )
			{
				alSourcePlay( streamSource->GetALHandle() );
			}
			else
			{
				// End streaming
				FScopeLock		scopeLock( &streamSource->csStreamData );
				streamSource->bIsStreaming = false;
			}
		}

		// Get the number of buffers that have been processed (i.e. ready for reuse)
		ALint			processed = 0;
		alGetSourcei( streamSource->GetALHandle(), AL_BUFFERS_PROCESSED, &processed );

		while ( processed-- )
		{
			// Pop the first unused buffer from the queue
			ALuint			buffer = 0;
			alSourceUnqueueBuffers( streamSource->GetALHandle(), 1, &buffer );

			// Find its number
			uint32		bufferID = 0;
			for ( uint32 index = 0; index < BufferCount; ++index )
			{
				if ( alBuffers[ index ] == buffer )
				{
					bufferID = index;
					break;
				}
			}

			// Fill it and push it back into the playing queue
			if ( FillAndPushBuffer( bufferID ) )
			{
				bRequestStop = true;
			}
		}

		// Leave some time for the other threads if the stream is still playing
		if ( streamSource->GetStatus() != ASS_Stoped )
		{
			appSleep( 0.01f );		// 10 milliseconds
		}
	}

	// Stop the playback
	alSourceStop( streamSource->GetALHandle() );

	// Dequeue any buffer left in the queue
	ClearQueue();

	// Delete the buffers
	alSourcei( streamSource->GetALHandle(), AL_BUFFER, 0 );
	alDeleteBuffers( BufferCount, alBuffers );
	return 0;
}

void FAudioStreamRunnable::Stop()
{}

void FAudioStreamRunnable::Exit()
{}

bool FAudioStreamRunnable::FillQueue()
{	
	// Fill and enqueue all the available buffers
	bool			requestStop = false;
	for ( uint32 index = 0; index < BufferCount && !requestStop; ++index )
	{
		// Since no sound has been loaded yet, we can't schedule loop seeks preemptively,
		// So if we start on EOF or Loop End, we let FillAndPushBuffer() adjust the sample count
		if ( FillAndPushBuffer( index ) )
		{
			requestStop = true;
		}
	}

	return requestStop;
}

void FAudioStreamRunnable::ClearQueue()
{
	// Get the number of buffers still in the queue
	ALint			queued;
	alGetSourcei( streamSource->GetALHandle(), AL_BUFFERS_QUEUED, &queued );

	// Dequeue them all
	ALuint			buffer;
	for ( ALint index = 0; index < queued; ++index )
	{
		alSourceUnqueueBuffers( streamSource->GetALHandle(), 1, &buffer );
	}
}

bool FAudioStreamRunnable::FillAndPushBuffer( uint32 InBufferIndex )
{
	bool		requestStop = false;

	// Acquire audio data, also address EOF and error cases if they occur
	FChunk		data = { nullptr, 0 };
	for ( uint32 retryCount = 0; !GetData( data ) && retryCount < BufferRetries; ++retryCount )
	{
		// Check if the stream must loop or stop
		if ( !streamSource->bIsLoop )
		{
			requestStop = true;
			break;
		}

		//  If we looped - move to start file
		streamSource->audioBank.Pin()->SeekBankPCM( streamSource->audioBankHandle, 0 );
	}

	// Fill the buffer if some data was returned
	if ( data.samples && data.numSamples )
	{
		uint32			buffer = alBuffers[ InBufferIndex ];

		// Fill the buffer and push it into the sound queue
		alBufferData( buffer, appSampleFormatToEngine( streamSource->audioBankInfo.format ), data.samples, data.numSamples, streamSource->audioBankInfo.rate );
		alSourceQueueBuffers( streamSource->GetALHandle(), 1, &buffer );
	}
	else
	{
		// If we get here, we most likely ran out of retries
		requestStop = true;
	}

	return requestStop;
}

bool FAudioStreamRunnable::GetData( FChunk& OutData )
{
	FScopeLock	scopeLock( &streamSource->csStreamData );
	TSharedPtr<FAudioBank>		audioBankRef	= streamSource->audioBank.Pin();
	uint32						toFill			= samples.size();
	uint64						currentOffset	= audioBankRef->GetOffsetBankPCM( streamSource->audioBankHandle );
	uint64						numSamples		= streamSource->audioBankInfo.numSamples;

	// If there are less samples left than the buffer size, we count how many samples need to be read
	if ( currentOffset + toFill > numSamples )
	{
		toFill = numSamples - currentOffset;
	}

	// Fill the chunk parameters
	OutData.samples		= &samples[ 0 ];
	OutData.numSamples	= audioBankRef->ReadBankPCM( streamSource->audioBankHandle, &samples[ 0 ], toFill );
	currentOffset		+= OutData.numSamples;

	// Check if we have stopped obtaining samples or reached either the EOF or the loop end point
	return OutData.numSamples != 0 && currentOffset != numSamples;
}

FAudioStreamSource::FAudioStreamSource()
	: bIsStreaming( false )
	, bIsLoop( false )
	, status( ASS_Stoped )
	, audioBankHandle( nullptr )
	, audioStreamRunnable( nullptr )
	, threadStreamData( nullptr )
{}

FAudioStreamSource::~FAudioStreamSource()
{
	if ( audioBankHandle )
	{
		SetAudioBank( nullptr );
	}
}

void FAudioStreamSource::Play()
{
	bool					isStreaming = false;
	EAudioSourceStatus		status = ASS_Stoped;

	// Getting values state
	{
		FScopeLock		scopeLock( &csStreamData );
		isStreaming		= bIsStreaming;
		status			= this->status;
	}

	// If sound is streaming and thread state is ASS_Paused - start play sound
	// If sound is streaming and thread state is ASS_Playing - restart play sound
	if ( isStreaming && status == ASS_Paused )
	{
		FScopeLock		scopeLock( &csStreamData );
		this->status = ASS_Playing;
		alSourcePlay( GetALHandle() );
		return;
	}
	else if ( isStreaming && status == ASS_Playing )
	{
		Stop();
	}

	// Start thread for streaming data
	bIsStreaming		= true;
	this->status		= ASS_Playing;
	audioStreamRunnable = new FAudioStreamRunnable( this );
	threadStreamData	= GThreadFactory->CreateThread( audioStreamRunnable, TEXT( "AudioStream" ), true, true, 0, TP_Normal );
}

void FAudioStreamSource::Pause()
{
	// Set state to ASS_Paused
	{
		FScopeLock		scopeLock( &csStreamData );
		if ( !bIsStreaming )
		{
			return;
		}
		status = ASS_Paused;
	}

	alSourcePause( GetALHandle() );
}

void FAudioStreamSource::Stop()
{
	// Request the thread to terminate
	{
		FScopeLock		scopeLock( &csStreamData );
		bIsStreaming	= false;
	}

	// Wait for the thread to terminate
	if ( threadStreamData )
	{
		threadStreamData->WaitForCompletion();
		audioStreamRunnable		= nullptr;
		threadStreamData		= nullptr;
	}

	// Move to the beginning
	TSharedPtr<FAudioBank>		audioBankRef = audioBank.Pin();
	if ( audioBankHandle && audioBankRef )
	{
		audioBankRef->SeekBankPCM( audioBankHandle, 0 );
	}
}

void FAudioStreamSource::SetLoop( bool InIsLoop )
{
	bIsLoop = InIsLoop;
}

void FAudioStreamSource::OpenBank( const FAudioBankPtr& InAudioBank )
{
	// If already opened bank - close
	if ( audioBankHandle )
	{
		CloseBank();
	}

	// If new bank is nullptr - exit from method
	TSharedPtr<FAudioBank>		audioBankRef = InAudioBank.Pin();
	if ( !audioBankRef )
	{
		return;
	}

	// Open bank
	audioBankHandle = audioBankRef->OpenBank( audioBankInfo );
	bIsStreaming = false;

	// Check if the format is valid
	if ( audioBankInfo.format == SF_Unknown )
	{
		CloseBank();
		LE_LOG( LT_Warning, LC_Audio, TEXT( "Unsupported sample format in audio bank '%s'" ), audioBankRef->GetAssetName().c_str() );
		return;
	}
}

void FAudioStreamSource::CloseBank()
{
	if ( !audioBankHandle )
	{
		return;
	}

	Stop();
	{
		TSharedPtr<FAudioBank>		audioBankRef = audioBank.Pin();
		if ( audioBankRef )
		{
			audioBankRef->CloseBank( audioBankHandle );
		}
	}
	audioBankHandle = nullptr;
}

void FAudioStreamSource::SetAudioBank( const FAudioBankPtr& InAudioBank )
{
	// If bank opened - close
	if ( audioBankHandle )
	{
		CloseBank();
	}

	// Open new bank
	if ( InAudioBank )
	{
		OpenBank( InAudioBank );
	}

	audioBank = InAudioBank;
}

bool FAudioStreamSource::IsLooped() const
{
	return bIsLoop;
}

EAudioSourceStatus FAudioStreamSource::GetStatus() const
{
	ALint			alStatus = 0;
	alGetSourcei( GetALHandle(), AL_SOURCE_STATE, &alStatus );

	switch ( alStatus )
	{
	case AL_PLAYING:	return ASS_Playing;
	case AL_PAUSED:		return ASS_Paused;	
	case AL_STOPPED:
	default:
	{
		FScopeLock		scopeLock( &csStreamData );
		if ( bIsStreaming )
		{
			return status;
		}
		return ASS_Stoped;
	}
	}
}