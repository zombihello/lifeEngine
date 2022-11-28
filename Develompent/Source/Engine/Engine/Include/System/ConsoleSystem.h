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
	 * @brief Register variable
	 * @param InConVar		Pointer to console variable
	 */
	FORCEINLINE static void RegisterVar( CConVar* InConVar )
	{
		check( InConVar );
		if ( !FindVar( InConVar->GetName() ) )
		{
			GetConVars().push_back( InConVar );
		}
	}

	/**
	 * @brief Register command
	 * @param InConCmd		Pointer to console command
	 */
	FORCEINLINE static void RegisterCmd( CConCmd* InConCmd )
	{
		check( InConCmd );
		if ( !FindCmd( InConCmd->GetName() ) )
		{
			GetConCmds().push_back( InConCmd );
		}
	}

	/**
	 * @brief Unregister variable
	 * @param InConVar		Pointer to console variable
	 */
	static void UnRegisterVar( CConVar* InConVar );

	/**
	 * @brief Unregister command
	 * @param InConCmd		Pointer to console command
	 */
	static void UnRegisterCmd( CConCmd* InConCmd );

	/**
	 * @brief Execute command
	 * 
	 * @param InCommand		Command
	 * @return Return TRUE if command is successfully executed, otherwise will returning FALSE 
	 */
	static bool Exec( const std::wstring& InCommand );

	/**
	 * @brief Find console variable
	 * 
	 * @param InName	Name of the console variable
	 * @return Return pointer to console variable is exist, otherwise will return NULL
	 */
	static CConVar* FindVar( const std::wstring& InName );

	/**
	 * @brief Find console command
	 *
	 * @param InName	Name of the console command
	 * @return Return pointer to console command is exist, otherwise will return NULL
	 */
	static CConCmd* FindCmd( const std::wstring& InName );

	/**
	 * @brief Command 'Help'
	 * 
	 * @param InArguments		Command arguments
	 */
	static void CmdHelp( const std::vector<std::wstring>& InArguments );

private:
	/**
	 * @brief Get console variables
	 * @return Return array of console variables
	 */
	FORCEINLINE static std::vector<CConVar*>& GetConVars()
	{
		static std::vector<CConVar*>	vars;
		return vars;
	}

	/**
	 * @brief Get console commands
	 * @return Return array of console commands
	 */
	FORCEINLINE static std::vector<CConCmd*>& GetConCmds()
	{
		static std::vector<CConCmd*>	cmds;
		return cmds;
	}
};

#endif // !CONSOLESYSTEM_H