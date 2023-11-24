/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef LEXEREMITTER_H
#define LEXEREMITTER_H

#include "Scripts/LexerListener.h"
#include "Scripts/ScriptTokenEater.h"

/**
 * @ingroup WorldEd
 * @brief Parsed file token
 */
class CLexerEmitter : public CBaseLexerListener
{
public:
	/**
	 * @brief Constructor
	 * 
	 * @param InScriptListener		Script listener
	 * @param InFileName			File name
	 */
	CLexerEmitter( CScriptTokenEater* InScriptListener, const std::wstring& InFileName );

	/**
	 * @brief A Script comment has been encountered (Could be either single or multiline)
	 * @param InLexerState		Lexer state
	 */
	virtual void Comment( const LexerState& InLexerState ) override;

	/**
	 * @brief A Standard token
	 *
	 * @param InLexerState		Lexer state
	 * @param InTokenID			Token ID
	 */
	virtual void Token( const LexerState& InLexerState, uint32 InTokenID ) override;

	/**
	 * @brief A Token denoted by a start and end series of chars (such as a string or name)
	 *
	 * @param InLexerState		Lexer state
	 * @param InTokenID			Token ID
	 */
	virtual void Sequence( const LexerState& InLexerState, uint32 InTokenID ) override;

	/**
	 * @brief A section of script that has failed to match any token rules
	 * @param InLexerState		Lexer state
	 */
	virtual void Error( const LexerState& InLexerState ) override;

private:
	CScriptTokenEater*		scriptListener;	/**< Script listener */
	std::wstring			fileName;		/**< File name */
};

#endif // !LEXEREMITTER_H