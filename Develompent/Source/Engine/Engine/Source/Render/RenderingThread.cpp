#include "Misc/EngineGlobals.h"
#include "RHI/BaseRHI.h"
#include "Logger/BaseLogger.h"
#include "Logger/LoggerMacros.h"
#include "Render/RenderingThread.h"
#include "System/TickableObject.h"

//
// Definitions
//

/* The size of the rendering command buffer, in bytes */
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
CEvent*	g_RenderFrameFinished = nullptr;

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

		// Create a render thread
		s_RenderingThread = CRunnableThread::Create( s_RenderingThreadRunnable, TEXT( "RenderingThread" ), true, true, 0, TP_Normal );
		Assert( s_RenderingThread );

		// Create a synchronize mechanism for FlushRenderingCommands()
		g_RenderFrameFinished = new CEvent( false, TEXT( "RenderFrameFinished" ) );
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

	static bool			s_IsRenderThreadStopping = false;
	if ( g_IsThreadedRendering && !s_IsRenderThreadStopping )
	{
		s_IsRenderThreadStopping = true;

		// The rendering thread may have already been stopped
		if ( g_IsThreadedRendering )
		{
			// Rendering thread must be valid
			Assert( s_RenderingThread );

			// Turn off the threaded rendering flag
			g_IsThreadedRendering = false;

			//Reset the rendering thread id
			g_RenderingThreadId = 0;

			// Wait for the rendering thread to return
			s_RenderingThread->WaitForCompletion();

			// Destroy the rendering thread objects
			delete g_RenderFrameFinished;
			s_RenderingThread			= nullptr;
			s_RenderingThreadRunnable	= nullptr;
			g_RenderFrameFinished		= nullptr;

			// Acquire rendering context ownership on the current thread
			g_RHI->AcquireThreadOwnership();
		}
	}

	s_IsRenderThreadStopping = false;
}