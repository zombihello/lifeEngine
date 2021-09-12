/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef WORLDED_H
#define WORLDED_H

#include "Widgets/MainMenuBar.h"

/**
 * @ingroup WorldEd
 * @brief Main class for work world editor
 */
class FWorldEd
{
public:
	/**
	 * Constructor
	 */
	FWorldEd();

	/**
	 * Destructor
	 */
	virtual ~FWorldEd();

	/**
	 * Initialize editor
	 */
	virtual void Init();

	/**
	 * Process event
	 *
	 * @param[in] InWindowEvent Window event
	 */
	virtual void ProcessEvent( struct SWindowEvent& InWindowEvent );

	/**
	 * Update logic editor
	 */
	virtual void Tick();

	/**
	 * Shutdown editor
	 */
	virtual void Shutdown();

protected:
	/**
	 * Initialize user interface of editor
	 */
	virtual void InitUI();

private:
	WMainMenuBar				mainMenuBar;		/**< Widget of main menu bar */
};

#endif // !WORLDED_H