/**
 * @file
 * @addtogroup Engine Engine
 *
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

#ifndef FULLSCREENMOVIE_H
#define FULLSCREENMOVIE_H

#include <string>

#include "Core/Misc/Types.h"
#include "Engine/System/TickableObject.h"

/**
 * @ingroup Engine
 * @brief Abstract base class for full-screen movie player
 */
class CFullScreenMovieSupport : public CTickableObject
{
public:
	/**
	 * @brief Constructor
	 * @param InIsRenderingThreadObject		TRUE if this object is owned by the rendering thread
	 */
	FORCEINLINE CFullScreenMovieSupport( bool InIsRenderingThreadObject = true )
		: CTickableObject( InIsRenderingThreadObject )
	{}

	/**
	 * @brief Destructor
	 */
	virtual ~CFullScreenMovieSupport() {}

	/**
	* @brief Kick off a movie play from the game thread
	*
	* @param InMovieFilename	Path of the movie to play in its entirety
	* @param InIsSkippable		Is skippable movie
	* @param InStartFrame		Optional frame number to start on
	*/
	virtual void GameThreadPlayMovie( const std::wstring& InMovieFilename, bool InIsSkippable = false, uint32 InStartFrame = 0 ) = 0;

	/**
	* @brief Stops the currently playing movie
	*
	* @param InIsWaitForMovie		If TRUE then wait until the movie finish event triggers
	* @param InIsForceStop			If TRUE then non-skippable movies and startup movies are forced to stop
	*/
	virtual void GameThreadStopMovie( bool InIsWaitForMovie = true, bool InIsForceStop = false ) = 0;

	/**
	* @brief Pause or resume the movie
	* @param InPause		TRUE to pause, FALSE to resume playback
	*/
	virtual void GameThreadPauseMovie( bool InPause ) = 0;

	/**
	* @brief Is movie paused
	* @return Return TRUE if movie is paused, else returning FALSE
	*/
	virtual bool GameThreadIsMoviePaused() const = 0;

	/**
	* @brief Block game thread until movie is complete
	*/
	virtual void GameThreadWaitForMovie() = 0;

	/**
	* @brief Kicks off a thread to control the startup movie sequence
	*/
	virtual void GameThreadInitiateStartupSequence() = 0;

	/**
	 * @brief Set skippable current movie
	 * @param InIsSkippable		Is skippable movie
	 */
	virtual void GameThreadSetSkippable( bool InIsSkippable ) = 0;

	/**
	 * @brief Is current movie is was skipped
	 * @return Return TRUE if current movie is was skipped, else return FALSE
	 */
	virtual bool GameThreadWasSkipped() const = 0;
};

#endif // !FULLSCREENMOVIE_H
