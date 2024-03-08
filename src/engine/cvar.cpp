/**
 * ************************************************************
 *                  This file is part of:
 *                      LIFEENGINE
 *          https://github.com/zombihello/lifeEngine
 * ************************************************************
 * Copyright (C) 2024 Yehor Pohuliaka.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "pch_engine.h"
#include <sstream>

#include "engine/icvar.h"
#include "filesystem/ifilesystem.h"
#include "stdlib/convar.h"

/*
==================
CheatsChanged
==================
*/
static void CheatsChanged( IConVar* pConVar )
{
	// Cheats were disabled, revert all cheat cvars to their default values
	if ( g_pCvar && pConVar->GetInt() == 0 )
	{
		g_pCvar->ResetFlaggedVars( FCVAR_CHEAT );
		Msg( "FCVAR_CHEAT cvars reverted to defaults" );
	}
}

/**
 * @ingroup engine
 * @brief Cvar allow cheats in the game
 */
CConVar		cheats( "cheats", "0", "Allow cheats in the game", FCVAR_NONE, CheatsChanged );


/**
 * @ingroup engine
 * @brief Implementation of the ICvarQuery interface
 */
class CCvarQuery : public CBaseAppSystem<ICvarQuery> 
{
public:
	/**
	 * @brief Constructor
	 */
	CCvarQuery();

	/**
	 * @brief Connect application system
	 *
	 * @param pFactory		Pointer to interface factory
	 * @return Return TRUE if successes application system is connected, otherwise return FALSE
	 */
	virtual bool Connect( CreateInterfaceFn_t pFactory ) override;

	/**
	 * @brief Disconnect application system
	 */
	virtual void Disconnect() override;

	/**
	 * @brief Query interface
	 *
	 * Here's where systems can access other interfaces implemented by this object
	 *
	 * @param pInterfaceName	Interface name
	 * @return Return pointer to interface, if doesn't implement the requested interface return NULL
	 */
	virtual void* QueryInterface( const achar* pInterfaceName ) override;

	/**
	 * @brief Can these two IConVars be aliased
	 *
	 * @param pChildVar		Child variable
	 * @param pParentVar	Parent variable
	 * @return Return TRUE if these two IConVars can be aliased, otherwise returns FALSE
	 */
	virtual bool AreConVarsLinkable( const IConVar* pChildVar, const IConVar* pParentVar ) override;

private:
	ICvar*		pCvar;		/**< Cvar query connected to this ICvar */
};

/**
 * @ingroup engine
 * @brief The console system
 */
class CCvar : public CBaseAppSystem<ICvar>
{
public:
	/**
	 * @brief Constructor
	 */
	CCvar();

	/**
	 * @brief Connect application system
	 *
	 * @param pFactory		Pointer to interface factory
	 * @return Return TRUE if successes application system is connected, otherwise return FALSE
	 */
	virtual bool Connect( CreateInterfaceFn_t pFactory ) override;

	/**
	 * @brief Disconnect application system
	 */
	virtual void Disconnect() override;

	/**
	 * @brief Init application system
	 * @return Return TRUE if application system is inited
	 */
	virtual bool Init() override;

	/**
	 * @brief Shutdown application system
	 */
	virtual void Shutdown() override;

	/**
	 * @brief Allocate a unique DLL identifier
	 * @return Return allocated a unique DLL identifier
	 */
	virtual CVarDLLIdentifier_t AllocateDLLIdentifier() override;

	/**
	 * @brief Register command
	 * @param pCommand	Console command
	 */
	virtual void RegisterCommand( IConCmdBase* pCommand ) override;

	/**
	 * @brief Unregister command
	 * @param pCommand	Console command
	 */
	virtual void UnregisterCommand( IConCmdBase* pCommand ) override;

	/**
	 * @brief Unregister commands
	 * @param dllIdentifier		DLL identifier
	 */
	virtual void UnregisterCommands( CVarDLLIdentifier_t dllIdentifier ) override;

