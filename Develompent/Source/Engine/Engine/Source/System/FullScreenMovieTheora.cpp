#include "LEBuild.h"

#if USE_THEORA_CODEC
#include "Misc/CoreGlobals.h"
#include "Misc/EngineGlobals.h"
#include "Math/Math.h"
#include "Logger/LoggerMacros.h"
#include "System/Config.h"
#include "System/FullScreenMovieTheora.h"
#include "System/Archive.h"
#include "System/BaseFileSystem.h"
#include "System/GameEngine.h"
#include "System/BaseWindow.h"
#include "System/SplashScreen.h"
#include "Render/RenderingThread.h"
#include "Render/Shaders/ScreenShader.h"
#include "Render/Shaders/ShaderManager.h"
#include "Render/VertexFactory/SimpleElementVertexFactory.h"
#include "RHI/BaseRHI.h"
#include "RHI/StaticStatesRHI.h"
#include "Core.h"

/**
 * @ingroup Engine
 * @brief Class of pixel shader for render Theora movie
 */
class CTheoraMoviePixelShader : public CScreenPixelShader
{
	DECLARE_SHADER_TYPE( CTheoraMoviePixelShader )
};

IMPLEMENT_SHADER_TYPE(, CTheoraMoviePixelShader, TEXT( "TheoraPixelShader.hlsl" ), TEXT( "MainPS" ), SF_Pixel, true );

CTheoraMovieRenderClient::CTheoraMovieRenderClient()
	: viewport( nullptr )
	, originalViewportClient( nullptr )
{
	CreateViewport();
}

CTheoraMovieRenderClient::~CTheoraMovieRenderClient()
{
	MovieCleanupRendering();
	DestroyViewport();
}

void CTheoraMovieRenderClient::CreateViewport()
{
	if ( !viewport )
	{
		check( GEngine->IsA<CGameEngine>() );
		viewport				= const_cast<CViewport*>( &GEngine->Cast<CGameEngine>()->GetViewport() );
		originalViewportClient	= viewport->GetViewportClient();

		check( viewport );
		viewport->SetViewportClient( this );
	}
}

void CTheoraMovieRenderClient::DestroyViewport()
{
	if ( !viewport )
	{
		return;
	}

	viewport->SetViewportClient( originalViewportClient );

	viewport				= nullptr;
	originalViewportClient	= nullptr;
}

void CTheoraMovieRenderClient::MovieInitRendering( uint32 InWidth, uint32 InHeight )
{
	textureFrame = GRHI->CreateTexture2D( TEXT( "TheoraFrame" ), InWidth, InHeight, PF_A8R8G8B8, 1, TCF_None );
}

void CTheoraMovieRenderClient::MovieCleanupRendering()
{
	textureFrame.SafeRelease();
}

void CTheoraMovieRenderClient::CopyFrameToTexture( yuv_buffer* InYUVBuffer )
{
	check( textureFrame );
	CBaseDeviceContextRHI*		deviceContext = GRHI->GetImmediateContext();
	SLockedData					lockedData;
	GRHI->LockTexture2D( deviceContext, textureFrame, 0, true, lockedData );

	// Copy data from YUV444 buffer to texture
	for ( uint32 y = 0; y < textureFrame->GetSizeY(); ++y )
	{
		for ( uint32 x = 0; x < textureFrame->GetSizeX(); ++x )
		{
			const int off = x + y * textureFrame->GetSizeX();
			const int xx = x >> 1;
			const int yy = y >> 1;

			lockedData.data[ off * 4 + 0 ] = InYUVBuffer->y[ x + y * InYUVBuffer->y_stride ];
			lockedData.data[ off * 4 + 1 ] = InYUVBuffer->u[ xx + yy * InYUVBuffer->uv_stride ];
			lockedData.data[ off * 4 + 2 ] = InYUVBuffer->v[ xx + yy * InYUVBuffer->uv_stride ];
			lockedData.data[ off * 4 + 3 ] = 255;
		}
	}

	GRHI->UnlockTexture2D( deviceContext, textureFrame, 0, lockedData );
}

