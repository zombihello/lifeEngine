/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef FULLSCREENMOVIETHEORA_H
#define FULLSCREENMOVIETHEORA_H

#include "LEBuild.h"

#if USE_THEORA_CODEC
#include <vector>
#include <ogg/ogg.h>
#include <theora/theora.h>
#include <theora/theoradec.h>

#include "System/ThreadingBase.h"
#include "System/FullScreenMovie.h"
#include "Render/Viewport.h"
#include "RHI/BaseSurfaceRHI.h"
#include "RHI/TypesRHI.h"

/**
 * @ingroup Engine
 * @brief Rendering support for Theora movies
 */
class CTheoraMovieRenderClient : public CViewportClient
{
public:
	/**
	 * @brief Constructor
	 */
	CTheoraMovieRenderClient();

	/**
	 * @brief Destructor
	 */
	virtual ~CTheoraMovieRenderClient();

	/**
	 * @brief Initialize internal rendering structures for a particular movie
	 *
	 * @param InWidth		Frame width
	 * @param InHeight		Frame height
	 * @return Return TRUE if successful
	 */
	void MovieInitRendering( uint32 InWidth, uint32 InHeight );

	/**
	 * @brief Teardown internal rendering structures for a particular movie
	 */
	void MovieCleanupRendering();

	/**
	 * @brief Copy data frame to texture
	 * 
	 * @param InYUVBuffer	YUV Buffer
	 */
	void CopyFrameToTexture( yuv_buffer* InYUVBuffer );

	/**
	 * @brief Get viewport
	 * @return Return viewport. If not exist returning NULL
	 */
	FORCEINLINE CViewport* GetViewport() const
	{
		return viewport;
	}

private:
	/**
	 * @brief Create the viewport for rendering the movies to
	 */
	void CreateViewport();

	/**
	 * @brief Destroy the viewport that was created
	 */
	void DestroyViewport();

	/**
	 * @brief Draw viewport
	 * @param InViewport	Viewport
	 */
	virtual void Draw( CViewport* InViewport ) override;

	CViewport*			viewport;					/**< Viewport */
	CViewportClient*	originalViewportClient;		/**< Original viewport client */
	Texture2DRHIRef_t	textureFrame;				/**< Texture of the frame */
};

 /**
  * @ingroup Engine
  * @brief Theora movie implementation
  */
class CFullScreenMovieTheora : public CFullScreenMovieSupport
{
public:
	/**
	 * @brief Constructor
	 */
	CFullScreenMovieTheora();

	/**
	 * @brief Destructor
	 */
	virtual ~CFullScreenMovieTheora();

	/**
	 * @brief Tick object (must be called from rendering thread)
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

private:
	/**
	 * @brief Kicks off the playback of a movie (must be called in rendering thread)
	 *
	 * @param InMovieFilename			Path to the movie file that will be played
	 * @return Return TRUE if successful
	 */
	bool PlayMovie( const std::wstring& InMovieFilename );

	/**
	 * @brief Stop movie (must be called in rendering thread)
	 *
	 * @param InIsForce		Whether we should force the movie to stop, even if it's not the last one in the sequence
	 * @return Return TRUE if successful
	 */
	bool StopMovie( bool InIsForce = false );

	/**
	 * @brief Opens a Theora movie with streaming from disk
	 *
	 * @param InMovieFilename		Path to the movie file that will be played
	 * @return TRUE if the movie was initialized properly, FALSE otherwise
	 */
	bool OpenStreamedMovie( const std::wstring& InMovieFilename );

	/**
	 * @brief Grab buffer some data from file into Ogg stream
	 * @return Return count readed bytes
	 */
	uint32 GrabBufferData();

	/**
	 * @brief Perform per-frame processing, including rendering to the screen
	 * @param InDeltaTime		Delta time
	 */
	void PumpMovie( float InDeltaTime );

	/**
	 * @brief Decodes current frame from Theora to YUV 
	 */
	void DecodeVideoFrame();

	bool							bStopped;				/**< Is stopped movie */
	std::wstring					gameThreadMovieName;	/**< Game thread's copy of the movie name */
	ogg_sync_state					oggSyncState;			/**< Ogg sync state */
	ogg_page						oggPage;				/**< Ogg page */
	ogg_packet						oggPacket;				/**< Ogg packet */
	ogg_stream_state				videoStream;			/**< Ogg video stream */
	theora_info						theoraInfo;				/**< Theora info */
	theora_state					theoraState;			/**< Theora state */
	theora_comment					theoraComment;			/**< Theora comment */
	yuv_buffer						yuvFrame;				/**< YUV frame */
	uint32							frameWidth;				/**< Frame width */
	uint32							frameHeight;			/**< Frame height */
	float							framesPerSecond;		/**< Frames per second */
	float							videoTimer;				/**< Video timer */
	uint32							lastVideoFrame;			/**< Last video frame */
	class CArchive*					arMovie;				/**< File of movie */
	CEvent*							movieFinishEvent;		/**< Synchronization object for game to wait for movie to finish */
	CTheoraMovieRenderClient*		theoraRender;			/**< Theora rendering */
	std::vector<std::wstring>		startupMovies;			/**< Startup movies */
};
#endif // USE_THEORA_CODEC

#endif // !FULLSCREENMOVIEFALLBACK_H