	/**
	 * @brief Execute command
	 *
	 * @param pCommand		Command
	 * @return Return TRUE if command is successfully executed, otherwise returns FALSE
	 */
	virtual bool Exec( const achar* pCommand ) override;

	/**
	 * @brief Find command base by name
	 *
	 * @param pName		Command name
	 * @return Return pointer to command base if found, otherwise returns NULL
	 */
	virtual IConCmdBase* FindCommandBase( const achar* pName ) const override;

	/**
	 * @brief Find command by name
	 *
	 * @param pName		Command name
	 * @return Return pointer to command if found, otherwise returns NULL
	 */
	virtual IConCmd* FindCommand( const achar* pName ) const override;

	/**
	 * @brief Find variable by name
	 *
	 * @param pName		Command name
	 * @return Return pointer to variable if found, otherwise returns NULL
	 */
	virtual IConVar* FindVar( const achar* pName ) const override;

	/**
	 * @brief Reset cvars which contain a specific flags
	 * Sets cvars containing the flags to their default value
	 * 
	 * @param flags		Cvar flags
	 */
	virtual void ResetFlaggedVars( uint32 flags ) override;

	/**
	 * @brief Get the first IConCmdBase to allow iteration over all IConCmd and IConVars
	 * @return Return the first IConCmdBase to allow iteration. If in the system isn't any commands returns NULL
	 */
	virtual IConCmdBase* GetCommands() const override;

	/**
	 * @brief Set global change callback of IConVars
	 * @note To be called when any IConVar changes
	 *
	 * @param pChangeCallbackFn		Change callback
	 */
	virtual void SetGlobalChangeCallback( ConVarChangeCallbackFn_t pChangeCallbackFn ) override;

	/**
	 * @brief Call global change callback
	 * @param pConVar	Changed cvar
	 */
	virtual void CallGlobalChangeCallback( IConVar* pConVar ) override;

	/**
	 * @brief Set console display function
	 * @param pConsoleDisplayFunc	Console display function
	 */
	virtual void SetConsoleDisplayFunc( IConsoleDisplayFunc* pConsoleDisplayFunc ) override;

	/**
	 * @brief Print message into console
	 *
	 * @param color		Message color
	 * @param pFormat	Message format
	 * @param ...		Other message parameters
	 */
	virtual void ConsolePrintf( const CColor& color, const achar* pFormat, ... ) override;

	/**
	 * @brief Print message into console
	 *
	 * @param pFormat	Message format
	 * @param ...		Other message parameters
	 */
	virtual void ConsolePrintf( const achar* pFormat, ... ) override;

	/**
	 * @brief Set ICvarQuery for the console system
	 *
	 * Method allowing the engine ICvarQuery interface to take over
	 * A little hacky, owing to the fact the engine is loaded
	 * well after ICvar, so we can't use the standard connect pattern
	 *
	 * @param pCvarQuery	Cvar query to set. If NULL ICvar set to default ICvarQuery
	 */
	virtual void SetCVarQuery( ICvarQuery* pCvarQuery ) override;

private:
	enum
	{
		COMMAND_MAX_ARGC		= 64,	/**< Max count arguments in a command */
		COMMAND_MAX_LENGTH		= 512	/**< Max length of a command */
	};

	/**
	 * @brief Parse command
	 * 
	 * This method parse command and result place into variables:
	 * commandArgc - Here will be count command arguments
	 * pCommandArgv - Here will be command arguments
	 * 
	 * @param pCommand		Command to parse. After the function is executed, this pointer will point to the beginning of the next command after the separator, or NULL if there is nothing else left
	 * @param separator		Symbol for splitting a command
	 * @return Return TRUE if command was successfully parsed, otherwise returns FALSE
	 */
	bool ParseCommand( const achar*& pCommand, const achar separator = '$' );

