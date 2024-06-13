/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef SCRIPTTOKENSTREAM_H
#define SCRIPTTOKENSTREAM_H

#include <vector>
#include "Scripts/ScriptToken.h"

/**
 * @ingroup WorldEd
 * @brief Script token stream
 */
class CScriptTokenStream
{
public:
	/**
	 * @brief Typedef array of tokens
	 */
	typedef std::vector<ScriptToken>	Tokens_t;

	/**
	 * @brief Constructor
	 */
	CScriptTokenStream();

	/**
	 * @brief Constructor of copy
	 * @param InOther	Other a token stream
	 */
	CScriptTokenStream( const CScriptTokenStream& InOther );

	/**
	 * @brief Extract function tokens
	 * 
	 * @param OutStream		Output stream where will be placed function tokens
	 * @return Return TRUE if function tokens has been successfully extracted, otherwise FALSE
	 */
	bool ExtractFunctionTokens( CScriptTokenStream& OutStream );

	/**
	 * @brief Extract init code
	 *
	 * @param OutStream		Output stream where will be placed init code
	 * @return Return TRUE if function tokens has been successfully extracted, otherwise returns FALSE
	 */
	bool ExtractInitCode( CScriptTokenStream& OutStream );

	/**
	 * @brief Push token at the end of the stream
	 * 
	 * @param InTokenString		Token string
	 * @param InTokenID			Token ID
	 * @param InScopeLevel		Scope level (bracket level)
	 * @param InContext			Token context
	 */
	FORCEINLINE void PushToken( const achar* InTokenString, int32 InTokenID, int32 InScopeLevel, const ScriptFileContext& InContext )
	{
		tokens.emplace_back( InTokenString, InTokenID, InScopeLevel, InContext );
	}

	/**
	 * @brief Push token at the end of the stream
	 * @param InToken	Token
	 */
	FORCEINLINE void PushToken( ScriptToken&& InToken )
	{
		tokens.emplace_back( std::move( InToken ) );
	}

	/**
	 * @brief Pop token
	 * 
	 * @param OutToken	Retrieved token
	 * @return Return TRUE if token has been retrieved, otherwise FALSE (e.g no more tokens)
	 */
	bool PopToken( ScriptToken& OutToken );

	/**
	 * @brief Get read token
	 * @return Return reference to token for read 
	 */
	FORCEINLINE ScriptToken& GetReadToken()
	{
		AssertMsg( !IsEmpty() || !IsEndOfStream(), TEXT( "No more tokens to read" ) );
		return tokens[readPosition];
	}

	/**
	 * @brief Get read token
	 * @return Return reference to token for read
	 */
	FORCEINLINE const ScriptToken& GetReadToken() const
	{
		AssertMsg( !IsEmpty() || !IsEndOfStream(), TEXT( "No more tokens to read" ) );
		return tokens[readPosition];
	}

	/**
	 * @brief Get last token in the stream
	 * @return Return reference to the last token for read
	 */
	FORCEINLINE const ScriptToken& GetLastToken() const
	{
		AssertMsg( !IsEmpty(), TEXT( "No more tokens to read" ) );
		return tokens[tokens.size() - 1];
	}

	/**
	 * @brief Increment read position
	 */
	FORCEINLINE void IncrementReadPosition()
	{
		++readPosition;
	}

	/**
	 * @brief Get array of raw tokens
	 * @return Return array of raw tokens
	 */
	FORCEINLINE const Tokens_t& GetTokens() const 
	{ 
		return tokens; 
	}

	/**
	 * @brief Get array of raw tokens
	 * @return Return array of raw tokens
	 */
	FORCEINLINE Tokens_t& GetTokens() 
	{ 
		return tokens; 
	}

	/**
	 * @brief Is end of stream
	 * @return Return TRUE if we at the end of stream, otherwise FALSE
	 */
	FORCEINLINE bool IsEndOfStream() const 
	{ 
		return readPosition >= tokens.size();
	}

	/**
	 * @brief Is empty stream
	 * @return Return TRUE if stream is empty, otherwise FALSE
	 */
	FORCEINLINE bool IsEmpty() const 
	{ 
		return tokens.empty();
	}

private:
	Tokens_t	tokens;			/**< Array of tokens */
	uint32		readPosition;	/**< Read position */
};

#endif // !SCRIPTTOKENSTREAM_H