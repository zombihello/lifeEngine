#include "Misc/EngineGlobals.h"
#include "RHI/BaseRHI.h"
#include "Logger/BaseLogger.h"
#include "Logger/LoggerMacros.h"
#include "Render/RenderingThread.h"

//
// Definitions
//

/* The size of the rendering command buffer, in bytes. */
#define RENDERING_COMMAND_BUFFER_SIZE			( 1024 * 1024 )

//
// Globals
//

/* Whether the renderer is currently running in a separate thread */
bool			GIsThreadedRendering = false;

/* ID of rendering thread */
uint32			GRenderingThreadId = 0;

/* The rendering command queue */
FRingBuffer		GRenderCommandBuffer( RENDERING_COMMAND_BUFFER_SIZE, 16 );

/* Event of finished rendering frame */
FEvent*			GRenderFrameFinished = nullptr;

FSkipRenderCommand::FSkipRenderCommand( uint32 InNumSkipBytes ) :
	numSkipBytes( InNumSkipBytes )
{}

uint32 FSkipRenderCommand::Execute()
{
	return numSkipBytes;
}

uint32 FSkipRenderCommand::GetSize() const
{
	return numSkipBytes;
}

const tchar* FSkipRenderCommand::DescribeCommand() const
{
	return TEXT( "FSkipRenderCommand" );
}

const char* FSkipRenderCommand::DescribeCommandChar() const
{
	return "FSkipRenderCommand";
}

bool FRenderingThread::Init()
{
	// Acquire rendering context ownership on the current thread
	GRHI->AcquireThreadOwnership();
	GRenderingThreadId = appGetCurrentThreadId();

	return true;
}

uint32 FRenderingThread::Run()
{
	void*		readPointer = nullptr;
	uint32		numReadBytes = 0;

	while ( GIsThreadedRendering )
	{	
		// Command processing loop
		while ( GIsThreadedRendering && GRenderCommandBuffer.BeginRead( readPointer, numReadBytes ) )
		{
			// Process one render command
			{
				// Execute the Render Command
				FRenderCommand*		command = ( FRenderCommand* )readPointer;
				{			
					uint32		commandSize = command->Execute();
					command->~FRenderCommand();
					GRenderCommandBuffer.FinishRead( commandSize );
				}
			}
		}

		// Trigger event of end rendering frame
		if ( GRenderFrameFinished )
		{
			GRenderFrameFinished->Trigger();
		}
	}

	return 0;
}

void FRenderingThread::Stop()
{}

void FRenderingThread::Exit()
{
	// Release rendering context ownership on the current thread
	GRHI->ReleaseThreadOwnership();
	GRenderingThreadId = 0;
}

/** Thread used for rendering */
FRunnableThread*	GRenderingThread = nullptr;
FRunnable*			GRenderingThreadRunnable = nullptr;

void StartRenderingThread()
{
	if ( GAllowRenderThread && !GIsThreadedRendering )
	{
		// Turn on the threaded rendering flag.
		GIsThreadedRendering = true;

		// Create the rendering thread.
		GRenderingThreadRunnable = new FRenderingThread();

		// Release rendering context ownership on the current thread
		GRHI->ReleaseThreadOwnership();

		const uint32		stackSize = 0;
		GRenderingThread = GThreadFactory->CreateThread( GRenderingThreadRunnable, TEXT( "RenderingThread" ), 0, 0, stackSize, TP_Realtime );
		GRenderFrameFinished = GSynchronizeFactory->CreateSynchEvent( false, TEXT( "RenderFrameFinished" ) );
		check( GRenderingThread && GRenderFrameFinished );
	}
}

void StopRenderingThread()
{
	// This function is not thread-safe. Ensure it is only called by the main game thread.
	check( IsInGameThread() );

	static bool			GIsRenderThreadStopping = false;
	if ( GIsThreadedRendering && !GIsRenderThreadStopping )
	{
		GIsRenderThreadStopping = true;

		// The rendering thread may have already been stopped
		if ( GIsThreadedRendering )
		{
			check( GRenderingThread );

			// Turn off the threaded rendering flag.
			GIsThreadedRendering = false;

			//Reset the rendering thread id
			GRenderingThreadId = 0;

			// Wait for the rendering thread to return.
			GRenderingThread->WaitForCompletion();

			// We must kill the thread here, so that it correctly frees up the rendering thread handle
			// without this we get thread leaks when the device is lost TTP 14738, TTP 22274
			GRenderingThread->Kill();

			// Destroy the rendering thread objects.
			GThreadFactory->Destroy( GRenderingThread );
			GSynchronizeFactory->Destroy( GRenderFrameFinished );
			delete GRenderingThreadRunnable;

			GRenderingThread = nullptr;
			GRenderingThreadRunnable = nullptr;
			GRenderFrameFinished = nullptr;

			// Acquire rendering context ownership on the current thread
			GRHI->AcquireThreadOwnership();
		}
	}

	GIsRenderThreadStopping = false;
}