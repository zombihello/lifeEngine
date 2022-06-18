#include "System/FullScreenMovieFallback.h"

CFullScreenMovieFallback::~CFullScreenMovieFallback()
{}

void CFullScreenMovieFallback::Tick( float InDeltaTime )
{}

bool CFullScreenMovieFallback::IsTickable() const
{
	return false;
}

void CFullScreenMovieFallback::GameThreadPlayMovie( const std::wstring& InMovieFilename, uint32 InStartFrame /*= 0*/ )
{}

void CFullScreenMovieFallback::GameThreadStopMovie( float InDelayInSeconds /*= 0.f*/, bool InIsWaitForMovie /*= true*/, bool InIsForceStop /*= false*/ )
{}

void CFullScreenMovieFallback::GameThreadWaitForMovie()
{}

void CFullScreenMovieFallback::GameThreadInitiateStartupSequence()
{}