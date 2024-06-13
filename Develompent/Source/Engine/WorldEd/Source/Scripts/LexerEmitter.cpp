#include "Logger/LoggerMacros.h"
#include "Scripts/LexerEmitter.h"

/*
==================
CLexerEmitter::CLexerEmitter
==================
*/
CLexerEmitter::CLexerEmitter( CScriptTokenEater& InScriptListener, const std::wstring& InFileName )
	: scriptListener( InScriptListener )
	, fileName( InFileName )
{}

/*
==================
CLexerEmitter::Comment
==================
*/
void CLexerEmitter::Comment( const LexerState& InLexerState )
{
	uint32	line				= InLexerState.tokenStart.line + 1;
	uint32	scopeLevel			= InLexerState.scopeLevel;
	uint32	charLineStart		= InLexerState.sequenceLineStart.charOffset;
	uint32	charStartPosition	= InLexerState.sequenceStart.charOffset;
	uint32	charEndPosition		= InLexerState.currentContext.charOffset;

	// Sequences include the surrounding quote marks, so we need to reduce the size of the token text respectively
	const achar* text		= InLexerState.source + InLexerState.sequenceStart.byteOffset + 1;
	uint32		 size		= InLexerState.currentContext.byteOffset - InLexerState.sequenceStart.byteOffset - 2;
	scriptListener.OnEmitComment( line, std::string( text, size ), scopeLevel, charLineStart, charStartPosition, charEndPosition );
}

/*
==================
CLexerEmitter::Token
==================
*/
void CLexerEmitter::Token( const LexerState& InLexerState, uint32 InTokenID )
{
	uint32	line				= InLexerState.tokenStart.line + 1;
	uint32	scopeLevel			= InLexerState.scopeLevel;
	uint32	charLineStart		= InLexerState.lineStart.charOffset;
	uint32	charStartPosition	= InLexerState.tokenStart.charOffset;
	uint32	charEndPosition		= InLexerState.currentContext.charOffset;
	const achar* text			= InLexerState.source + InLexerState.tokenStart.byteOffset;
	uint32		 size			= InLexerState.currentContext.byteOffset - InLexerState.tokenStart.byteOffset;
	scriptListener.OnEmitToken( line, InTokenID, std::string( text, size ), scopeLevel, charLineStart, charStartPosition, charEndPosition );
}

/*
==================
CLexerEmitter::Sequence
==================
*/
void CLexerEmitter::Sequence( const LexerState& InLexerState, uint32 InTokenID )
{
	uint32	line				= InLexerState.tokenStart.line + 1;
	uint32	scopeLevel			= InLexerState.scopeLevel;
	uint32	charLineStart		= InLexerState.sequenceLineStart.charOffset;
	uint32	charStartPosition	= InLexerState.sequenceStart.charOffset;
	uint32	charEndPosition		= InLexerState.currentContext.charOffset;

	// Sequences include the surrounding quote marks, so we need to reduce the size of the token text respectively
	const achar* text		= InLexerState.source + InLexerState.sequenceStart.byteOffset + 1;
	uint32		 size		= InLexerState.currentContext.byteOffset - InLexerState.sequenceStart.byteOffset - 2;
	scriptListener.OnEmitToken( line, InTokenID, std::string( text, size ), scopeLevel, charLineStart, charStartPosition, charEndPosition );
}

/*
==================
CLexerEmitter::Error
==================
*/
void CLexerEmitter::Error( const LexerState& InLexerState, const achar* InMessage )
{
	uint32	line				= InLexerState.tokenStart.line + 1;
	uint32	charLineStart		= InLexerState.lineStart.charOffset;
	uint32	charStartPosition	= InLexerState.tokenStart.charOffset;
	uint32	charEndPosition		= InLexerState.currentContext.charOffset;
	const achar* text			= InLexerState.source + InLexerState.tokenStart.byteOffset;
	uint32		 size			= InLexerState.currentContext.byteOffset - InLexerState.tokenStart.byteOffset;

	ScriptFileContext context( fileName, line, charLineStart, charStartPosition, charEndPosition );
	Errorf( TEXT( "%s: %s\n" ), context.ToString().c_str(), ANSI_TO_TCHAR( InMessage ) );
}