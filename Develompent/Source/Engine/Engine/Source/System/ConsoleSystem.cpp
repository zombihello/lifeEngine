#include "Logger/LoggerMacros.h"
#include "System/ConsoleSystem.h"

//
// GLOBALS
//
CConCmd			CCmdHelp( TEXT( "help" ), TEXT( "Show help variables and comands" ), std::bind( &CConsoleSystem::CmdHelp, std::placeholders::_1 ) );

void CConsoleSystem::UnRegisterVar( CConVar* InConVar )
{
	check( InConVar );
	std::vector<CConVar*>&		vars = GetConVars();
	
	for ( uint32 index = 0, count = vars.size(); index < count; ++index )
	{
		if ( vars[index] == InConVar )
		{
			vars.erase( vars.begin() + index );
			break;
		}
	}
}

void CConsoleSystem::UnRegisterCmd( CConCmd* InConCmd )
{
	check( InConCmd );
	std::vector<CConCmd*>& cmds = GetConCmds();

	for ( uint32 index = 0, count = cmds.size(); index < count; ++index )
	{
		if ( cmds[index] == InConCmd )
		{
			cmds.erase( cmds.begin() + index );
			break;
		}
	}
}

bool CConsoleSystem::Exec( const std::wstring& InCommand )
{
	if ( InCommand.empty() )
	{
		return false;
	}

	// Find command name or variable in string
	size_t			startIndexName	= InCommand.find_first_not_of( TEXT( ' ' ) );
	size_t			endIndexName	= InCommand.find_first_of( TEXT( ' ' ), startIndexName );
	std::wstring	name			= InCommand.substr( startIndexName, endIndexName - startIndexName );
	std::wstring	arguments		= endIndexName != std::wstring::npos ? InCommand.substr( endIndexName + 1, InCommand.size() - endIndexName ) : TEXT( "" );

	// Find by name variable, if we found change him value
	{
		CConVar*	var = FindVar( name );
		if ( var )
		{
			if ( !arguments.empty() && arguments.find_first_not_of( TEXT( ' ' ) ) != std::wstring::npos )
			{
				var->SetValue( arguments.c_str(), var->GetType() );
			}
			else
			{
				std::wstring		stringVar;
				if ( var->GetType() == CVT_String )
				{
					stringVar = var->GetValueString();
				}
				else
				{
					switch ( var->GetType() )
					{
					case CVT_Int:		stringVar = std::to_wstring( var->GetValueInt() );					break;
					case CVT_Float:		stringVar = std::to_wstring( var->GetValueFloat() );				break;
					case CVT_Bool:		stringVar = var->GetValueBool() ? TEXT( "true" ) : TEXT( "false" ); break;
					default:			
						checkMsg( false, TEXT( "Unknown ConVar type 0x%X" ), var->GetType() );
						stringVar = TEXT( "" );
						break;
					}
				}

				LE_LOG( LT_Log, LC_Console, TEXT( "%s: %s" ), name.c_str(), var->GetHelpText().c_str() );
				LE_LOG( LT_Log, LC_Console, TEXT( "Value: %s" ), stringVar.c_str() );
				LE_LOG( LT_Log, LC_Console, TEXT( "Default value: %s" ), var->GetValueDefault().c_str() );
			}

			LE_LOG( LT_Log, LC_Console, InCommand.c_str() );
			return true;
		}
	}

	// Find by name command, if found execute him
	{
		CConCmd*		cmd = FindCmd( name );
		if ( cmd )
		{
			// Separate string on substrings
			bool							bFindApostrof = false;
			bool							bEndSubstring = false;
			std::wstring					tempString;
			std::vector<std::wstring>		arrayArguments;

			for ( uint32 index = 0, count = arguments.size(); index < count; ++index )
			{
				if ( arguments[index] == TEXT( '\"' ) )
				{
					bFindApostrof = !bFindApostrof;
					if ( bFindApostrof )
					{
						continue;
					}
					else
					{
						bEndSubstring = true;
					}
				}
				else if ( !bFindApostrof && arguments[index] == TEXT( ' ' ) && !tempString.empty() )
				{
					bEndSubstring = true;
				}

				if ( !bEndSubstring )
				{
					tempString += arguments[index];
				}

				if ( bEndSubstring || index + 1 >= count )
				{
					arrayArguments.push_back( tempString );
					tempString.clear();
					bEndSubstring = false;
				}
			}

			// Execute command
			cmd->Exec( arrayArguments );
			return true;
		}
	}
	
	LE_LOG( LT_Error, LC_Console, TEXT( "Not correct command: \"%s\"" ), InCommand.c_str() );
	return false;
}

CConVar* CConsoleSystem::FindVar( const std::wstring& InName )
{
	std::vector<CConVar*>&		vars = GetConVars();
	for ( uint32 index = 0, count = vars.size(); index < count; ++index )
	{
		if ( vars[index]->GetName() == InName )
		{
			return vars[index];
		}
	}

	return nullptr;
}

CConCmd* CConsoleSystem::FindCmd( const std::wstring& InName )
{
	std::vector<CConCmd*>&		cmds = GetConCmds();
	for ( uint32 index = 0, count = cmds.size(); index < count; ++index )
	{
		if ( cmds[index]->GetName() == InName )
		{
			return cmds[index];
		}
	}

	return nullptr;
}

void CConsoleSystem::CmdHelp( const std::vector<std::wstring>& InArguments )
{
	// Print all console variables
	LE_LOG( LT_Log, LC_Console, TEXT( "" ) );
	LE_LOG( LT_Log, LC_Console, TEXT( "** Console variables **" ) );
	{
		std::vector<CConVar*>&		vars = GetConVars();
		for ( uint32 index = 0, count = vars.size(); index < count; ++index )
		{
			CConVar*	var = vars[index];
			LE_LOG( LT_Log, LC_Console, TEXT( "%s : %s. Default value: %s" ), var->GetName().c_str(), var->GetHelpText().c_str(), var->GetValueDefault().c_str() );
		}
	}

	// Print all console commands
	LE_LOG( LT_Log, LC_Console, TEXT( "" ) );
	LE_LOG( LT_Log, LC_Console, TEXT( "** Console commands **" ) );
	{
		std::vector<CConCmd*>&		cmds = GetConCmds();
		for ( uint32 index = 0, count = cmds.size(); index < count; ++index )
		{
			CConCmd*	cmd = cmds[index];
			LE_LOG( LT_Log, LC_Console, TEXT( "%s : %s" ), cmd->GetName().c_str(), cmd->GetHelpText().c_str() );
		}
	}
}