	IConCmdBase*				pConCmdList;							/**< Console command list */
	ICvarQuery*					pCvarQuery;								/**< Installed ICvarQuery */
	CVarDLLIdentifier_t			nextDLLIdentifier;						/**< Next a unique DLL identifier */
	ConVarChangeCallbackFn_t	pGlobalChangeCallbackFn;				/**< Global change callback of IConVars */
	IConsoleDisplayFunc*		pConsoleDisplayFunc;					/**< Console display function */
	achar						commandArgvBuffer[COMMAND_MAX_LENGTH];	/**< Buffer for command arguments */
	uint32						commandArgc;							/**< Count of command arguments */
	const achar*				pCommandArgv[COMMAND_MAX_ARGC];			/**< Array of pointer to begin each command argument */
};

static CCvar		s_Cvar;
static CCvarQuery	s_CvarQuery;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR( CCvar, ICvar, CVAR_INTERFACE_VERSION, s_Cvar );
EXPOSE_SINGLE_INTERFACE_GLOBALVAR( CCvarQuery, ICvarQuery, CVAR_QUERY_INTERFACE_VERSION, s_CvarQuery );


/*
==================
CCvarQuery::CCvarQuery
==================
*/
CCvarQuery::CCvarQuery()
	: pCvar( nullptr )
{}

/*
==================
CCvarQuery::Connect
==================
*/
bool CCvarQuery::Connect( CreateInterfaceFn_t pFactory )
{
	pCvar = ( ICvar* )pFactory( CVAR_INTERFACE_VERSION );
	if ( !pCvar )
	{
		return false;
	}

	pCvar->SetCVarQuery( this );
	return true;
}

/*
==================
CCvarQuery::Disconnect
==================
*/
void CCvarQuery::Disconnect()
{
	if ( pCvar )
	{
		pCvar->SetCVarQuery( nullptr );
	}
}

/*
==================
CCvarQuery::QueryInterface
==================
*/
void* CCvarQuery::QueryInterface( const achar* pInterfaceName )
{
	if ( !L_Stricmp( pInterfaceName, CVAR_QUERY_INTERFACE_VERSION ) )
	{
		return ( ICvarQuery* )this;
	}

	return nullptr;
}

/*
==================
CCvarQuery::AreConVarsLinkable
==================
*/
bool CCvarQuery::AreConVarsLinkable( const IConVar* pChildVar, const IConVar* pParentVar )
{
	return true;
}


/*
==================
CCvar::CCvar
==================
*/
CCvar::CCvar()
	: pConCmdList( nullptr )
	, pCvarQuery( &s_CvarQuery )
	, nextDLLIdentifier( 0 )
	, pGlobalChangeCallbackFn( nullptr )
	, pConsoleDisplayFunc( nullptr )
	, commandArgc( 0 )
{}

/*
==================
CCvar::Connect
==================
*/
bool CCvar::Connect( CreateInterfaceFn_t pFactory )
{
	return ConnectStdLib( pFactory );
}

/*
==================
CCvar::Disconnect
==================
*/
void CCvar::Disconnect()
{
	DisconnectStdLib();
}

/*
==================
CCvar::Init
==================
*/
bool CCvar::Init()
{
	ConVar_Register();
	return true;
}

/*
==================
CCvar::Shutdown
==================
*/
void CCvar::Shutdown()
{
	ConVar_Unregister();
}

/*
==================
CCvar::AllocateDLLIdentifier
==================
*/
CVarDLLIdentifier_t CCvar::AllocateDLLIdentifier()
{
	return nextDLLIdentifier++;
}