void CTheoraMovieRenderClient::Draw( CViewport* InViewport )
{
	ViewportRHIRef_t						viewportRHI				= InViewport->GetViewportRHI();
	CBaseDeviceContextRHI*					immediateContext		= GRHI->GetImmediateContext();
	CScreenVertexShader<SVST_Fullscreen>*	screenVertexShader		= GShaderManager->FindInstance< CScreenVertexShader<SVST_Fullscreen>, CSimpleElementVertexFactory >();
	CTheoraMoviePixelShader*				theoraMoviePixelShader	= GShaderManager->FindInstance< CTheoraMoviePixelShader, CSimpleElementVertexFactory >();
	check( screenVertexShader && theoraMoviePixelShader );

	GRHI->SetRenderTarget( immediateContext, viewportRHI->GetSurface(), nullptr );
	GRHI->SetDepthTest( immediateContext, TStaticDepthStateRHI<false>::GetRHI() );
	GRHI->SetRasterizerState( immediateContext, TStaticRasterizerStateRHI<>::GetRHI() );
	GRHI->SetBoundShaderState( immediateContext, GRHI->CreateBoundShaderState( TEXT( "TheoraDrawBBS" ), GSimpleElementVertexDeclaration.GetVertexDeclarationRHI(), screenVertexShader->GetVertexShader(), theoraMoviePixelShader->GetPixelShader() ) );

	theoraMoviePixelShader->SetTexture( immediateContext, textureFrame );
	theoraMoviePixelShader->SetSamplerState( immediateContext, TStaticSamplerStateRHI<>::GetRHI() );
	GRHI->DrawPrimitive( immediateContext, PT_TriangleList, 0, 1 );
}


CFullScreenMovieTheora::CFullScreenMovieTheora()
	: bStopped( true )
	, frameWidth( 0 )
	, frameHeight( 0 )
	, framesPerSecond( 0.f )
	, videoTimer( 0.f )
	, lastVideoFrame( 0 )
	, arMovie( nullptr )
	, movieFinishEvent( nullptr )
	, theoraRender( nullptr )
{
	// Get list of startup movies
	CConfigValue					confStartupMovies = GGameConfig.GetValue( TEXT( "Game.FullScreenMovie" ), TEXT( "StartupMovies" ) );
	check( confStartupMovies.IsA( CConfigValue::T_Array ) );
	std::vector<CConfigValue>		confArrayStartupMovies = confStartupMovies.GetArray();

	for ( uint32 index = 0, count = confArrayStartupMovies.size(); index < count; ++index )
	{
		const CConfigValue&		configValue = confArrayStartupMovies[ index ];
		check( configValue.IsA( CConfigValue::T_String ) );
		startupMovies.push_back( configValue.GetString() );
	}

	// Init synchronization objects (a manual reset Event)
	check( GSynchronizeFactory );
	movieFinishEvent = GSynchronizeFactory->CreateSynchEvent( true );

	// By default, we are "done" playing a movie
	movieFinishEvent->Trigger();
}

CFullScreenMovieTheora::~CFullScreenMovieTheora()
{
	GameThreadStopMovie( 0, false, true );
}

void CFullScreenMovieTheora::Tick( float InDeltaTime )
{
	// We are only tickable if we are playing a movie
	if ( arMovie )
	{
		PumpMovie( InDeltaTime );
		if ( bStopped )
		{
			StopMovie( true );
		}
	}
}

bool CFullScreenMovieTheora::IsTickable() const
{
	return arMovie && theoraRender;
}

void CFullScreenMovieTheora::PumpMovie( float InDeltaTime )
{
	if ( !bStopped )
	{
		// Advance video timer by delta time
		videoTimer += InDeltaTime;

		// Calculate current frame and decode him if need
		uint32		currentFrame = videoTimer * framesPerSecond;
		if ( currentFrame != lastVideoFrame )
		{
			lastVideoFrame = currentFrame;
			DecodeVideoFrame();
		}
	}

	// Draw frame
	CViewport*		viewport = theoraRender->GetViewport();
	if ( viewport )
	{
		viewport->Draw( true );
	}
}

void CFullScreenMovieTheora::DecodeVideoFrame()
{
	// First of all - grab some data into ogg packet
	while ( ogg_stream_packetout( &videoStream, &oggPacket ) <= 0 )
	{
		// If no data in video stream, grab some data
		if ( !GrabBufferData() )
		{
			bStopped = true;
			return;
		}

		// Grab all decoded ogg pages into our video stream
		while ( ogg_sync_pageout( &oggSyncState, &oggPage ) > 0 )
		{
			ogg_stream_pagein( &videoStream, &oggPage );
		}
	}

	// Load packet into theora decoder
	if ( !theora_decode_packetin( &theoraState, &oggPacket ) )
	{
		// If decoded ok - get YUV frame
		theora_decode_YUVout( &theoraState, &yuvFrame );

		// Copy YUV frame to texture
		theoraRender->CopyFrameToTexture( &yuvFrame );
	}
	else
	{
		bStopped = true;
	}
}

