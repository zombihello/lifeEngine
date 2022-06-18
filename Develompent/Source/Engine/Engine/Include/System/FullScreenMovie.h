/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef FULLSCREENMOVIE_H
#define FULLSCREENMOVIE_H

#include <string>

#include "Misc/Types.h"
#include "System/TickableObject.h"

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
