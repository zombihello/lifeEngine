#include "Misc/EngineGlobals.h"
#include "System/ConCmd.h"
#include "System/ConsoleSystem.h"

CConCmd::CConCmd( const std::wstring& InName, const std::wstring& InHelpText, const COnExecCmd::DelegateType_t& InExecDelegate )
	: name( InName )
	, helpText( InHelpText )
{
	onExecCmd.Bind( InExecDelegate );
	CConsoleSystem::RegisterCmd( this );
}

CConCmd::~CConCmd()
{
	CConsoleSystem::UnRegisterCmd( this );
}