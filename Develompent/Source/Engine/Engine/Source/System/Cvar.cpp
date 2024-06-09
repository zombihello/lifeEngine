#include "Misc/EngineGlobals.h"
#include "System/Cvar.h"

/*
==================
CheatsChanged
==================
*/
static void CheatsChanged( CConVar* InConVar )
{
	// Cheats were disabled, revert all cheat cvars to their default values
	if ( InConVar->GetInt() == 0 )
	{
		g_Cvar.ResetFlaggedVars( FCVAR_Cheat );
		Logf( TEXT( "FCVAR_Cheat cvars reverted to defaults\n" ) );
	}
}

/**
 * @ingroup Engine
 * @brief Cvar allow cheats in the game
 */
CConVar		CVarCheats( TEXT( "cheats" ), TEXT( "0" ), TEXT( "Allow cheats in the game" ), FCVAR_None, CheatsChanged );


/*
==================
CCvar::CCvar
==================
*/
CCvar::CCvar()
	: conCmdList( nullptr )
	, commandArgc( 0 )
{}

/*
==================
CCvar::RegisterCommand
==================
*/
void CCvar::RegisterCommand( CConCmdBase* InCommand )
{
	// Do nothing if command already is registered
	Assert( InCommand );
	if ( InCommand->bRegistered )
	{
		return;
	}

	// Mark as registered
	InCommand->bRegistered = true;

	// Do nothing if name isn't valid
	const tchar*	name = InCommand->GetName();
	if ( !name || !name[0] )
	{
		InCommand->next = nullptr;
		return;
	}

	// Link the variable in
	InCommand->next = conCmdList;
	conCmdList = InCommand;
}

/*
==================
CCvar::UnregisterCommand
==================
*/
void CCvar::UnregisterCommand( CConCmdBase* InCommand )
{
	// Do nothing if command isn't registered
	Assert( InCommand );
	if ( !InCommand->bRegistered )
	{
		return;
	}

	// Unmark as registered
	InCommand->bRegistered = false;

	// Remove command
	for ( CConCmdBase* curCommand = conCmdList, *prevCommand = nullptr; curCommand; curCommand = curCommand->next )
	{
		if ( curCommand != InCommand )
		{
			prevCommand = curCommand;
			continue;
		}

		if ( !prevCommand )
		{
			conCmdList = curCommand->next;
		}
		else
		{
			prevCommand->next = curCommand->next;
		}

		curCommand->next = nullptr;
		break;
	}
}

/*
==================
CCvar::UnregisterAllCommands
==================
*/
void CCvar::UnregisterAllCommands()
{
	// Remove commands
	for ( CConCmdBase* curCommand = conCmdList; curCommand; curCommand = curCommand->next )
	{
		curCommand->bRegistered = false;
		curCommand->next		= nullptr;
	}
	conCmdList = nullptr;
}

