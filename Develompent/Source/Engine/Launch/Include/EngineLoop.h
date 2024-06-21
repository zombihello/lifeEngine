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
#include "System/Config.h"

/**
 * @ingroup Launch
 * @brief Class for initialize and tick engine
 */
class CEngineLoop
{
public:
	/**
	 * @brief Constructor
	 */
	CEngineLoop();

	/**
	 * @brief Destructor
	 */
	virtual ~CEngineLoop();

	/**
	 * @brief Pre-Initialize the main loop
	 * 
	 * @param[in] InCmdLine Command line
	 * @return Returns the error level, 0 if successful and > 0 if there were errors
	 */
	int32 PreInit( const tchar* InCmdLine );

	/**
	 * @brief Initialize the main loop
	 * 
	 * @return Returns the error level, 0 if successful and > 0 if there were errors
	 */
	int32 Init();

	/**
	 * @brief Advances main loop 
	 */
	void Tick();

	/**
	 * @brief Performs shut down 
	 */
	void Exit();

	/**
	 * @brief Process event
	 *
	 * @param[in] InWindowEvent Window event
	 */
	void ProcessEvent( struct WindowEvent& InWindowEvent );

private:
	/**
	 * @brief Init configs
	 */
	void InitConfigs();

	/**
	 * @brief Load script packages
	 */
	void LoadScriptPackages();

	/**
	 * @brief Load script packages for specific config group
	 * 
	 * @param InType	Config type
	 * @param InGroup	Name of group in config
	 * @param InName	Name of value in config group
	 */
	void LoadScriptPackages( EConfigType InType, const std::wstring& InGroup, const std::wstring& InName );

	bool				isInitialize;		/**< Is initialized engine */
	bool				bIsFocus;			/**< Is focus on window */
};

#endif // !ENGINELOOP_H
