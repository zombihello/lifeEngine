#include "Logger/LoggerMacros.h"
#include "Scripts/ScriptTokenEater.h"

/*
==================
CScriptTokenEater::CScriptTokenEater
==================
*/
CScriptTokenEater::CScriptTokenEater( CScriptTokenStream& OutTokens, const std::wstring& InFileName )
	: tokens( OutTokens )
	, fileName( InFileName )
{
	tokens.GetTokens().reserve( 1000 );
}

/*
==================
CScriptTokenEater::OnEmitToken
==================
*/
void CScriptTokenEater::OnEmitToken( uint32 InLine, uint32 InTokenID, const std::string& InTokenString, uint32 InScopeLevel, uint32 InCharLineStart, uint32 InCharStart, uint32 InCharEnd )
{
	// Setup context information
	const ScriptFileContext		context( fileName, InLine, InCharLineStart, InCharStart, InCharEnd );
	tokens.PushToken( std::forward<ScriptToken>( ScriptToken( InTokenString, InTokenID, InScopeLevel, context ) ) );
}

/*
==================
CScriptTokenEater::OnEmitComment
==================
*/
void CScriptTokenEater::OnEmitComment( uint32 InLine, const std::string& InText, uint32 InScopeLevel, uint32 InCharLineStart, uint32 InCharStart, uint32 InCharEnd )
{}

/*
==================
CScriptTokenEater::OnEmitError
==================
*/
void CScriptTokenEater::OnEmitError( uint32 InLine, const achar* InMessage )
{
	Errorf( TEXT( "%s: %s\n" ), ScriptFileContext( fileName, InLine ).ToString().c_str(), ANSI_TO_TCHAR( InMessage ) );
}