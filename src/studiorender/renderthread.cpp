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

#include "pch_studiorender.h"
#include "studiorender/istudiorendercmd.h"
#include "studiorender/renderthread.h"

/**
 * @ingroup studiorender
 * @brief The size of the render command buffer, in bytes
 */
#define RENDER_COMMAND_BUFFER_SIZE			( 1024 * 1024 )

/**
 * @ingroup studiorender
 * @brief Whether the renderer is currently running in a separate thread
 */
static bool		s_bIsThreadedRendering = false;

/**
 * @ingroup studiorender
 * @brief The render command buffer
 */
CStudioCmdBuffer		g_StudioCmdBuffer( RENDER_COMMAND_BUFFER_SIZE, 16 );

/**
 * @ingroup studiorender
 * @brief The render thread handle
 */
threadHandle_t			g_RenderThreadHandle = INVALID_THREAD_HANDLE;


/**
 * @ingroup studiorender
 * @brief The render thread
 */
class CRenderThread : public CThread
{
protected:
	/**
	 * @brief Initialize a thread
	 * @note Called in the context of the aggregating thread
	 * @note By default is empty and always returns TRUE
	 * 
	 * Allows per thread initialization
	 *
	 * @return Return TRUE if initialization was successful, FALSE otherwise
	 */
	virtual bool ThreadInit() override
	{
		// Acquire rendering context ownership on the current thread
		g_pStudioAPI->AcquireThreadOwnership();

		// Remember render thread handle
		g_RenderThreadHandle = GetThreadHandle();

		// Set realtime priority and debug name for the thread
		SetPriority( THREAD_PRIOR_REALTIME );
		SetName( "Render Thread" );
		return true;
	}

	/**
	 * @brief Run a thread
	 * @note Called in the context of the aggregating thread
	 * 
	 * This is where all per thread work is done. This is only called
	 * if the initialization was successful
	 *
	 * @return Return the exit code of the thread
	 */
	virtual uint32 ThreadRun() override
	{
		void*	pReadPointer = nullptr;
		uint32	numReadBytes = 0;

		while ( s_bIsThreadedRendering )
		{	
			// Command processing loop
			while ( s_bIsThreadedRendering && g_StudioCmdBuffer.BeginRead( pReadPointer, numReadBytes ) )
			{
				// Execute the Render Command
				IStudioRenderCmd*		pCommand = ( IStudioRenderCmd* )pReadPointer;	
				uint32					commandSize = pCommand->Execute();
				pCommand->~IStudioRenderCmd();
				g_StudioCmdBuffer.EndRead( commandSize );
			}
		}
		
		return 0;
	}

	/**
	 * @brief Exit a thread
	 * @note Called in the context of the aggregating thread
	 * @note By default is empty
	 * 
	 * Called to perform any cleanup
	 */
	virtual void ThreadExit() override
	{
		// Release rendering context ownership on the current thread
		g_pStudioAPI->ReleaseThreadOwnership();

		// Reset render thread handle
		g_RenderThreadHandle = INVALID_THREAD_HANDLE;
	}
};


/** Thread used for rendering */
static CRenderThread*	s_pRenderThread = nullptr;

/*
==================
R_StartRenderThread
==================
*/
void R_StartRenderThread()
{
	if ( !s_bIsThreadedRendering )
	{
		// Turn on the threaded rendering flag
		s_bIsThreadedRendering = true;

		// Create the render thread
		s_pRenderThread = new CRenderThread();

		// Release rendering context ownership on the current thread
		g_pStudioAPI->ReleaseThreadOwnership();

		// Start thread
		const uint32	stackSize = 0;
		bool			bResult = s_pRenderThread->Start( stackSize );
		if ( !bResult )
		{
			Sys_Error( "CStudioRender: Failed to start the render thread" );
		}

		Msg( "CStudioRender: Render thread is started" );
	}
}

/*
==================
R_StopRenderThread
==================
*/
void R_StopRenderThread()
{
	// This function is not thread-safe. Ensure it is only called by the main thread
	Assert( Sys_IsInMainThread() );

	static bool		s_bIsRenderThreadStopping = false;
	if ( s_bIsThreadedRendering && !s_bIsRenderThreadStopping )
	{
		s_bIsRenderThreadStopping = true;

		// The rendering thread may have already been stopped
		if ( s_bIsThreadedRendering )
		{
			// Turn off the threaded rendering flag
			s_bIsThreadedRendering = false;

			// Reset the render thread handle
			g_RenderThreadHandle = INVALID_THREAD_HANDLE;

			// Wait for the rendering thread to return
			s_pRenderThread->WaitForCompletion();

			// We must stop the thread here, so that it correctly frees up the render thread handle
			// without this we get thread leaks when the device is lost
			s_pRenderThread->Stop();

			// Destroy the render thread object
			delete s_pRenderThread;
			s_pRenderThread = NULL;

			// Acquire rendering context ownership on the current thread
			g_pStudioAPI->AcquireThreadOwnership();
		}
	}

	Msg( "CStudioRender: Render thread is stopped" );
	s_bIsRenderThreadStopping = false;
}