/*
==================
CCvar::RegisterCommand
==================
*/
void CCvar::RegisterCommand( IConCmdBase* pCommand )
{
	// Do nothing if command already is registered
	if ( pCommand->IsRegistered() )
	{
		return;
	}

	// Mark as registered
	pCommand->SetRegistered( true );

	// Do nothing if name isn't valid
	const achar*	pName = pCommand->GetName();
	if ( !pName || !pName[0] )
	{
		pCommand->SetNext( nullptr );
		return;
	}

	// If the variable is already defined, then setup the new variable as a proxy to it
	const IConCmdBase*	pOtherCommand = FindVar( pName );
	if ( pOtherCommand )
	{
		// We unable link commands, only variables
		if ( pCommand->IsCommand() || pOtherCommand->IsCommand() )
		{
			Warning( "Cvar: Unable to link %s and %s because one or more is a command", pName, pOtherCommand->GetName() );
		}
		// Otherwise link variables
		else
		{
			// This cast is ok because we make sure they're IConVars above
			IConVar*	pChildVar = ( IConVar* )pCommand;
			IConVar*	pParentVar = ( IConVar* )pOtherCommand;

			// See if it's a valid linkage
			if ( pCvarQuery->AreConVarsLinkable( pChildVar, pParentVar ) )
			{
				// Make sure the default values are the same
				if ( pChildVar->GetDefault() && pParentVar->GetDefault() && L_Stricmp( pChildVar->GetDefault(), pParentVar->GetDefault() ) != 0 )
				{
					Warning( "Cvar: Parent and child IConVars with different default values! %s child: %s parent: %s (parent wins)",
							 pCommand->GetName(), pChildVar->GetDefault(), pParentVar->GetDefault() );
				}

				// Set parent
				pChildVar->SetParent( pParentVar->GetParent() );

				// Check the parent's callbacks and slam if doesn't have, warn if both have callbacks
				if ( pChildVar->GetChangeCallback() )
				{
					if ( !pParentVar->GetChangeCallback() )
					{
						pParentVar->SetChangeCallback( pChildVar->GetChangeCallback() );
					}
					else
					{
						Warning( "Cvar: IConVar %s has multiple different change callbacks", pCommand->GetName() );
					}
				}

				// Make sure we don't have conflicting help strings
				if ( pChildVar->GetHelpText() && L_Strlen( pChildVar->GetHelpText() ) > 0 )
				{
					if ( pParentVar->GetHelpText() && L_Strlen( pParentVar->GetHelpText() ) > 0 )
					{
						if ( L_Stricmp( pParentVar->GetHelpText(), pChildVar->GetHelpText() ) != 0 )
						{
							Warning( "Cvar: IConVar %s has multiple help strings:\n\tparent (wins): \"%s\"\n\tchild: \"%s\"",
									 pCommand->GetName(), pParentVar->GetHelpText(), pChildVar->GetHelpText() );
						}
					}
					else
					{
						pParentVar->SetHelpText( pChildVar->GetHelpText() );
					}
				}

				// Make sure we don't have conflicting FCVAR_CHEAT flag
				if ( pChildVar->IsFlagSet( FCVAR_CHEAT ) != pParentVar->IsFlagSet( FCVAR_CHEAT ) )
				{
					Warning( "Cvar: IConVar %s has conflicting FCVAR_CHEAT flags (child: %s, parent: %s, parent wins)",
							 pCommand->GetName(), pChildVar->IsFlagSet( FCVAR_CHEAT ) ? "FCVAR_CHEAT" : "no FCVAR_CHEAT",
							 pParentVar->IsFlagSet( FCVAR_CHEAT ) ? "FCVAR_CHEAT" : "no FCVAR_CHEAT" );
				}
			}
		}

		pCommand->SetNext( nullptr );
		return;
	}

	// Link the variable in
	pCommand->SetNext( pConCmdList );
	pConCmdList = pCommand;
}

/*
==================
CCvar::UnregisterCommand
==================
*/
void CCvar::UnregisterCommand( IConCmdBase* pCommand )
{
	// Do nothing if command isn't registered
	if ( !pCommand->IsRegistered() )
	{
		return;
	}

	// Unmark as registered
	pCommand->SetRegistered( false );

	// Remove command
	for ( IConCmdBase* pCurCommand = pConCmdList, *pPrevCommand = nullptr; pCurCommand; pCurCommand = pCurCommand->GetNext() )
	{
		if ( pCurCommand != pCommand )
		{
			pPrevCommand = pCurCommand;
			continue;
		}

		if ( !pPrevCommand )
		{
			pConCmdList = pCurCommand->GetNext();
		}
		else
		{
			pPrevCommand->SetNext( pCurCommand->GetNext() );
		}

		pCurCommand->SetNext( nullptr );
		break;
	}
}

