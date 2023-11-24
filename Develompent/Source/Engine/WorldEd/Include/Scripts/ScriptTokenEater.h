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
	CScriptTokenEater( CScriptTokenStream& OutTokens, const ScriptStringTCHARBuf_t& InFileName );

	/**
	 * @brief Destructor
	 */
	virtual ~CScriptTokenEater();

	/**
	 * @brief Called when token has been emitted
	 * 
	 * @param InLine		Line where the start of the token emitted
	 * @param InLineStart	Character position at the start of the line
	 * @param InTokenID		Token ID
	 * @param InLevel		The number of opening braces '{' encountered up till now
	 * @param InText		Token in string format
	 * @param InStart		Character position this context begins at
	 * @param InEnd			Character position at the end of the context
	 */
	virtual void OnEmitToken( uint32 InLine, uint32 InLineStart, uint32 InTokenID, uint32 InLevel, const std::string& InText, uint32 InStart, uint32 InEnd );

	/**
	 * @brief Called when comment has been emitted
	 * 
	 * @param InLine		Line where the start of the comment emitted
	 * @param InLineStart	Character position at the start of the line
	 * @param InLevel		The number of opening braces '{' encountered up till now
	 * @param InText		Comment text
	 * @param InStart		Character position this context begins at
	 * @param InEnd			Character position at the end of the context
	 */
	virtual void OnEmitComment( uint32 InLine, uint32 InLineStart, uint32 InLevel, const std::string& InText, uint32 InStart, uint32 InEnd );
	
	/**
	 * @brief Called when has been error
	 * 
	 * @param InLine		Line where the start of the error emitted
	 * @param InMessage		Message of the error
	 */
	virtual void OnEmitError( uint32 InLine, const achar* InMessage );

protected:
	ScriptStringTCHARBuf_t		fileName;	/**< File name for context */
	CScriptTokenStream*			tokens;		/**< Stream tokens. Here will be places all of parsed tokens */
};

#endif // !SCRIPTTOKENEATER_H