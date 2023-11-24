#include "Logger/LoggerMacros.h"
#include "Scripts/LexerEmitter.h"

/*
==================
CLexerEmitter::CLexerEmitter
==================
*/
CLexerEmitter::CLexerEmitter( CScriptTokenEater* InScriptListener, const std::wstring& InFileName )
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
	uint32	line			= InLexerState.tokenStart.line + 1;
	uint32	lineStart		= InLexerState.sectionLineStart.charOffset;
	uint32	level			= InLexerState.scopeLevel;
	uint32	startPosition	= InLexerState.sequenceStart.charOffset;
	uint32	endPosition		= InLexerState.currentContext.charOffset;

	// Sequences include the surrounding quote marks, so we need to reduce the size of the token text respectively
	const achar* text		= InLexerState.source + InLexerState.sequenceStart.byteOffset + 1;
	uint32		 size		= InLexerState.currentContext.byteOffset - InLexerState.sequenceStart.byteOffset - 2;
	scriptListener->OnEmitComment( line, lineStart, level, std::string( text, size ), startPosition, endPosition );
}

/*
==================
CLexerEmitter::Token
==================
*/
void CLexerEmitter::Token( const LexerState& InLexerState, uint32 InTokenID )
{
	uint32	line			= InLexerState.tokenStart.line + 1;
	uint32	lineStart		= InLexerState.lineStart.charOffset;
	uint32	level			= InLexerState.scopeLevel;
	uint32	startPosition	= InLexerState.tokenStart.charOffset;
	uint32	endPosition		= InLexerState.currentContext.charOffset;

	const achar* text		= InLexerState.source + InLexerState.tokenStart.byteOffset;
	uint32		 size		= InLexerState.currentContext.byteOffset - InLexerState.tokenStart.byteOffset;
	scriptListener->OnEmitToken( line, lineStart, InTokenID, level, std::string( text, size ), startPosition, endPosition );
}

/*
==================
CLexerEmitter::Sequence
==================
*/
void CLexerEmitter::Sequence( const LexerState& InLexerState, uint32 InTokenID )
{
	uint32	line			= InLexerState.tokenStart.line + 1;
	uint32	lineStart		= InLexerState.sectionLineStart.charOffset;
	uint32	level			= InLexerState.scopeLevel;
	uint32	startPosition	= InLexerState.sequenceStart.charOffset;
	uint32	endPosition		= InLexerState.currentContext.charOffset;

	// Sequences include the surrounding quote marks, so we need to reduce the size of the token text respectively
	const achar* text		= InLexerState.source + InLexerState.sequenceStart.byteOffset + 1;
	uint32		 size		= InLexerState.currentContext.byteOffset - InLexerState.sequenceStart.byteOffset - 2;
	scriptListener->OnEmitToken( line, lineStart, InTokenID, level, std::string( text, size ), startPosition, endPosition );
}

/*
==================
CLexerEmitter::Error
==================
*/
void CLexerEmitter::Error( const LexerState& InLexerState )
{
	uint32	line			= InLexerState.tokenStart.line + 1;
	uint32	lineStart		= InLexerState.lineStart.charOffset;
	uint32	startPosition	= InLexerState.tokenStart.charOffset;
	uint32	endPosition		= InLexerState.currentContext.charOffset;

	const achar* text		= InLexerState.source + InLexerState.tokenStart.byteOffset;
	uint32		 size		= InLexerState.currentContext.byteOffset - InLexerState.tokenStart.byteOffset;

	ScriptFileContext context( fileName, line, lineStart, startPosition, endPosition );
	Errorf( TEXT( "%s: Syntax error near: %.*s\n" ), context.ToString().c_str(), size, ANSI_TO_TCHAR( text ) );
}