/*
==================
CCvar::UnregisterCommands
==================
*/
void CCvar::UnregisterCommands( CVarDLLIdentifier_t dllIdentifier )
{
	IConCmdBase*	pNewList = nullptr;
	IConCmdBase*	pCurCommand = pConCmdList, *pNextCommand = nullptr;
	while ( pCurCommand )
	{
		pNextCommand = pCurCommand->GetNext();
		if ( pCurCommand->GetDLLIdentifier() != dllIdentifier )
		{
			pCurCommand->SetNext( pNewList );
			pNewList = pCurCommand;
		}
		else
		{
			pCurCommand->SetRegistered( false );
			pCurCommand->SetNext( nullptr );
		}

		pCurCommand = pNextCommand;
	}

	pConCmdList = pNewList;
}

/*
==================
CCvar::ParseCommand
==================
*/
bool CCvar::ParseCommand( const achar*& pCommand, const achar separator /*= '$'*/ )
{
	// Reset command buffers
	commandArgc				= 0;
	commandArgvBuffer[0]	= '\0';

	// Do nothing if the command isn't valid
	if ( !pCommand || !pCommand[0] )
	{
		return false;
	}

	// Skip separators and spaces
	while ( *pCommand && ( *pCommand == separator || L_IsSpace( *pCommand ) ) )
	{
		++pCommand;
	}

	// Find the end of the command
	const achar*	pEndCommand	= pCommand;
	{
		bool	bFoundQuote = false;
		for ( ; *pEndCommand; ++pEndCommand )
		{
			// We must ignore a separator in quotes
			if ( *pEndCommand == '\"' )
			{
				bFoundQuote = !bFoundQuote;
				continue;
			}

			// We found a separator, it is the end
			if ( !bFoundQuote && *pEndCommand == separator )
			{
				break;
			}
		}
	}

	// Move pEndCommand to nearest symbol for save buffer size
	const achar*	pNextCommand = pEndCommand;
	while ( pEndCommand != pCommand && ( L_IsSpace( *pEndCommand ) || *pEndCommand == separator ) )
	{
		if ( !L_IsSpace( *( pEndCommand-1 ) ) && *( pEndCommand-1 ) != separator )
		{
			break;
		}
		--pEndCommand;
	}

	// Copy the current command into commandArgvBuffer
	uint32		commandLength = ( uint32 )( pEndCommand - pCommand );
	if ( commandLength >= COMMAND_MAX_LENGTH - 1 )
	{
		Warning( "Cvar: Encountered command which overflows the buffer, skipping!" );
		return false;
	}

	Mem_Memcpy( &commandArgvBuffer[0], pCommand, commandLength );
	commandArgvBuffer[commandLength]	= '\0';
	pCommand							= pNextCommand;

	// Parse the current command into pCommandArgv
	achar*	pStartArgv		= &commandArgvBuffer[0];
	achar*	pCurChar		= pStartArgv;
	bool	bFoundQuote		= false;
	bool	bEndSubstring	= false;
	while ( *pCurChar && commandArgc < COMMAND_MAX_ARGC )
	{
		// We found a quote
		if ( *pCurChar == '\"' )
		{
			bFoundQuote = !bFoundQuote;
			if ( bFoundQuote )
			{
				// The quote must be at begin of the argv, otherwise it's syntax error
				if ( pCurChar == pStartArgv )
				{
					++pStartArgv;
				}
				else
				{
					Warning( "Cvar: Encountered command have wrong syntax" );
					return false;
				}
			}
			else
			{
				if ( pCurChar != pStartArgv )
				{
					--pCurChar;
				}
				bEndSubstring = true;
			}
		}
		// Otherwise if next will be a space or the end of string then also we found another argv
		else if ( !bFoundQuote && L_IsSpace( *( pCurChar+1 ) ) || !*( pCurChar+1 ) )
		{
			bEndSubstring = true;
		}

		// We found substring save pointer into pCommandArgv
		if ( bEndSubstring )
		{
			bEndSubstring				= false;
			pCommandArgv[commandArgc]	= pStartArgv;
			++commandArgc;
			if ( commandArgc >= COMMAND_MAX_ARGC )
			{
				Warning( "Cvar: Encountered command which overflows the argument buffer, clamped!" );
			}

			// Set in the argv null terminator if it need
			if ( *( pCurChar+1 ) )
			{
				++pCurChar;
				*pCurChar = '\0';
			}
			++pCurChar;

			// Eat white space
			while ( *pCurChar && L_IsSpace( *pCurChar ) )
			{
				++pCurChar;
			}
			pStartArgv = pCurChar;
			
		}
		else
		{
			++pCurChar;
		}
	}

	return true;
}

