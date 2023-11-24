#include "Logger/LoggerMacros.h"
#include "Scripts/ScriptTokenEater.h"

/*
==================
CScriptTokenEater::CScriptTokenEater
==================
*/
CScriptTokenEater::CScriptTokenEater( CScriptTokenStream& OutTokens, const ScriptStringTCHARBuf_t& InFileName )
	: tokens( &OutTokens )
	, fileName( InFileName )
{
	tokens->GetTokens().reserve( 1000 );
}

/*
==================
CScriptTokenEater::~CScriptTokenEater
==================
*/
CScriptTokenEater::~CScriptTokenEater()
{}

/*
==================
CScriptTokenEater::OnEmitToken
==================
*/
void CScriptTokenEater::OnEmitToken( uint32 InLine, uint32 InLineStart, uint32 InTokenID, uint32 InLevel, const std::string& InText, uint32 InStart, uint32 InEnd )
{
	// Setup context information
	const ScriptFileContext		context( fileName, InLine, InLineStart, InStart, InEnd );
	tokens->PushToken( std::forward<ScriptToken>( ScriptToken( InText, InTokenID, InLevel, context ) ) );
}

/*
==================
CScriptTokenEater::OnEmitComment
==================
*/
void CScriptTokenEater::OnEmitComment( uint32 InLine, uint32 InLineStart, uint32 InLevel, const std::string& InText, uint32 InStart, uint32 InEnd )
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