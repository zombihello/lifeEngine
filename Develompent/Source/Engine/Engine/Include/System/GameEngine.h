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
	DECLARE_CLASS( CGameEngine, CBaseEngine )

public:
	/**
	 * Constructor
	 */
	CGameEngine();

	/**
	 * Destructor
	 */
	virtual ~CGameEngine();

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
	 * @brief Process event
	 *
	 * @param[in] InWindowEvent Window event
	 */
	virtual void ProcessEvent( struct SWindowEvent& InWindowEvent ) override;

private:
	CViewport				viewport;			/**< Viewport */
	CGameViewportClient		viewportClient;		/**< Viewport client */
};

#endif // !GAMEENGINE_H