/*
==================
CCvar::Exec
==================
*/
bool CCvar::Exec( const achar* pCommand )
{
	// Do nothing if a command is empty
	if ( !pCommand || !pCommand[0] )
	{
		return false;
	}

	// Execute the command
	bool	bResult = false;
	while ( pCommand && *pCommand != '\0' )
	{
		// Parse the command
		if ( ParseCommand( pCommand ) && commandArgc > 0 )
		{
			// Find a command
			IConCmdBase*	pConCmdBase = FindCommandBase( pCommandArgv[0] );
			if ( pConCmdBase )
			{
				// Allow cheat commands with 'cheats' on
				if ( pConCmdBase->IsFlagSet( FCVAR_CHEAT ) && !cheats.GetBool() && ( pConCmdBase->IsCommand() || commandArgc > 1 ) )
				{
					Warning( "Cvar: Can't use cheat command \"%s\", unless has cheats set to 1", pCommandArgv[0] );
					continue;
				}

				// Execute if it is command
				if ( pConCmdBase->IsCommand() )
				{
					IConCmd*	pConCmd = ( IConCmd* )pConCmdBase;
					pConCmd->Exec( commandArgc - 1, pCommandArgv + 1 );
				}
				// Otherwise it's variable
				else 
				{
					// Change variable if we have enough arguments
					IConVar*	pConVar = ( IConVar* )pConCmdBase;
					if ( commandArgc > 1 )
					{
						pConVar->SetString( pCommandArgv[1] );
					}
					// Otherwise print current and default value
					else
					{
						Msg( "Cvar: %s (%s): Current \"%s\", Default: \"%s\"", pCommandArgv[0], pConVar->GetHelpText(), pConVar->GetString(), pConVar->GetDefault() );
					}
				}

				bResult |= true;
			}
			else
			{
				Warning( "Cvar: Unknown command \"%s\"", pCommandArgv[0] );
			}
		}
	}
	
	return bResult;
}

/*
==================
CCvar::FindCommandBase
==================
*/
IConCmdBase* CCvar::FindCommandBase( const achar* pName ) const
{
	for ( IConCmdBase* pCurCommand = pConCmdList; pCurCommand; pCurCommand = pCurCommand->GetNext() )
	{
		if ( !L_Stricmp( pName, pCurCommand->GetName() ) )
		{
			return pCurCommand;
		}
	}

	return nullptr;
}

/*
==================
CCvar::FindCommand
==================
*/
IConCmd* CCvar::FindCommand( const achar* pName ) const
{
	IConCmdBase*	pConCmd = FindCommandBase( pName );
	if ( !pConCmd || !pConCmd->IsCommand() )
	{
		return nullptr;
	}

	return ( IConCmd* )pConCmd;
}

/*
==================
CCvar::FindVar
==================
*/
IConVar* CCvar::FindVar( const achar* pName ) const
{
	IConCmdBase*	pConVar = FindCommandBase( pName );
	if ( !pConVar || pConVar->IsCommand() )
	{
		return nullptr;
	}

	return ( IConVar* )pConVar;
}

