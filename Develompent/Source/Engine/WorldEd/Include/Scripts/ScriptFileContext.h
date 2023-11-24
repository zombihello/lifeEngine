/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef SCRIPTFILECONTEXT_H
#define SCRIPTFILECONTEXT_H

#include "Containers/String.h"
#include "Scripts/ScriptStringBuffer.h"

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
		, lineStart( 0 )
		, position( 0 )
		, endPosition( 0 )
	{}

	/**
	 * @brief Constructor
	 * 
	 * @param InFile		Referenced file
	 * @param InLine		Line this context starts on
	 * @param InLineStart	Character position at the start of the line
	 * @param InPosition	Character position this context begins at
	 * @param InEndPosition	Character position at the end of the context
	 */
	FORCEINLINE ScriptFileContext( const ScriptStringTCHARBuf_t& InFile, int32 InLine, int32 InLineStart, int32 InPosition, int32 InEndPosition )
		: file( InFile )
		, line( InLine )
		, lineStart( InLineStart )
		, position( InPosition )
		, endPosition( InEndPosition )
	{}

	/**
	 * @brief Constructor
	 * 
	 * @param InFile		Referenced file
	 * @param InLine		Line this context starts on
	 */
	FORCEINLINE ScriptFileContext( const ScriptStringTCHARBuf_t& InFile, int32 InLine )
		: ScriptFileContext( InFile, InLine, 0, 0, 0 )
	{}

	/**
	 * @brief Constructor of copy
	 * @param InOther	Other a script file context
	 */
	FORCEINLINE ScriptFileContext( const ScriptFileContext& InOther )
		: file( InOther.file )
		, line( InOther.line )
		, lineStart( InOther.lineStart )
		, position( InOther.position )
		, endPosition( InOther.endPosition )
	{}

	/**
	 * @brief Constructor of move
	 * @param InOther	Other a script file context
	 */
	FORCEINLINE ScriptFileContext( ScriptFileContext&& InOther )
		: file( std::move( InOther.file ) )
		, line( std::move( InOther.line ) )
		, lineStart( std::move( InOther.lineStart ) )
		, position( std::move( InOther.position ) )
		, endPosition( std::move( InOther.endPosition ) )
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
			file = InOther.file;
			line = InOther.line;
			lineStart = InOther.lineStart;
			position = InOther.position;
			endPosition = InOther.endPosition;
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
			file = std::move( InOther.file );
			line = std::move( InOther.line );
			lineStart = std::move( InOther.lineStart );
			position = std::move( InOther.position );
			endPosition = std::move( InOther.endPosition );
		}
		return *this;
	}

	/**
	 * @brief Convert the context to string
	 * @return Return converted the context to string
	 */
	FORCEINLINE std::wstring ToString() const
	{
		if ( !file.IsEmpty() )
		{
			return CString::Format( TEXT( "%s[%i]" ), file.AsChar(), line );
		}
		else
		{
			return TEXT( "Unknown" );
		}
	}

	ScriptStringTCHARBuf_t		file;			/**< Referenced file */
	ScriptStringTCHARBuf_t		function;		/**< Function name */
	int32						line;			/**< Line this context starts on */
	int32						lineStart;		/**< Character position at the start of the line */
	int32						position;		/**< Character position this context begins at */
	int32						endPosition;	/**< Character position at the end of the context */
};

#endif // !SCRIPTFILECONTEXT_H