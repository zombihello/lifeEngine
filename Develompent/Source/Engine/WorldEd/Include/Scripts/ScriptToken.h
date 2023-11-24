/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef SCRIPTTOKEN_H
#define SCRIPTTOKEN_H

#include "Containers/StringConv.h"
#include "Scripts/ScriptFileContext.h"

/**
 * @ingroup WorldEd
 * @brief Parsed file token
 */
struct ScriptToken
{
	/**
	 * @brief Constructor
	 */
	FORCEINLINE ScriptToken()
		: tokenID( 0 )
		, level( -1 )
	{}

	/**
	 * @brief Constructor
	 * 
	 * @param InText	Token text
	 * @param InTokenID	Token ID
	 * @param InLevel	Code level (bracket level)
	 * @param InContext	Token context
	 */
	FORCEINLINE ScriptToken( const std::string& InText, int32 InTokenID, int32 InLevel, const ScriptFileContext& InContext )
		: text( InText )
		, tokenID( InTokenID )
		, level( InLevel )
		, context( InContext )
	{}

	/**
	 * @brief Constructor of copy
	 * @param InOther	Other script token
	 */
	FORCEINLINE ScriptToken( const ScriptToken& InOther )
		: text( InOther.text )
		, tokenID( InOther.tokenID )
		, level( InOther.level )
		, context( InOther.context )
	{}

	/**
	 * @brief Constructor of move
	 * @param InOther	Other script token
	 */
	FORCEINLINE ScriptToken( ScriptToken&& InOther )
		: text( std::move( InOther.text ) )
		, tokenID( std::move( InOther.tokenID ) )
		, level( std::move( InOther.level ) )
		, context( std::move( InOther.context ) )
	{}

	/**
	 * @brief Override operator of copy
	 * 
	 * @param InOther	Other script token
	 * @return Return reference to self
	 */
	FORCEINLINE ScriptToken& operator=( const ScriptToken& InOther )
	{
		text = InOther.text;
		tokenID = InOther.tokenID;
		level = InOther.level;
		context = InOther.context;
		return *this;
	}

	/**
	 * @brief Convert the token to string
	 * @return Return converted the token to string
	 */
	FORCEINLINE std::wstring ToString() const
	{
		return CString::Format( TEXT( "Token %i \"%hs\", #%i at %hs" ), tokenID, ANSI_TO_TCHAR( text.c_str() ), level, context.ToString().c_str() );
	}

	std::string				text;		/**< Token text TODO yehor.pohuliaka: Need to implement CStringView and replace std::string by it for oprimize using of memory */
	int32					tokenID;	/**< Toke ID */
	int32					level;		/**< Code level (bracket level) */
	ScriptFileContext		context;	/**< Token context */
};

#endif // !SCRIPTTOKEN_H