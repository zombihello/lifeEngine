#include "Misc/EngineGlobals.h"
#include "RHI/BaseRHI.h"
#include "Logger/BaseLogger.h"
#include "Logger/LoggerMacros.h"
#include "Render/RenderingThread.h"
#include "System/TickableObject.h"

//
// Definitions
//

/* The size of the rendering command buffer, in bytes. */
#define RENDERING_COMMAND_BUFFER_SIZE			( 1024 * 1024 )

//
// Globals
//

/* Whether the renderer is currently running in a separate thread */
bool			g_IsThreadedRendering = false;

/* ID of rendering thread */
uint32			g_RenderingThreadId = 0;

/* The rendering command queue */
CRingBuffer		g_RenderCommandBuffer( RENDERING_COMMAND_BUFFER_SIZE, 16 );

/* Event of finished rendering frame */
CEvent*			g_RenderFrameFinished = nullptr;

/*
==================
TickRenderingTickables
==================
*/
void TickRenderingTickables()
{
	static double		lastTickTime = Sys_Seconds();

	// Calc how long has passed since last tick
	double		currentTime		= Sys_Seconds();
	float		deltaTime		= currentTime - lastTickTime;

	// Tick any rendering thread tickables
	for ( uint32 index = 0, count = CTickableObject::renderingThreadTickableObjects.size(); index < count; ++index )
	{
		CTickableObject*		tickableObject = CTickableObject::renderingThreadTickableObjects[ index ];

		// Make sure it wants to be ticked and the rendering thread isn't suspended
		if ( tickableObject->IsTickable() )
		{
			tickableObject->Tick( deltaTime );
		}
	}

	// Update the last time we ticked
	lastTickTime = currentTime;
}

/*
==================
CSkipRenderCommand::CSkipRenderCommand
==================
*/
CSkipRenderCommand::CSkipRenderCommand( uint32 InNumSkipBytes ) :
	numSkipBytes( InNumSkipBytes )
{}

/*
==================
CSkipRenderCommand::Execute
==================
*/
uint32 CSkipRenderCommand::Execute()
{
	return numSkipBytes;
}

/*
==================
CSkipRenderCommand::GetSize
==================
*/
uint32 CSkipRenderCommand::GetSize() const
{
	return numSkipBytes;
}

/*
==================
CSkipRenderCommand::DescribeCommand
==================
*/
const tchar* CSkipRenderCommand::DescribeCommand() const
{
	return TEXT( "CSkipRenderCommand" );
}

/*
==================
CSkipRenderCommand::DescribeCommandChar
==================
*/
const char* CSkipRenderCommand::DescribeCommandChar() const
{
	return "CSkipRenderCommand";
}


/*
==================
CRenderingThread::Init
==================
*/
bool CRenderingThread::Init()
{
	// Acquire rendering context ownership on the current thread
	g_RHI->AcquireThreadOwnership();
	g_RenderingThreadId = Sys_GetCurrentThreadId();

	return true;
}

/*
==================
CRenderingThread::Run
==================
*/
uint32 CRenderingThread::Run()
{
	void*		readPointer = nullptr;
	uint32		numReadBytes = 0;

	while ( g_IsThreadedRendering )
	{	
		// Command processing loop
		while ( g_IsThreadedRendering && g_RenderCommandBuffer.BeginRead( readPointer, numReadBytes ) )
		{
			// Process one render command
			{
				// Execute the Render Command
				CRenderCommand*		command = ( CRenderCommand* )readPointer;
				{			
					uint32		commandSize = command->Execute();
					command->~CRenderCommand();
					g_RenderCommandBuffer.FinishRead( commandSize );
				}
			}
		}

		// Tick tickable objects
		TickRenderingTickables();
	}

	return 0;
}

/*
==================
CRenderingThread::Stop
==================
*/
void CRenderingThread::Stop()
{}

/*
==================
CRenderingThread::Exit
==================
*/
void CRenderingThread::Exit()
{
	// Release rendering context ownership on the current thread
	g_RHI->ReleaseThreadOwnership();
	g_RenderingThreadId = 0;
}

/** Thread used for rendering */
static CRunnableThread*		s_RenderingThread = nullptr;
static CRunnable*			s_RenderingThreadRunnable = nullptr;

/*
==================
StartRenderingThread
==================
*/
void StartRenderingThread()
{
	if ( !g_IsThreadedRendering )
	{
		// Turn on the threaded rendering flag.
		g_IsThreadedRendering = true;

		// Create the rendering thread.
		s_RenderingThreadRunnable = new CRenderingThread();

		// Release rendering context ownership on the current thread
		g_RHI->ReleaseThreadOwnership();

		const uint32		stackSize = 0;
		s_RenderingThread = g_ThreadFactory->CreateThread( s_RenderingThreadRunnable, TEXT( "RenderingThread" ), 0, 0, stackSize, TP_Realtime );
		g_RenderFrameFinished = g_SynchronizeFactory->CreateSynchEvent( false, TEXT( "RenderFrameFinished" ) );
		Assert( s_RenderingThread && g_RenderFrameFinished );
	}
}

/*
==================
StopRenderingThread
==================
*/
void StopRenderingThread()
{
	// This function is not thread-safe. Ensure it is only called by the main game thread.
	Assert( IsInGameThread() );

	static bool			GIsRenderThreadStopping = false;
	if ( g_IsThreadedRendering && !GIsRenderThreadStopping )
	{
		GIsRenderThreadStopping = true;

		// The rendering thread may have already been stopped
		if ( g_IsThreadedRendering )
		{
			Assert( s_RenderingThread );

			// Turn off the threaded rendering flag.
			g_IsThreadedRendering = false;

			//Reset the rendering thread id
			g_RenderingThreadId = 0;

			// Wait for the rendering thread to return.
			s_RenderingThread->WaitForCompletion();

			// We must kill the thread here, so that it correctly frees up the rendering thread handle
			// without this we get thread leaks when the device is lost TTP 14738, TTP 22274
			s_RenderingThread->Kill();

			// Destroy the rendering thread objects.
			g_ThreadFactory->Destroy( s_RenderingThread );
			g_SynchronizeFactory->Destroy( g_RenderFrameFinished );
			delete s_RenderingThreadRunnable;

			s_RenderingThread = nullptr;
			s_RenderingThreadRunnable = nullptr;
			g_RenderFrameFinished = nullptr;

			// Acquire rendering context ownership on the current thread
			g_RHI->AcquireThreadOwnership();
		}
	}

	GIsRenderThreadStopping = false;
}