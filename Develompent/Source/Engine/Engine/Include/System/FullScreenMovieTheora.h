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

#include "System/Threading.h"
#include "System/FullScreenMovie.h"
#include "System/AudioBank.h"
#include "System/AudioStreamSource.h"
#include "Render/Viewport.h"
#include "Render/Shaders/ScreenShader.h"
#include "RHI/BaseSurfaceRHI.h"
#include "RHI/TypesRHI.h"

/**
 * @ingroup Engine
 * @brief Class of pixel shader for render Theora movie
 */
class CTheoraMoviePixelShader : public CScreenPixelShader
{
	DECLARE_SHADER_TYPE( CTheoraMoviePixelShader )
};

/**
 * @ingroup Engine
 * @brief Rendering support for Theora movies
 */
class CTheoraMovieRenderClient : public CViewportClient
{
public:
	/**
	 * @brief Constructor
	 * 
	 * @param InMoviePlayer		Movie player
	 */
	CTheoraMovieRenderClient( class CFullScreenMovieTheora* InMoviePlayer );

	/**
	 * @brief Destructor
	 */
	virtual ~CTheoraMovieRenderClient();

	/**
	 * @brief Process event
	 * @param InWindowEvent			Window event
	 */
	virtual void ProcessEvent( struct WindowEvent& InWindowEvent ) override;

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

	class CFullScreenMovieTheora*		moviePlayer;				/**< Movie player */
	CViewport*							viewport;					/**< Viewport */
	CViewportClient*					originalViewportClient;		/**< Original viewport client */
	Texture2DRHIRef_t					textureFrame;				/**< Texture of the frame */
};

 /**
  * @ingroup Engine
  * @brief Theora movie implementation
  */
class CFullScreenMovieTheora : public CFullScreenMovieSupport
{
public:
	friend CTheoraMovieRenderClient;

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

private:
	/**
	 * @brief Kicks off the playback of a movie (must be called in rendering thread)
	 *
	 * @param InMovieFilename	Path to the movie file that will be played
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
	* @brief Pause or resume the movie
	* @param InPause	TRUE to pause, FALSE to resume playback
	*/
	void PauseMovie( bool InPause );

	/**
	 * @brief Stops the movie if it is skippable
	 */
	void SkipMovie();

	/**
	 * @brief Opens a Theora movie with streaming from disk
	 *
	 * @param InMovieFilename		Path to the movie file that will be played
	 * @return TRUE if the movie was initialized properly, FALSE otherwise
	 */
	bool OpenStreamedMovie( const std::wstring& InMovieFilename );

	/**
	 * @brief Close streamed movie
	 */
	void CloseStreamedMovie();

	/**
	 * @brief Grab buffer some data from file into Ogg stream
	 * @return Return count readed bytes
	 */
	uint32 GrabBufferData();

	/**
	 * @brief Perform per-frame processing, including rendering to the screen
	 * @return Return TRUE if movie is not ended, else returning FALSE
	 */
	bool PumpMovie();

	/**
	 * @brief Decodes current frame from Theora to YUV 
	 */
	void DecodeVideoFrame();

	/**
	 * @brief Stop current movie and proceed to the next startup movie (cleaning up any memory it can)
	 * @param InIsPlayNext	Whether we should proceed to the next startup movie
	 */
	void StopCurrentAndPlayNext( bool InIsPlayNext = true );

	/**
	 * @brief Process the next step in the startup sequence
	 * @return TRUE if a new movie was kicked off, FALSE otherwise
	 */
	bool ProcessNextStartupSequence();

	bool							bStopped;				/**< Is stopped movie */
	bool							bIsMovieSkippable;		/**< Is current movie skippable */
	bool							bWasSkipped;			/**< Was skipped current movie */
	bool							bPaused;				/**< Is current movie is paused */
	std::wstring					currentMovieName;		/**< Current of the movie name */
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
	float							frameRate;				/**< Frame rate */
	double							beginPlaybackTime;		/**< Time in seconds of begin playback */
	double							lastFrameTime;			/**< Last time of frame */
	uint32							startupSequenceStep;	/**< Index of current startup movie */
	TSharedPtr<CAudioBank>			audioBank;				/**< Audio bank for streamed source */
	CAudioStreamSource*				audioStreamSource;		/**< Audio stream source */
	class CArchive*					arMovie;				/**< File of movie */
	CThreadEvent					movieFinishEvent;		/**< Synchronization object for game to wait for movie to finish */
	CTheoraMovieRenderClient*		theoraRender;			/**< Theora rendering */
	std::vector<std::wstring>		startupMovies;			/**< Startup movies */
};
#endif // USE_THEORA_CODEC

#endif // !FULLSCREENMOVIEFALLBACK_H
