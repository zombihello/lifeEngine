/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef FULLSCREENMOVIEFALLBACK_H
#define FULLSCREENMOVIEFALLBACK_H

#include "System/FullScreenMovie.h"

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
	* @param InStartFrame		Optional frame number to start on
	*/
	virtual void GameThreadPlayMovie( const std::wstring& InMovieFilename, uint32 InStartFrame = 0 ) override;

	/**
	* @brief Stops the currently playing movie
	*
	* @param InDelayInSeconds		Will delay the stopping of the movie for this many seconds. If zero, this function will wait until the movie stops before returning.
	* @param InIsWaitForMovie		If TRUE then wait until the movie finish event triggers
	* @param InIsForceStop			If TRUE then non-skippable movies and startup movies are forced to stop
	*/
	virtual void GameThreadStopMovie( float InDelayInSeconds = 0.f, bool InIsWaitForMovie = true, bool InIsForceStop = false ) override;

	/**
	* @brief Block game thread until movie is complete
	*/
	virtual void GameThreadWaitForMovie() override;

	/**
	* @brief Kicks off a thread to control the startup movie sequence
	*/
	virtual void GameThreadInitiateStartupSequence() override;
};

#endif // !FULLSCREENMOVIEFALLBACK_H