void CFullScreenMovieTheora::GameThreadPlayMovie( const std::wstring& InMovieFilename, uint32 InStartFrame /*= 0*/ )
{
	// Check for movie already playing and exit out if so
	if ( !movieFinishEvent->Wait( 0 ) )
	{
		LE_LOG( LT_Log, LC_Movie, TEXT( "Attempting to start already playing movie \"%s\"; aborting" ), gameThreadMovieName.c_str() );
		return;
	}

	// If we're going to play startup movies, hide the splashscreen and show the game window.
	// Both of these functions do nothing if called a second time
	if ( GIsGame )
	{
		appHideSplash();
		GWindow->Show();
	}

	// Remember the name of the movie we're playing
	gameThreadMovieName = InMovieFilename;

	// Deferred init for Theora renderer the first time we try playing a movie
	if ( !theoraRender )
	{
		CTheoraMovieRenderClient*		newTheoraRender = new CTheoraMovieRenderClient();
		UNIQUE_RENDER_COMMAND_TWOPARAMETER( CInitTheoraRenderCommand,
											CTheoraMovieRenderClient*, newTheoraRender, newTheoraRender,
											CFullScreenMovieTheora*, moviePlayer, this,
											{
												moviePlayer->theoraRender = newTheoraRender;
											} )
	}

	// Reset synchronization to untriggered state
	movieFinishEvent->Reset();

	// Play movie
	UNIQUE_RENDER_COMMAND_ONEPARAMETER( CPlayMovieCommand,
										CFullScreenMovieTheora*, moviePlayer, this,
										{
											moviePlayer->PlayMovie( moviePlayer->gameThreadMovieName );
										} )
}

void CFullScreenMovieTheora::GameThreadStopMovie( float InDelayInSeconds /*= 0.f*/, bool InIsWaitForMovie /*= true*/, bool InIsForceStop /*= false*/ )
{
	LE_LOG( LT_Log, LC_Movie, TEXT( "Stopping movie %s" ), gameThreadMovieName.c_str() );

	if ( !InIsForceStop )
	{
		GameThreadWaitForMovie();
	}

	// Stop movie and delete the Theora render to free up the GPU memory
	UNIQUE_RENDER_COMMAND_THREEPARAMETER( CStopMovieCommand,
										bool, isForceStop, InIsForceStop,
										CFullScreenMovieTheora*, moviePlayer, this, 
										CTheoraMovieRenderClient*, theoraRender, theoraRender,
										{
											moviePlayer->StopMovie( isForceStop );
											delete theoraRender;
										} )

	FlushRenderingCommands();
	theoraRender = nullptr;
}

void CFullScreenMovieTheora::GameThreadWaitForMovie()
{
	// Wait for the event
	bool		bIsFinished = movieFinishEvent->Wait( 1 );
	while ( !bIsFinished && !GIsRequestingExit )
	{
		// Compute the time since the last tick
		static double		lastTickTime	= appSeconds();
		const double		currentTime		= appSeconds();
		const float			deltaTime		= currentTime - lastTickTime;
		lastTickTime = currentTime;

		// Process window events and tick viewport
		appProcessWindowEvents();
		if ( theoraRender )
		{
			CViewport*		viewport = theoraRender->GetViewport();
			if ( viewport )
			{
				viewport->Tick( deltaTime );
			}
		}

		// Wait movie finish event
		bIsFinished = movieFinishEvent->Wait( 1 );
	}
}

void CFullScreenMovieTheora::GameThreadInitiateStartupSequence()
{
	if ( !startupMovies.empty() )
	{
		GameThreadPlayMovie( startupMovies[ 0 ] );
	}
}

bool CFullScreenMovieTheora::PlayMovie( const std::wstring& InMovieFilename )
{
	check( theoraRender );
	LE_LOG( LT_Log, LC_Movie, TEXT( "Playing movie [%s]" ), !InMovieFilename.empty() ? InMovieFilename.c_str() : TEXT( "None" ) );

	// Use the region name from the startup movies if available
	std::wstring		fullPath	= appGameDir() + TEXT( "Movies" ) PATH_SEPARATOR + InMovieFilename + TEXT( ".ogv" );
	
	// Open streamed movie and init rendering structures
	bool				result		= OpenStreamedMovie( fullPath );
	if ( result )
	{
		theoraRender->MovieInitRendering( frameWidth, frameHeight );
	}

	return result;
}

