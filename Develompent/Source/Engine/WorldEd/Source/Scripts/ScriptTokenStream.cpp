#include "Scripts/ScriptTokenStream.h"

/*
==================
CScriptTokenStream::CScriptTokenStream
==================
*/
CScriptTokenStream::CScriptTokenStream()
	: readPosition( 0 )
{}

/*
==================
CScriptTokenStream::CScriptTokenStream
==================
*/
CScriptTokenStream::CScriptTokenStream( const CScriptTokenStream& InOther )
	: tokens( InOther.tokens )
	, readPosition( InOther.readPosition )
{}

/*
==================
CScriptTokenStream::PopToken
==================
*/
bool CScriptTokenStream::PopToken( ScriptToken& OutToken )
{
	// Get token
	if ( readPosition < tokens.size() )
	{
		OutToken = std::forward<ScriptToken>( tokens[readPosition] );
		++readPosition;
		return true;
	}

	// No more tokens
	return false;
}

/*
==================
CScriptTokenStream::ExtractFunctionTokens
==================
*/
bool CScriptTokenStream::ExtractFunctionTokens( CScriptTokenStream& OutStream )
{
	// Make sure we are not on the top
	if ( readPosition == 0 )
	{
		return false;
	}

	// Step back unless we are just after the bracket
	if ( tokens[readPosition - 1].tokenID != '{' )
	{
		--readPosition;
	}

	// Level counter
	uint32	currentLevel = 1;

	// Extract tokens
	ScriptToken		token;
	while ( true )
	{
		// Pop token
		if ( !PopToken( token ) )
		{
			break;
		}

		// Exiting bracket
		if ( token.tokenID == '}' )
		{
			--currentLevel;
		}

		// Entering bracket
		else if ( token.tokenID == '{' )
		{
			++currentLevel;
		}

		// End of block?
		if ( token.tokenID == '}' && currentLevel == 0 )
		{
			// We need to unparse the bracket
			--readPosition;
			return true;
		}

		// Push token to output stream
		OutStream.PushToken( std::forward<ScriptToken>( token ) );
	}

	// Error, not able to extract code from current block
	return false;
}

/*
==================
CScriptTokenStream::ExtractInitCode
==================
*/
bool CScriptTokenStream::ExtractInitCode( CScriptTokenStream& OutStream )
{
	// Make sure we are not on the top
	if ( readPosition == 0 )
	{
		return false;
	}

	// Step back unless we are just before the assignment '=' sign
	if ( tokens[readPosition - 1].tokenID != '=' )
	{
		--readPosition;
	}

	// Include '=' sign in tokens
	Assert( tokens[readPosition - 1].tokenID == '=' );
	--readPosition;

	// Worst case - realloc to no of tokens stored on internal stream
	if ( OutStream.tokens.empty() )
	{
		OutStream.tokens.reserve( tokens.size() );
	}

	// Extract tokens
	ScriptToken token;
	int32		tokenID;
	do
	{
		// Pop token
		if ( !PopToken( token ) )
		{
			// Error, not able to extract code from current block
			return false;
		}

		// Push token to output stream
		tokenID = token.tokenID;
		OutStream.PushToken( std::forward<ScriptToken>( token ) );

	}
	while ( tokenID != ';' );

	// We need to unparse the semicolon
	--readPosition;
	return true;
}
