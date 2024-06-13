/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef LEXERLISTENER_H
#define LEXERLISTENER_H

#include "Scripts/LexerState.h"

/**
 * @ingroup WorldEd
 * @brief Base class of lexer listener
 * 
 * You need to create a subclass of CBaseLexerListener in
 * order to receive the tokenised output of the lexer
 */
class CBaseLexerListener
{
public:
	/**
	 * @brief A Script comment has been encountered (Could be either single or multiline)
	 * @param InLexerState		Lexer state
	 */
	virtual void Comment( const LexerState& InLexerState ) {}

	/**
	 * @brief A Standard token
	 * 
	 * @param InLexerState		Lexer state
	 * @param InTokenID			Token ID
	 */
	virtual void Token( const LexerState& InLexerState, uint32 InTokenID ) {}

	/**
	 * @brief A Token denoted by a start and end series of chars (such as a string or name)
	 *
	 * @param InLexerState		Lexer state
	 * @param InTokenID			Token ID
	 */
	virtual void Sequence( const LexerState& InLexerState, uint32 InTokenID ) {}
	
	/**
	 * @brief A section of script that has failed to match any token rules
	 *
	 * @param InLexerState		Lexer state
	 * @param InMessage			Error message
	 */
	virtual void Error( const LexerState& InLexerState, const achar* InMessage ) {}
};

#endif // !LEXERLISTENER_H