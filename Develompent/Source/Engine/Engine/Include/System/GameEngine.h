/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include "System/BaseEngine.h"
#include "Render/Viewport.h"
#include "Render/GameViewportClient.h"

/**
 * @ingroup Engine
 * Class of game engine
 */
class CGameEngine : public CBaseEngine
{
	DECLARE_CLASS_INTRINSIC( CGameEngine, CBaseEngine, 0, 0, TEXT( "Engine" ) )

public:
	/**
	 * Constructor
	 */
	CGameEngine();

	/**
	 * Initialize engine
	 */
	virtual void Init() override;

	/**
	 * Update logic of engine
	 *
	 * @param[in] InDeltaTime Delta time
	 */
	virtual void Tick( float InDeltaSeconds ) override;

	/**
	 * Shutdown engine
	 */
	virtual void Shutdown();

	/**
	 * Load map
	 *
	 * @param[in] InMap Path to map
	 * @param[out] OutError Error of loading map
	 * @return Return true if map loaded successed, else return false
	 */
	virtual bool LoadMap( const std::wstring& InMap, std::wstring& OutError ) override;

	/**
	 * @brief Process event
	 *
	 * @param[in] InWindowEvent Window event
	 */
	virtual void ProcessEvent( struct WindowEvent& InWindowEvent ) override;

	/**
	 * @brief Get viewport
	 * @return Viewport
	 */
	FORCEINLINE const CViewport& GetViewport() const
	{
		return viewport;
	}

	/**
	 * @brief Get viewport client
	 * @return Return viewport client
	 */
	FORCEINLINE const CGameViewportClient& GetViewportClient() const
	{
		return viewportClient;
	}

private:
	CViewport				viewport;			/**< Viewport */
	CGameViewportClient		viewportClient;		/**< Viewport client */
};

#endif // !GAMEENGINE_H