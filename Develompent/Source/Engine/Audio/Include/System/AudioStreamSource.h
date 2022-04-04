/**
 * @file
 * @addtogroup Audio Audio
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef AUDIOSTREAMSOURCE_H
#define AUDIOSTREAMSOURCE_H

#include <vector>

#include "System/AudioSource.h"
#include "System/ThreadingBase.h"

/**
 * @ingroup Audio
 * Runnable thread for audio stream
 */
class FAudioStreamRunnable : public FRunnable
{
public:
	friend class FAudioStreamSource;

	/**
	 * Constructor
	 * 
	 * @param InStreamSource Owner stream source
	 */
	FAudioStreamRunnable( FAudioStreamSource* InStreamSource );

	/**
	 * @brief Initialize
	 *
	 * Allows per runnable object initialization. NOTE: This is called in the
	 * context of the thread object that aggregates this, not the thread that
	 * passes this runnable to a new thread.
	 *
	 * @return True if initialization was successful, false otherwise
	 */
	virtual bool Init() override;

	/**
	 * @brief Run
	 *
	 * This is where all per object thread work is done. This is only called
	 * if the initialization was successful.
	 *
	 * @return The exit code of the runnable object
	 */
	virtual uint32 Run() override;

	/**
	 * @brief Stop
	 *
	 * This is called if a thread is requested to terminate early
	 */
	virtual void Stop() override;

	/**
	 * @brief Exit
	 *
	 * Called in the context of the aggregating thread to perform any cleanup.
	 */
	virtual void Exit() override;

private:
	/**
	 * Enumeration of settings of stream audio
	 */
	enum
	{
		BufferCount		= 3,	/**< Count audio buffers for stream */
		BufferRetries	= 2		/**< Count retries to read from archive */
	};

	/**
	 * Struct of streamed chunk
	 */
	struct FChunk
	{
		const byte*		samples;	/**< Pointer to array of samples */
		uint32			numSamples;	/**< Number samples in array */
	};

	/**
	 * Fill OpenAL queue buffers
	 * @return Return true if need stop thread, else return false
	 */
	bool FillQueue();

	/**
	 * Clear OpenAL queue buffers
	 */
	void ClearQueue();

	/**
	 * Fill OpenAL buffer and push to queue
	 * 
	 * @param InBufferIndex Buffer index to update
	 * @return Return true if need stop thread, else return false
	 */
	bool FillAndPushBuffer( uint32 InBufferIndex );

	/**
	 * Get loaded data from bank
	 * 
	 * @param OutData Output loaded data
	 * @return Return true if seccussed loaded, else return false
	 */
	bool GetData( FChunk& OutData );
	
	class FAudioStreamSource*		streamSource;				/**< Stream source */
	uint32							alBuffers[ BufferCount ];	/**< OpenAL buffers */
	std::vector<byte>				samples;					/**< Buffer of samples for streamed to buffers */
};

/**
 * @ingroup Audio
 * @brief Audio stream source
 */
class FAudioStreamSource : public FAudioSource
{
public:
	friend class FAudioStreamRunnable;

	/**
	 * Constructor
	 */
	FAudioStreamSource();

	/**
	 * Destructor
	 */
	~FAudioStreamSource();

	/**
	 * Play
	 */
	virtual void Play() override;

	/**
	 * Pause
	 */
	virtual void Pause() override;

	/**
	 * Stop
	 */
	virtual void Stop() override;

	/**
	 * Set loop
	 * @param InIsLoop Is need loop sound
	 */
	virtual void SetLoop( bool InIsLoop ) override;

	/**
	 * Set audio bank
	 * @param InAudioBank Audio bank
	 */
	virtual void SetAudioBank( FAudioBank* InAudioBank ) override;

	/**
	 * Is looped
	 * @return Return true if sound is looped, else return false
	 */
	virtual bool IsLooped() const override;

	/**
	 * Get audio source status
	 * @return Return audio source status
	 */
	virtual EAudioSourceStatus GetStatus() const override;

private:
	/**
	 * Open bank for stream
	 * @param InAudioBank Audio bank
	 */
	void OpenBank( FAudioBank* InAudioBank );

	/**
	 * Close bank
	 */
	void CloseBank();

	bool						bIsStreaming;			/**< Is streamed in this time audio */
	bool						bIsLoop;				/**< Is need loop audio */
	EAudioSourceStatus			status;					/**< Source audio status */
	FAudioBankHandle			audioBankHandle;		/**< Handle to opened bank for streamed audio */
	FAudioBankInfo				audioBankInfo;			/**< Info about opened bank */
	mutable FCriticalSection	csStreamData;			/**< Critical section of stream data */
	FAudioStreamRunnable*		audioStreamRunnable;	/**< Audio stream runnable */
	FRunnableThread*			threadStreamData;		/**< Thread for streaming audio */
};

#endif // !AUDIOSTREAMSOURCE_H