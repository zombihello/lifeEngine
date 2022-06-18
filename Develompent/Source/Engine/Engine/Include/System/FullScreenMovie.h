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
	* @param InStartFrame		Optional frame number to start on
	*/
	virtual void GameThreadPlayMovie( const std::wstring& InMovieFilename, uint32 InStartFrame = 0 ) = 0;

	/**
	* @brief Stops the currently playing movie
	*
	* @param InDelayInSeconds		Will delay the stopping of the movie for this many seconds. If zero, this function will wait until the movie stops before returning.
	* @param InIsWaitForMovie		If TRUE then wait until the movie finish event triggers
	* @param InIsForceStop			If TRUE then non-skippable movies and startup movies are forced to stop
	*/
	virtual void GameThreadStopMovie( float InDelayInSeconds = 0.f, bool InIsWaitForMovie = true, bool InIsForceStop = false ) = 0;

	/**
	* @brief Block game thread until movie is complete
	*/
	virtual void GameThreadWaitForMovie() = 0;

	/**
	* @brief Kicks off a thread to control the startup movie sequence
	*/
	virtual void GameThreadInitiateStartupSequence() = 0;
};

#endif // !FULLSCREENMOVIE_H
