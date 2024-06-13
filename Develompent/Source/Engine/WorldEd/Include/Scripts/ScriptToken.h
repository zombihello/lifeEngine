/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef SCRIPTTOKEN_H
#define SCRIPTTOKEN_H

#include "Misc/StringConv.h"
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
		, scopeLevel( -1 )
	{}

	/**
	 * @brief Constructor
	 * 
	 * @param InTokenString		Token string
	 * @param InTokenID			Token ID
	 * @param InScopeLevel		Scope level (bracket level)
	 * @param InContext			Token context
	 */
	FORCEINLINE ScriptToken( const std::string& InTokenString, int32 InTokenID, int32 InScopeLevel, const ScriptFileContext& InContext )
		: tokenString( InTokenString )
		, tokenID( InTokenID )
		, scopeLevel( InScopeLevel )
		, context( InContext )
	{}

	/**
	 * @brief Constructor of copy
	 * @param InOther	Other script token
	 */
	FORCEINLINE ScriptToken( const ScriptToken& InOther )
		: tokenString( InOther.tokenString )
		, tokenID( InOther.tokenID )
		, scopeLevel( InOther.scopeLevel )
		, context( InOther.context )
	{}

	/**
	 * @brief Constructor of move
	 * @param InOther	Other script token
	 */
	FORCEINLINE ScriptToken( ScriptToken&& InOther )
		: tokenString( std::move( InOther.tokenString ) )
		, tokenID( std::move( InOther.tokenID ) )
		, scopeLevel( std::move( InOther.scopeLevel ) )
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
		tokenString = InOther.tokenString;
		tokenID = InOther.tokenID;
		scopeLevel = InOther.scopeLevel;
		context = InOther.context;
		return *this;
	}

	/**
	 * @brief Convert the token to string
	 * @return Return converted the token to string
	 */
	FORCEINLINE std::wstring ToString() const
	{
		return L_Sprintf( TEXT( "Token %i \"%hs\", #%i at %hs" ), tokenID, !tokenString.empty() ? ANSI_TO_TCHAR( tokenString.data() ) : TEXT( "<TEXT_EMPTY>" ), scopeLevel, context.ToString().c_str() );
	}

	std::string				tokenString;	/**< Token string */
	int32					tokenID;		/**< Token ID */
	int32					scopeLevel;		/**< Scope level (bracket level) */
	ScriptFileContext		context;		/**< Token context */
};

#endif // !SCRIPTTOKEN_H