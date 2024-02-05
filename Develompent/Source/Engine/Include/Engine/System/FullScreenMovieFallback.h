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

#ifndef FULLSCREENMOVIEFALLBACK_H
#define FULLSCREENMOVIEFALLBACK_H

#include "Engine/System/FullScreenMovie.h"

/**
 * @ingroup Engine
 * @brief Fallback movie implementation
 */
class CFullScreenMovieFallback : public CFullScreenMovieSupport
{
public:
	/**
	 * @brief Destructor
	 */
	virtual ~CFullScreenMovieFallback();

	/**
	 * @brief Tick object
	 * @param InDeltaTime		Delta time
	 */
	virtual void Tick( float InDeltaTime ) override;

	/**
	 * @brief Is tickable object
	 * @return Return TRUE if class is ready to be ticked, FALSE otherwise
	 */
	virtual bool IsTickable() const override;

	/**
	* @brief Kick off a movie play from the game thread
	*
	* @param InMovieFilename	Path of the movie to play in its entirety
	* @param InIsSkippable		Is skippable movie
	* @param InStartFrame		Optional frame number to start on
	*/
	virtual void GameThreadPlayMovie( const std::wstring& InMovieFilename, bool InIsSkippable = false, uint32 InStartFrame = 0 ) override;

	/**
	* @brief Stops the currently playing movie
	*
	* @param InIsWaitForMovie		If TRUE then wait until the movie finish event triggers
	* @param InIsForceStop			If TRUE then non-skippable movies and startup movies are forced to stop
	*/
	virtual void GameThreadStopMovie( bool InIsWaitForMovie = true, bool InIsForceStop = false ) override;

	/**
	* @brief Pause or resume the movie
	* @param InPause		TRUE to pause, FALSE to resume playback
	*/
	virtual void GameThreadPauseMovie( bool InPause ) override;

	/**
	* @brief Is movie paused
	* @return Return TRUE if movie is paused, else returning FALSE
	*/
	virtual bool GameThreadIsMoviePaused() const override;

	/**
	* @brief Block game thread until movie is complete
	*/
	virtual void GameThreadWaitForMovie() override;

	/**
	* @brief Kicks off a thread to control the startup movie sequence
	*/
	virtual void GameThreadInitiateStartupSequence() override;

	/**
	 * @brief Set skippable current movie
	 * @param InIsSkippable		Is skippable movie
	 */
	virtual void GameThreadSetSkippable( bool InIsSkippable ) override;

	/**
	 * @brief Is current movie is was skipped
	 * @return Return TRUE if current movie is was skipped, else return FALSE
	 */
	virtual bool GameThreadWasSkipped() const override;
};

#endif // !FULLSCREENMOVIEFALLBACK_H