bool CFullScreenMovieTheora::OpenStreamedMovie( const std::wstring& InMovieFilename )
{
	// Init Ogg and Theora structs
	ogg_sync_init( &oggSyncState );
	theora_comment_init( &theoraComment );
	theora_info_init( &theoraInfo );

	// Open file of movie
	check( GFileSystem );
	arMovie = GFileSystem->CreateFileReader( InMovieFilename );
	if ( !arMovie )
	{
		return false;
	}

	bool		bDataFound = false;
	uint32		theoraPacketsFound = 0;
	
	while ( !bDataFound )
	{
		// Grab some data from file and put it into the ogg stream
		GrabBufferData();

		// Grab the ogg page from the stream
		while ( ogg_sync_pageout( &oggSyncState, &oggPage ) > 0 )
		{
			ogg_stream_state		oggStreamState;

			// If this is not headers page, then we finished
			if ( !ogg_page_bos( &oggPage ) )
			{
				// All headers pages are finished, now there are only data packets
				bDataFound = true;

				// Don't leak the page, get it into the video stream
				ogg_stream_pagein( &videoStream, &oggPage );
				break;
			}

			// We nee to identify the stream

			// 1. Init the test stream with the s/n from our page
			ogg_stream_init( &oggStreamState, ogg_page_serialno( &oggPage ) );
			
			// 2. Add this page to this test stream
			ogg_stream_pagein( &oggStreamState, &oggPage );
			
			// 3. Decode the page into the packet
			ogg_stream_packetout( &oggStreamState, &oggPacket );

			// Try to interpret the packet as Theora's data
			if ( !theoraPacketsFound && theora_decode_header( &theoraInfo, &theoraComment, &oggPacket ) >= 0 )
			{
				// Theora found ! Let's copy the stream
				memcpy( &videoStream, &oggStreamState, sizeof( oggStreamState ) );
				++theoraPacketsFound;
			}
			else
			{
				// Non-theora (vorbis maybe)
				ogg_stream_clear( &oggStreamState );
			}
		}
	}

	// No theora found, maybe this is music file ?
	if ( theoraPacketsFound )
	{
		int			error = 0;

		// By specification we need 3 header packets for any logical stream (theora, vorbis, etc.)
		while ( theoraPacketsFound < 3 )
		{
			error = ogg_stream_packetout( &videoStream, &oggPacket );
			if ( error < 0 )
			{
				// Some stream errors (maybe stream corrupted?)
				break;
			}

			if ( error > 0 )
			{
				if ( theora_decode_header( &theoraInfo, &theoraComment, &oggPacket ) >= 0 )
				{
					theoraPacketsFound++;
				}
				else
				{
					// Another stream corruption?
					break;
				}
			}

			// If read nothing from packet - just grab more data into packet
			if ( !error )
			{
				if ( ogg_sync_pageout( &oggSyncState, &oggPage ) > 0 )
				{
					// If data arrivet into packet - put it into our logical stream
					ogg_stream_pagein( &videoStream, &oggPage );
				}
				else
				{
					// Nothing goint from the ogg stream, need to read some data from file
					if ( !GrabBufferData() )
					{
						// End of file :(
						break;
					}
				}
			}
		}
	}

	// If we have theora ok
	if ( theoraPacketsFound )
	{
		// Init decoder
		if ( !theora_decode_init( &theoraState, &theoraInfo ) )
		{
			// Decoder intialization succeed
			frameWidth			= theoraInfo.frame_width;
			frameHeight			= theoraInfo.frame_height;
			framesPerSecond		= ( float )theoraInfo.fps_numerator / theoraInfo.fps_denominator;
			videoTimer			= 0.f;
			bStopped			= false;
			return true;
		}
	}

	return false;
}

bool CFullScreenMovieTheora::StopMovie( bool InIsForce /*= false*/ )
{
	// Close file of movie
	if ( arMovie )
	{
		delete arMovie;
		arMovie = nullptr;
	}

	// Cleanup rendering structures
	theoraRender->MovieCleanupRendering();

	// Trigger of finished movie playing
	movieFinishEvent->Trigger();

	return true;
}

uint32 CFullScreenMovieTheora::GrabBufferData()
{
	static const uint32			syncBufferSize = 4096;
	check( arMovie );

	// Ask some buffer for putting data into stream
	char*		buffer = ogg_sync_buffer( &oggSyncState, syncBufferSize );
	
	// Read data from file
	uint32		oldPosInFile = arMovie->Tell();
	arMovie->Serialize( buffer, syncBufferSize );
	uint32		readedBytes = arMovie->Tell() - oldPosInFile;

	// Put readed data into Ogg stream
	ogg_sync_wrote( &oggSyncState, readedBytes );
	return readedBytes;
}
#endif // USE_THEORA_CODEC