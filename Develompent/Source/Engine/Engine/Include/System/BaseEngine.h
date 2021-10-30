/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef BASEENGINE_H
#define BASEENGINE_H

#include "Misc/Object.h"
#include "Core.h"

/**
 * @ingroup Engine
 * Class of base engine
 */
class LBaseEngine : public LObject
{
	DECLARE_CLASS( LBaseEngine, LObject )

public:
	/**
	 * Destructor
	 */
	virtual ~LBaseEngine() {}

	/**
	 * Initialize engine
	 */
	virtual void Init();

	/**
	 * Update logic of engine
	 * 
	 * @param[in] InDeltaTime Delta time
	 */
	virtual void Tick( float InDeltaSeconds ) PURE_VIRTUAL( LBaseEngine::Tick, );

	/**
	 * Shutdown engine
	 */
	virtual void Shutdown() PURE_VIRTUAL( LBaseEngine::Shutdown, );

	/**
	 * @brief Process event
	 *
	 * @param[in] InWindowEvent Window event
	 */
	virtual void ProcessEvent( struct SWindowEvent& InWindowEvent );
};

#endif // !BASEENGINE_H