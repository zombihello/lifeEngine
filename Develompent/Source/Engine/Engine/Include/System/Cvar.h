/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef CVAR_H
#define CVAR_H

#include "Misc/Platform.h"
#include "Misc/Types.h"
#include "System/ConVar.h"

/**
 * @ingroup Engine
 * @brief The console system
 */
class CCvar
{
public:
	/**
	 * @brief Constructor
	 */
	CCvar();

	/**
	 * @brief Register command
	 * @param InCommand		Console command
	 */
	void RegisterCommand( CConCmdBase* InCommand );

	/**
	 * @brief Unregister command
	 * @param InCommand		Console command
	 */
	void UnregisterCommand( CConCmdBase* InCommand );

	/**
	 * @brief Unregister all commands
	 */
	void UnregisterAllCommands();

	/**
	 * @brief Execute command
	 *
	 * @param InCommand		Command
	 * @return Return TRUE if command is successfully executed, otherwise returns FALSE
	 */
	bool Exec( const tchar* InCommand );

	/**
	 * @brief Find command base by name
	 *
	 * @param InName	Command name
	 * @return Return pointer to command base if found, otherwise returns NULL
	 */
	CConCmdBase* FindCommandBase( const tchar* InName ) const;

	/**
	 * @brief Find command by name
	 *
	 * @param InName	Command name
	 * @return Return pointer to command if found, otherwise returns NULL
	 */
	CConCmd* FindCommand( const tchar* InName ) const;

	/**
	 * @brief Find variable by name
	 *
	 * @param InName	Command name
	 * @return Return pointer to variable if found, otherwise returns NULL
	 */
	CConVar* FindVar( const tchar* InName ) const;

	/**
	 * @brief Reset cvars which contain a specific flags
	 * Sets cvars containing the flags to their default value
	 *
	 * @param InFlags	Cvar flags (see ECVarFlags)
	 */
	void ResetFlaggedVars( uint32 InFlags );

	/**
	 * @brief Get the first CConCmdBase to allow iteration over all CConCmd and CConVars
	 * @return Return the first CConCmdBase to allow iteration. If in the system isn't any commands returns NULL
	 */
	FORCEINLINE CConCmdBase* GetCommands() const
	{
		return conCmdList;
	}

private:
	enum
	{
		COMMAND_MAX_ARGC	= 64,	/**< Max count arguments in a command */
		COMMAND_MAX_LENGTH	= 512	/**< Max length of a command */
	};

	/**
	 * @brief Parse command
	 *
	 * This method parse command and result place into variables:
	 * commandArgc - Here will be count command arguments
	 * commandArgv - Here will be command arguments
	 *
	 * @param InCommand		Command to parse. After the function is executed, this pointer will point to the beginning of the next command after the separator, or NULL if there is nothing else left
	 * @param InSeparator	Symbol for splitting a command
	 * @return Return TRUE if command was successfully parsed, otherwise returns FALSE
	 */
	bool ParseCommand( const tchar*& InCommand, const tchar InSeparator = TEXT( '$' ) );

	CConCmdBase*				conCmdList;								/**< Console command list */
	tchar						commandArgvBuffer[COMMAND_MAX_LENGTH];	/**< Buffer for command arguments */
	uint32						commandArgc;							/**< Count of command arguments */
	const tchar*				commandArgv[COMMAND_MAX_ARGC];			/**< Array of pointer to begin each command argument */
};

#endif // !CVAR_H