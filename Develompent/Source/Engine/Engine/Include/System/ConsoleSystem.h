/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef CONSOLESYSTEM_H
#define CONSOLESYSTEM_H

#include <string>
#include <vector>

#include "System/ConCmd.h"
#include "System/ConVar.h"

/**
 * @ingroup Engine
 * @brief Console system
 */
class CConsoleSystem
{
public:
	/**
	 * @brief Execute command
	 * 
	 * @param InCommand		Command
	 * @return Return TRUE if command is successfully executed, otherwise will returning FALSE 
	 */
	bool Exec( const std::wstring& InCommand );

	/**
	 * @brief Find console variable
	 * 
	 * @param InName	Name of the console variable
	 * @return Return pointer to console variable is exist, otherwise will return NULL
	 */
	CConVar* FindVar( const std::wstring& InName );

	/**
	 * @brief Find console command
	 *
	 * @param InName	Name of the console command
	 * @return Return pointer to console command is exist, otherwise will return NULL
	 */
	CConCmd* FindCmd( const std::wstring& InName );

	/**
	 * @brief Command 'Help'
	 * 
	 * @param InArguments		Command arguments
	 */
	static void CmdHelp( const std::vector<std::wstring>& InArguments );
};

#endif // !CONSOLESYSTEM_H