/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef SCRIPTFILECONTEXT_H
#define SCRIPTFILECONTEXT_H

#include "Misc/StringTools.h"

/**
 * @ingroup WorldEd
 * @brief File context info for script data
 */
struct ScriptFileContext
{
	/**
	 * @brief Constructor
	 */
	FORCEINLINE ScriptFileContext()
		: line( 0 )
		, charLineStart( 0 )
		, charPosition( 0 )
		, charEndPosition( 0 )
	{}

	/**
	 * @brief Constructor
	 * 
	 * @param InFile				Referenced file
	 * @param InLine				Line this context starts on
	 * @param InCharLineStart		Character position at the start of the line
	 * @param InCharPosition		Character position this context begins at
	 * @param InCharEndPosition		Character position at the end of the context
	 */
	FORCEINLINE ScriptFileContext( const std::wstring& InFile, int32 InLine, int32 InCharLineStart, int32 InCharPosition, int32 InCharEndPosition )
		: file( InFile )
		, line( InLine )
		, charLineStart( InCharLineStart )
		, charPosition( InCharPosition )
		, charEndPosition( InCharEndPosition )
	{}

	/**
	 * @brief Constructor
	 * 
	 * @param InFile		Referenced file
	 * @param InLine		Line this context starts on
	 */
	FORCEINLINE ScriptFileContext( const std::wstring& InFile, int32 InLine )
		: ScriptFileContext( InFile, InLine, 0, 0, 0 )
	{}

	/**
	 * @brief Constructor of copy
	 * @param InOther	Other a script file context
	 */
	FORCEINLINE ScriptFileContext( const ScriptFileContext& InOther )
		: file( InOther.file )
		, line( InOther.line )
		, charLineStart( InOther.charLineStart )
		, charPosition( InOther.charPosition )
		, charEndPosition( InOther.charEndPosition )
	{}

	/**
	 * @brief Constructor of move
	 * @param InOther	Other a script file context
	 */
	FORCEINLINE ScriptFileContext( ScriptFileContext&& InOther )
		: file( std::move( InOther.file ) )
		, line( std::move( InOther.line ) )
		, charLineStart( std::move( InOther.charLineStart ) )
		, charPosition( std::move( InOther.charPosition ) )
		, charEndPosition( std::move( InOther.charEndPosition ) )
	{}

	/**
	 * @brief Override operator of copy
	 * 
	 * @param InOther	Other a script file context
	 * @return Return reference to self
	 */
	FORCEINLINE ScriptFileContext& operator=( const ScriptFileContext& InOther )
	{
		if ( this != &InOther )
		{
			file			= InOther.file;
			line			= InOther.line;
			charLineStart	= InOther.charLineStart;
			charPosition	= InOther.charPosition;
			charEndPosition = InOther.charEndPosition;
		}
		return *this;
	}

	/**
	 * @brief Override operator of copy (move version)
	 *
	 * @param InOther	Other a script file context
	 * @return Return reference to self
	 */
	FORCEINLINE ScriptFileContext& operator=( ScriptFileContext&& InOther )
	{
		if ( this != &InOther )
		{
			file			= std::move( InOther.file );
			line			= std::move( InOther.line );
			charLineStart	= std::move( InOther.charLineStart );
			charPosition	= std::move( InOther.charPosition );
			charEndPosition	= std::move( InOther.charEndPosition );
		}
		return *this;
	}

	/**
	 * @brief Convert the context to string
	 * @return Return converted the context to string
	 */
	FORCEINLINE std::wstring ToString() const
	{
		if ( !file.empty() )
		{
			return L_Sprintf( TEXT( "%s[%i]" ), file.data(), line );
		}
		else
		{
			return TEXT( "Unknown" );
		}
	}

	std::wstring			file;				/**< Referenced file */
	int32					line;				/**< Line this context starts on */
	int32					charLineStart;		/**< Character position at the start of the line */
	int32					charPosition;		/**< Character position this context begins at */
	int32					charEndPosition;	/**< Character position at the end of the context */
};

#endif // !SCRIPTFILECONTEXT_H