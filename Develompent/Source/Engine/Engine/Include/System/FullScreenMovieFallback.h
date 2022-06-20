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