/*
==================
CCvar::ResetFlaggedVars
==================
*/
void CCvar::ResetFlaggedVars( uint32 flags )
{
	for ( IConCmdBase* pCurVar = pConCmdList; pCurVar; pCurVar = pCurVar->GetNext() )
	{
		// Skip commands and cvars that no have a specific flags
		if ( pCurVar->IsCommand() || !pCurVar->IsFlagSet( flags ) )
		{
			continue;
		}

		// Do nothing if cvar already have the default value
		IConVar*	pCvar = ( IConVar* )pCurVar;
		if ( !L_Stricmp( pCvar->GetDefault(), pCvar->GetString() ) )
		{
			continue;
		}

		// Otherwise reset it
		pCvar->Reset();
	}
}

/*
==================
CCvar::GetCommands
==================
*/
IConCmdBase* CCvar::GetCommands() const
{
	return pConCmdList;
}

/*
==================
CCvar::SetGlobalChangeCallback
==================
*/
void CCvar::SetGlobalChangeCallback( ConVarChangeCallbackFn_t pChangeCallbackFn )
{
	pGlobalChangeCallbackFn = pChangeCallbackFn;
}

/*
==================
CCvar::CallGlobalChangeCallback
==================
*/
void CCvar::CallGlobalChangeCallback( IConVar* pConVar )
{
	if ( pGlobalChangeCallbackFn )
	{
		pGlobalChangeCallbackFn( pConVar );
	}
}

/*
==================
CCvar::SetConsoleDisplayFunc
==================
*/
void CCvar::SetConsoleDisplayFunc( IConsoleDisplayFunc* pConsoleDisplayFunc )
{
	CCvar::pConsoleDisplayFunc = pConsoleDisplayFunc;
}

/*
==================
CCvar::ConsolePrintf
==================
*/
void CCvar::ConsolePrintf( const CColor& color, const achar* pFormat, ... )
{
	if ( pConsoleDisplayFunc )
	{
		va_list		params;
		va_start( params, pFormat );
		pConsoleDisplayFunc->Print( L_Vsprintf( pFormat, params ).c_str(), color );
		va_end( params );
	}
}

/*
==================
CCvar::ConsolePrintf
==================
*/
void CCvar::ConsolePrintf( const achar* pFormat, ... )
{
	if ( pConsoleDisplayFunc )
	{
		va_list		params;
		va_start( params, pFormat );
		pConsoleDisplayFunc->Print( L_Vsprintf( pFormat, params ).c_str() );
		va_end( params );
	}
}

/*
==================
CCvar::SetCVarQuery
==================
*/
void CCvar::SetCVarQuery( ICvarQuery* pCvarQuery )
{
	CCvar::pCvarQuery = pCvarQuery ? pCvarQuery : &s_CvarQuery;
}


/*
==================
Exec command
==================
*/
CON_COMMAND( exec, "Execute a command file", FCVAR_NONE )
{
	if ( argc < 1 || !argv )
	{
		Msg( "exec <path> : Execute a command file" );
		return;
	}

	// Open a command file
	TRefPtr<IFileReader>	file = g_pFileSystem->CreateFileReader( argv[0] );
	if ( file )
	{
		// Read whole file into buffer
		std::string		buffer;
		buffer.resize( file->GetSize() / sizeof( achar ) );
		file->Read( buffer.data(), file->GetSize() );

		// Executing a file
		Msg( "exec %s: Executing", argv[0] );
		std::stringstream	sstream( buffer );
		std::string			line;
		while ( std::getline( sstream, line ) )
		{
			// We throw away \r
			if ( !line.empty() && line.back() == '\r' )
			{
				line.pop_back();
			}

			// We ignore line if it starts with C++ comment (//)
			if ( line.rfind( "//", 0 ) == std::string::npos )
			{
				g_pCvar->Exec( line.c_str() );
			}
		}

		// We are done!
		return;
	}

	// We failed to open file, it is bad
	Warning( "exec %s: Failed to open file", argv[0] );
}