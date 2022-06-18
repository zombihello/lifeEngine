#include "System/FullScreenMovieFallback.h"

CFullScreenMovieFallback::~CFullScreenMovieFallback()
{}

void CFullScreenMovieFallback::Tick( float InDeltaTime )
{}

bool CFullScreenMovieFallback::IsTickable() const
{
	return false;
}

void CFullScreenMovieFallback::GameThreadPlayMovie( const std::wstring& InMovieFilename, bool InIsSkippable /*= false*/, uint32 InStartFrame /*= 0*/ )
{}

void CFullScreenMovieFallback::GameThreadStopMovie( bool InIsWaitForMovie /*= true*/, bool InIsForceStop /*= false*/ )
{}

void CFullScreenMovieFallback::GameThreadWaitForMovie()
{}

void CFullScreenMovieFallback::GameThreadInitiateStartupSequence()
{}

void CFullScreenMovieFallback::GameThreadSetSkippable( bool InIsSkippable )
{}

bool CFullScreenMovieFallback::GameThreadWasSkipped() const
{
	return true;
}