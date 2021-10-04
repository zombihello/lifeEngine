/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef EDITORENGINE_H
#define EDITORENGINE_H

#include "System/BaseEngine.h"

/**
 * @ingroup Engine
 * Class of editor engine
 */
class LEditorEngine : public LBaseEngine
{
	DECLARE_CLASS( LEditorEngine, LBaseEngine )

public:
	/**
	 * Constructor
	 */
	LEditorEngine();

	/**
	 * Destructor
	 */
	virtual ~LEditorEngine();

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

#endif // !EDITORENGINE_H