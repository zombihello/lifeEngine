#include "Misc/EngineGlobals.h"
#include "System/ConCmd.h"
#include "System/ConsoleSystem.h"

/*
==================
CConCmd::CConCmd
==================
*/
CConCmd::CConCmd( const std::wstring& InName, const std::wstring& InHelpText, const COnExecCmd::DelegateType_t& InExecDelegate )
	: name( InName )
	, helpText( InHelpText )
{
	onExecCmd.Bind( InExecDelegate );
	GetGlobalConCmds().push_back( this );
}

/*
==================
CConCmd::~CConCmd
==================
*/
CConCmd::~CConCmd()
{
	std::vector<CConCmd*>&		cmds = GetGlobalConCmds();
	for ( uint32 index = 0, count = cmds.size(); index < count; ++index )
	{
		if ( cmds[index] == this )
		{
			cmds.erase( cmds.begin() + index );
			break;
		}
	}
}