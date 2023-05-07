#include "Logger/LoggerMacros.h"
#include "System/ConsoleSystem.h"

//
// GLOBALS
//
CConCmd			CCmdHelp( TEXT( "help" ), TEXT( "Show help variables and comands" ), std::bind( &CConsoleSystem::CmdHelp, std::placeholders::_1 ) );

/*
==================
CConsoleSystem::Exec
==================
*/
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
						AssertMsg( false, TEXT( "Unknown ConVar type 0x%X" ), var->GetType() );
						stringVar = TEXT( "" );
						break;
					}
				}

				Logf( TEXT( "%s: %s\n" ), name.c_str(), var->GetHelpText().c_str() );
				Logf( TEXT( "Value: %s\n" ), stringVar.c_str() );
				Logf( TEXT( "Default value: %s\n" ), var->GetValueDefault().c_str() );
			}

			Logf( TEXT( "%s\n" ), InCommand.c_str());
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
	
	Errorf( TEXT( "Not correct command: \"%s\"\n" ), InCommand.c_str() );
	return false;
}

/*
==================
CConsoleSystem::FindVar
==================
*/
CConVar* CConsoleSystem::FindVar( const std::wstring& InName )
{
	std::vector<CConVar*>&		vars = GetGlobalConVars();
	for ( uint32 index = 0, count = vars.size(); index < count; ++index )
	{
		if ( vars[index]->GetName() == InName )
		{
			return vars[index];
		}
	}

	return nullptr;
}

/*
==================
CConsoleSystem::FindCmd
==================
*/
CConCmd* CConsoleSystem::FindCmd( const std::wstring& InName )
{
	std::vector<CConCmd*>&		cmds = GetGlobalConCmds();
	for ( uint32 index = 0, count = cmds.size(); index < count; ++index )
	{
		if ( cmds[index]->GetName() == InName )
		{
			return cmds[index];
		}
	}

	return nullptr;
}

/*
==================
CConsoleSystem::CmdHelp
==================
*/
void CConsoleSystem::CmdHelp( const std::vector<std::wstring>& InArguments )
{
	// Print all console variables
	Logf( TEXT( "\n** Console variables **\n" ) );
	{
		std::vector<CConVar*>&		vars = GetGlobalConVars();
		for ( uint32 index = 0, count = vars.size(); index < count; ++index )
		{
			CConVar*	var = vars[index];
			Logf( TEXT( "%s : %s. Default value: %s\n" ), var->GetName().c_str(), var->GetHelpText().c_str(), var->GetValueDefault().c_str() );
		}
	}

	// Print all console commands
	Logf( TEXT( "\n** Console commands **\n" ) );
	{
		std::vector<CConCmd*>&		cmds = GetGlobalConCmds();
		for ( uint32 index = 0, count = cmds.size(); index < count; ++index )
		{
			CConCmd*	cmd = cmds[index];
			Logf( TEXT( "%s : %s\n" ), cmd->GetName().c_str(), cmd->GetHelpText().c_str() );
		}
	}
}