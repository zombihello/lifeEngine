#include "System/Threading.h"

/*
==================
CRunnableThread::CRunnableThread
==================
*/
CRunnableThread::CRunnableThread()
	: exitCode( 0 )
	, threadID( ( uint32 )-1 )
	, runnable( nullptr )
	, threadPriority( TP_Normal )
{}

/*
==================
CRunnableThread::Create
==================
*/
CRunnableThread* CRunnableThread::Create( CRunnable* InRunnable, const tchar* InThreadName, bool InIsAutoDeleteSelf /* = false */, bool InIsAutoDeleteRunnable /* = false */, uint32 InStackSize /* = 0 */, EThreadPriority InThreadPriority /* = TP_Normal */ )
{
	CRunnableThread*	runnableThread = new CPlatformRunnableThread();
	if ( runnableThread->CreateInternal( InRunnable, InThreadName, InIsAutoDeleteSelf, InIsAutoDeleteRunnable, InStackSize, InThreadPriority ) )
	{
		return runnableThread;
	}

	Assert( false );
	delete runnableThread;
	return nullptr;
}