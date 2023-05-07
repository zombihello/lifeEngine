#include "System/FullScreenMovieFallback.h"

/*
==================
CFullScreenMovieFallback::~CFullScreenMovieFallback
==================
*/
CFullScreenMovieFallback::~CFullScreenMovieFallback()
{}

/*
==================
CFullScreenMovieFallback::Tick
==================
*/
void CFullScreenMovieFallback::Tick( float InDeltaTime )
{}

/*
==================
CFullScreenMovieFallback::IsTickable
==================
*/
bool CFullScreenMovieFallback::IsTickable() const
{
	return false;
}

/*
==================
CFullScreenMovieFallback::GameThreadPlayMovie
==================
*/
void CFullScreenMovieFallback::GameThreadPlayMovie( const std::wstring& InMovieFilename, bool InIsSkippable /*= false*/, uint32 InStartFrame /*= 0*/ )
{}

/*
==================
CFullScreenMovieFallback::GameThreadStopMovie
==================
*/
void CFullScreenMovieFallback::GameThreadStopMovie( bool InIsWaitForMovie /*= true*/, bool InIsForceStop /*= false*/ )
{}

/*
==================
CFullScreenMovieFallback::GameThreadPauseMovie
==================
*/
void CFullScreenMovieFallback::GameThreadPauseMovie( bool InPause )
{}

/*
==================
CFullScreenMovieFallback::GameThreadIsMoviePaused
==================
*/
bool CFullScreenMovieFallback::GameThreadIsMoviePaused() const
{
	return false;
}

/*
==================
CFullScreenMovieFallback::GameThreadWaitForMovie
==================
*/
void CFullScreenMovieFallback::GameThreadWaitForMovie()
{}

/*
==================
CFullScreenMovieFallback::GameThreadInitiateStartupSequence
==================
*/
void CFullScreenMovieFallback::GameThreadInitiateStartupSequence()
{}

/*
==================
CFullScreenMovieFallback::GameThreadSetSkippable
==================
*/
void CFullScreenMovieFallback::GameThreadSetSkippable( bool InIsSkippable )
{}

/*
==================
CFullScreenMovieFallback::GameThreadWasSkipped
==================
*/
bool CFullScreenMovieFallback::GameThreadWasSkipped() const
{
	return true;
}