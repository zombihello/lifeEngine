/**
 * @file
 * @addtogroup Launch Launch
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef ENGINELOOP_H
#define ENGINELOOP_H

#include "Misc/Types.h"

/**
 * @ingroup Launch
 * @brief Class for initialize and tick engine
 */
class FEngineLoop
{
public:
	/**
	 * @brief Constructor
	 */
						FEngineLoop();

	/**
	 * @brief Destructor
	 */
	virtual				~FEngineLoop();

	/**
	 * @brief Pre-Initialize the main loop
	 * 
	 * @param[in] InCmdLine Command line
	 * @return Returns the error level, 0 if successful and > 0 if there were errors
	 */
	int32				PreInit( const tchar* InCmdLine );

	/**
	 * @brief Initialize the main loop
	 * 
	 * @param[in] InCmdLine Command line
	 * @return Returns the error level, 0 if successful and > 0 if there were errors
	 */
	int32				Init( const tchar* InCmdLine );

	/**
	 * @brief Advances main loop 
	 */
	void				Tick();

	/**
	 * @brief Performs shut down 
	 */
	void				Exit();

	/**
	 * @brief Process event
	 *
	 * @param[in] InWindowEvent Window event
	 */
	void				ProcessEvent( struct SWindowEvent& InWindowEvent );

private:
	/**
	 * @brief Serialize configs
	 */
	void				SerializeConfigs();

	bool				isInitialize;		/**< Is initialized engine */
	bool				bIsFocus;			/**< Is focus on window */
};

#endif // !ENGINELOOP_H