/*
==================
CCvar::Exec
==================
*/
bool CCvar::Exec( const tchar* InCommand )
{
	// Do nothing if a command is empty
	if ( !InCommand || !InCommand[0] )
	{
		return false;
	}

	// Execute the command
	bool	bResult = false;
	while ( InCommand && *InCommand != TEXT( '\0' ) )
	{
		// Parse the command
		if ( ParseCommand( InCommand ) && commandArgc > 0 )
		{
			// Find a command
			CConCmdBase*	conCmdBase = FindCommandBase( commandArgv[0] );
			if ( conCmdBase )
			{
				// Allow cheat commands with 'cheats' on
				if ( conCmdBase->IsFlagSet( FCVAR_Cheat ) && !CVarCheats.GetBool() && ( conCmdBase->IsCommand() || commandArgc > 1 ) )
				{
					Warnf( TEXT( "Can't use cheat command \"%s\", unless has cheats set to 1\n" ), commandArgv[0] );
					continue;
				}

				// Execute if it is command
				if ( conCmdBase->IsCommand() )
				{
					CConCmd*	conCmd = ( CConCmd* )conCmdBase;
					conCmd->Exec( commandArgc - 1, commandArgv + 1 );
				}
				// Otherwise it's variable
				else 
				{
					// Change variable if we have enough arguments
					CConVar*	conVar = ( CConVar* )conCmdBase;
					if ( commandArgc > 1 )
					{
						conVar->SetString( commandArgv[1] );
					}
					// Otherwise print current and default value
					else
					{
						Logf( TEXT( "%s (%s): Current \"%s\", Default: \"%s\"\n" ), commandArgv[0], conVar->GetHelpText(), conVar->GetString(), conVar->GetDefault() );
					}
				}

				bResult |= true;
			}
			else
			{
				Warnf( TEXT( "Unknown command \"%s\"\n" ), commandArgv[0] );
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
CConCmdBase* CCvar::FindCommandBase( const tchar* InName ) const
{
	for ( CConCmdBase* curCommand = conCmdList; curCommand; curCommand = curCommand->next )
	{
		if ( !L_Stricmp( InName, curCommand->GetName() ) )
		{
			return curCommand;
		}
	}

	return nullptr;
}

/*
==================
CCvar::FindCommand
==================
*/
CConCmd* CCvar::FindCommand( const tchar* InName ) const
{
	CConCmdBase*	conCmd = FindCommandBase( InName );
	if ( !conCmd || !conCmd->IsCommand() )
	{
		return nullptr;
	}

	return ( CConCmd* )conCmd;
}

/*
==================
CCvar::FindVar
==================
*/
CConVar* CCvar::FindVar( const tchar* InName ) const
{
	CConCmdBase*	conVar = FindCommandBase( InName );
	if ( !conVar || conVar->IsCommand() )
	{
		return nullptr;
	}

	return ( CConVar* )conVar;
}

/*
==================
CCvar::ResetFlaggedVars
==================
*/
void CCvar::ResetFlaggedVars( uint32 InFlags )
{
	for ( CConCmdBase* curVar = conCmdList; curVar; curVar = curVar->next )
	{
		// Skip commands and cvars that no have a specific flags
		if ( curVar->IsCommand() || !curVar->IsFlagSet( InFlags ) )
		{
			continue;
		}

		// Do nothing if cvar already have the default value
		CConVar*	cvar = ( CConVar* )curVar;
		if ( !L_Stricmp( cvar->GetDefault(), cvar->GetString() ) )
		{
			continue;
		}

		// Otherwise reset it
		cvar->Reset();
	}
}

/*
==================
CCvar::ParseCommand
==================
*/
bool CCvar::ParseCommand( const tchar*& InCommand, const tchar InSeparator /* = TEXT( '$' ) */ )
{
	// Reset command buffers
	commandArgc				= 0;
	commandArgvBuffer[0]	= TEXT( '\0' );

	// Do nothing if the command isn't valid
	if ( !InCommand || !InCommand[0] )
	{
		return false;
	}

	// Skip separators and spaces
	while ( *InCommand && ( *InCommand == InSeparator || L_IsSpace( *InCommand ) ) )
	{
		++InCommand;
	}

	// Find the end of the command
	const tchar*	endCommand	= InCommand;
	{
		bool	bFoundQuote = false;
		for ( ; *endCommand; ++endCommand )
		{
			// We must ignore a separator in quotes
			if ( *endCommand == TEXT( '\"' ) )
			{
				bFoundQuote = !bFoundQuote;
				continue;
			}

			// We found a separator, it is the end
			if ( !bFoundQuote && *endCommand == InSeparator )
			{
				break;
			}
		}
	}

	// Move endCommand to nearest symbol for save buffer size
	const tchar*	nextCommand = endCommand;
	while ( endCommand != InCommand && ( L_IsSpace( *endCommand ) || *endCommand == InSeparator ) )
	{
		if ( !L_IsSpace( *( endCommand-1 ) ) && *( endCommand-1 ) != InSeparator )
		{
			break;
		}
		--endCommand;
	}

	// Copy the current command into commandArgvBuffer
	uint32		commandLength = ( uint32 )( endCommand - InCommand );
	if ( commandLength >= COMMAND_MAX_LENGTH - 1 )
	{
		Warnf( TEXT( "CCvar::ParseCommand: Encountered command which overflows the buffer, skipping!\n" ) );
		return false;
	}

	Memory::Memcpy( &commandArgvBuffer[0], InCommand, commandLength * sizeof( tchar ) );
	commandArgvBuffer[commandLength]	= TEXT( '\0' );
	InCommand							= nextCommand;

	// Parse the current command into pCommandArgv
	tchar*	startArgv		= &commandArgvBuffer[0];
	tchar*	curChar			= startArgv;
	bool	bFoundQuote		= false;
	bool	bEndSubstring	= false;
	while ( *curChar && commandArgc < COMMAND_MAX_ARGC )
	{
		// We found a quote
		if ( *curChar == TEXT( '\"' ) )
		{
			bFoundQuote = !bFoundQuote;
			if ( bFoundQuote )
			{
				// The quote must be at begin of the argv, otherwise it's syntax error
				if ( curChar == startArgv )
				{
					++startArgv;
				}
				else
				{
					Warnf( TEXT( "CCvar::ParseCommand: Encountered command have wrong syntax\n" ) );
					return false;
				}
			}
			else
			{
				if ( curChar != startArgv )
				{
					--curChar;
				}
				bEndSubstring = true;
			}
		}
		// Otherwise if next will be a space or the end of string then also we found another argv
		else if ( !bFoundQuote && L_IsSpace( *( curChar+1 ) ) || !*( curChar+1 ) )
		{
			bEndSubstring = true;
		}

		// We found substring save pointer into pCommandArgv
		if ( bEndSubstring )
		{
			bEndSubstring				= false;
			commandArgv[commandArgc]	= startArgv;
			++commandArgc;
			if ( commandArgc >= COMMAND_MAX_ARGC )
			{
				Warnf( TEXT( "CCvar::ParseCommand: Encountered command which overflows the argument buffer, clamped!\n" ) );
			}

			// Set in the argv null terminator if it need
			if ( *( curChar+1 ) )
			{
				++curChar;
				*curChar = TEXT( '\0' );
			}
			++curChar;

			// Eat white space
			while ( *curChar && L_IsSpace( *curChar ) )
			{
				++curChar;
			}
			startArgv = curChar;
			
		}
		else
		{
			++curChar;
		}
	}

	return true;
}