/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef LEXERCONTEXT_H
#define LEXERCONTEXT_H

#include "Core.h"

/**
 * @ingroup WorldEd
 * @brief Lexer context
 */
struct LexerContext
{
	/**
	 * @brief Constructor
	 */
	FORCEINLINE LexerContext()
		: line( 0 )
		, byteOffset( 0 )
		, charOffset( 0 )
	{}

	/**
	 * @brief Constructor of copy
	 * @param InOther	Other a LexerContext
	 */
	FORCEINLINE LexerContext( const LexerContext& InOther )
		: line( InOther.line )
		, byteOffset( InOther.byteOffset )
		, charOffset( InOther.charOffset )
	{}

	uint32		line;		/**< The Line that this context refers to */
	uint32		byteOffset;	/**< The associated position in the raw input buffer */
	uint32		charOffset; /**< The associated position in the input buffer resolved as a utf-8 string */
};

#endif // !LEXERCONTEXT_H