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
class CAudioStreamRunnable : public CRunnable
{
public:
	friend class CAudioStreamSource;

	/**
	 * Constructor
	 * 
	 * @param InStreamSource Owner stream source
	 */
	CAudioStreamRunnable( CAudioStreamSource* InStreamSource );

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
	struct Chunk
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
	bool GetData( Chunk& OutData );
	
	class CAudioStreamSource*		streamSource;				/**< Stream source */
	uint32							alBuffers[ BufferCount ];	/**< OpenAL buffers */
	std::vector<byte>				samples;					/**< Buffer of samples for streamed to buffers */
};

/**
 * @ingroup Audio
 * @brief Audio stream source
 */
class CAudioStreamSource : public CAudioSource
{
public:
	friend class CAudioStreamRunnable;

	/**
	 * Constructor
	 */
	CAudioStreamSource();

	/**
	 * Destructor
	 */
	~CAudioStreamSource();

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
	virtual void SetAudioBank( const TAssetHandle<CAudioBank>& InAudioBank ) override;

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
	void OpenBank( const TAssetHandle<CAudioBank>& InAudioBank );

	/**
	 * Close bank
	 */
	void CloseBank();

#if WITH_EDITOR
	/**
	 * On audio bank is updated
	 * @param InAudioBank		Audio bank
	 */
	void OnAudioBankUpdated( class CAudioBank* InAudioBank );
#endif // WITH_EDITOR

	bool						bIsStreaming;			/**< Is streamed in this time audio */
	bool						bIsLoop;				/**< Is need loop audio */
	EAudioSourceStatus			status;					/**< Source audio status */
	AudioBankHandle_t			audioBankHandle;		/**< Handle to opened bank for streamed audio */
	AudioBankInfo				audioBankInfo;			/**< Info about opened bank */
	mutable CCriticalSection	csStreamData;			/**< Critical section of stream data */
	CAudioStreamRunnable*		audioStreamRunnable;	/**< Audio stream runnable */
	CRunnableThread*			threadStreamData;		/**< Thread for streaming audio */
};

#endif // !AUDIOSTREAMSOURCE_H