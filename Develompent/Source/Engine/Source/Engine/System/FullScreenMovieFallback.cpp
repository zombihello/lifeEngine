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

#include "Engine/System/FullScreenMovieFallback.h"

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