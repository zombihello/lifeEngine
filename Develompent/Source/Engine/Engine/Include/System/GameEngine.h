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

/**
 * @ingroup Engine
 * Class of game engine
 */
class LGameEngine : public LBaseEngine
{
	DECLARE_CLASS( LGameEngine, LBaseEngine )

public:
	/**
	 * Constructor
	 */
	LGameEngine();

	/**
	 * Destructor
	 */
	virtual ~LGameEngine();

	/**
	 * Initialize engine
	 */
	virtual void Init();

	/**
	 * Update logic of engine
	 *
	 * @param[in] InDeltaTime Delta time
	 */
	virtual void Tick( float InDeltaSeconds );

	/**
	 * Shutdown engine
	 */
	virtual void Shutdown();
};

#endif // !GAMEENGINE_H