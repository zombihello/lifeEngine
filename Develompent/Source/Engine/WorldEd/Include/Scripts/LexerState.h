/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef LEXERSTATE_H
#define LEXERSTATE_H

#include "Scripts/LexerContext.h"

 /**
  * @ingroup WorldEd
  * @brief Used to manage the state of the lexer internally
  */
struct LexerState
{
public:
	/**
	 * @brief Constructor
	 * @param InSource	The raw input buffer
	 */
	FORCEINLINE LexerState( const achar* InSource )
		: source( InSource )
		, scopeLevel( 0 )
	{}

	/**
	 * @brief Constructor of copy
	 * @param InOther	Other a LexerState
	 */
	FORCEINLINE LexerState( const LexerState& InOther )
		: source( InOther.source )
		, currentContext( InOther.currentContext )
		, tokenStart( InOther.tokenStart )
		, sequenceStart( InOther.sequenceStart )
		, lineStart( InOther.lineStart )
		, sectionLineStart( InOther.sectionLineStart )
		, scopeLevel( InOther.scopeLevel )
	{}

	const achar*	source;				/**< The raw input buffer */
	LexerContext	currentContext;		/**< The current context of the lexer. Usually this will be synonymous with the "end" of the token emitted */
	LexerContext	tokenStart;			/**< The start of the token emitted */
	LexerContext	sequenceStart;		/**< The start of the sequence emitted. (a sequence being a token enclosed in a header/footer, such as a "string" or 'name') */
	LexerContext	lineStart;			/**< The context of the current line, at column 0 */
	LexerContext	sectionLineStart;	/**< The context of the line at the start of the section, at column 0 */
	uint32			scopeLevel;			/**< The number of opening braces '{' encountered up till now */
};

#endif // !LEXERSTATE_H