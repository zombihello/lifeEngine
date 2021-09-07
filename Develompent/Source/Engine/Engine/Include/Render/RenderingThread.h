/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef RENDERINGTHREAD_H
#define RENDERINGTHREAD_H

#include "System/ThreadingBase.h"

/**
 * @ingroup Engine
 * Whether the renderer is currently running in a separate thread.
 * If this is false, then all rendering commands will be executed immediately instead of being enqueued in the rendering command buffer.
 */
extern bool				GIsThreadedRendering;

/**
 * @ingroup Engine
 * ID of rendering thread
 */
extern uint32			GRenderingThreadId;

/**
 * @ingroup Engine
 * Is current thread is render
 *
 * @return True if called from the rendering thread
 */
FORCEINLINE bool IsInRenderingThread()
{
	return ( GRenderingThreadId == 0 ) || ( appGetCurrentThreadId() == GRenderingThreadId );
}

/**
 * @ingroup Engine
 * @brief The rendering thread runnable object
 */
class FRenderingThread : public FRunnable
{
public:
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
};

/** 
 * @ingroup Engine
 * Starts the rendering thread
 */
extern void StartRenderingThread();

/** 
 * @ingroup Engine
 * Stops the rendering thread
 */
extern void StopRenderingThread();

#endif // !RENDERINGTHREAD_H
