/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef SCRIPTTOKENEATER_H
#define SCRIPTTOKENEATER_H

#include "Scripts/ScriptTokenStream.h"
#include "Scripts/ScriptStringBuffer.h"

/**
 * @ingroup WorldEd
 * @brief Connection interface between lexer and the file parser
 */
class CScriptTokenEater
{
public:
	/**
	 * @brief Constructor
	 * 
	 * @param OutTokens		Stream tokens. Here will be places all of parsed tokens
	 * @param InFileName	File name for context
	 */
	CScriptTokenEater( CScriptTokenStream& OutTokens, const std::wstring& InFileName );

	/**
	 * @brief Called when token has been emitted
	 * 
	 * @param InLine			Line where the start of the token emitted
	 * @param InTokenID			Token ID
	 * @param InTokenString		Token in string format
	 * @param InScopeLevel		The number of opening braces '{' encountered up till now
	 * @param InCharLineStart	Character position at the start of the line
	 * @param InCharStart		Character position this context begins at
	 * @param InCharEnd			Character position at the end of the context
	 */
	virtual void OnEmitToken( uint32 InLine, uint32 InTokenID, const std::string& InTokenString, uint32 InScopeLevel, uint32 InCharLineStart, uint32 InCharStart, uint32 InCharEnd );

	/**
	 * @brief Called when comment has been emitted
	 * 
	 * @param InLine			Line where the start of the comment emitted
	 * @param InText			Comment text
	 * @param InScopeLevel		The number of opening braces '{' encountered up till now
	 * @param InCharLineStart	Character position at the start of the line
	 * @param InCharStart		Character position this context begins at
	 * @param InCharEnd			Character position at the end of the context
	 */
	virtual void OnEmitComment( uint32 InLine, const std::string& InText, uint32 InScopeLevel, uint32 InCharLineStart, uint32 InCharStart, uint32 InCharEnd );
	
	/**
	 * @brief Called when has been error
	 * 
	 * @param InLine		Line where the start of the error emitted
	 * @param InMessage		Error message
	 */
	virtual void OnEmitError( uint32 InLine, const achar* InMessage );

protected:
	std::wstring				fileName;	/**< File name for context */
	CScriptTokenStream&			tokens;		/**< Stream tokens. Here will be places all of parsed tokens */
};

#endif // !SCRIPTTOKENEATER_H