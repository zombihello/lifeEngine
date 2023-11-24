/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef SCRIPTSTRINGBUFFER_H
#define SCRIPTSTRINGBUFFER_H

#include <string>

#include "Core.h"
#include "Misc/RefCounted.h"

/**
 * @ingroup WorldEd
 * @brief Script string buffer (shared string)
 */
template<typename TCharType, class TCharContainerType>
class TScriptStringBuf
{
public:
	/**
	 * @brief Constructor
	 */
	FORCEINLINE TScriptStringBuf()
		: buffer( nullptr )
		, bOwned( true )
	{}

	/**
	 * @brief Constructor
	 * @param InString	String
	 */
	FORCEINLINE TScriptStringBuf( const TCharType* InString )
		: buffer( nullptr )
		, bOwned( true )
	{
		if ( InString && *InString )
		{
			buffer = new Buffer( InString );
			buffer->AddRef();
		}
	}

	/**
	 * @brief Constructor
	 * @param InString	String
	 */
	FORCEINLINE TScriptStringBuf( const TCharContainerType& InString )
		: buffer( nullptr )
		, bOwned( true )
	{
		if ( !InString.empty() )
		{
			buffer = new Buffer( InString );
			buffer->AddRef();
		}
	}

	/**
	 * @brief Constructor of copy
	 * @param InOther	Other a script string buffer
	 */
	FORCEINLINE TScriptStringBuf( const TScriptStringBuf& InOther )
		: buffer( InOther.buffer )
		, bOwned( false )
	{
		if ( buffer )
		{
			buffer->AddRef();
		}
	}

	/**
	 * @brief Destructor
	 */
	FORCEINLINE ~TScriptStringBuf()
	{
		if ( buffer )
		{
			buffer->ReleaseRef();
		}
	}

	/**
	 * @brief Clear the buffer
	 */
	FORCEINLINE void Clear()
	{
		if ( buffer )
		{
			buffer->ReleaseRef();
		}

		buffer = nullptr;
		bOwned = false;
	}

	/**
	 * @brief Get the buffer as array of chars
	 * @retrun Return pointer to first char in the buffer. If buffer is empty returns NULL
	 */
	FORCEINLINE const TCharType* AsChar() const;

	/**
	 * @brief Is empty the buffer
	 * @return Return TRUE if the buffer is empty, otherwise returns FALSE
	 */
	FORCEINLINE bool IsEmpty() const
	{
		return buffer;
	}

	/**
	 * @brief Override operator of copy
	 * 
	 * @param InOther	Other a script string buffer
	 * @return Return reference to self
	 */
	FORCEINLINE TScriptStringBuf& operator=( const TScriptStringBuf& InOther )
	{
		if ( this != &InOther )
		{
			if ( buffer )
			{
				buffer->ReleaseRef();
			}

			buffer = InOther.buffer;
			bOwned = false;

			if ( buffer )
			{
				buffer->AddRef();
			}
		}
		return *this;
	}

	/**
	 * @brief Override operator of copy
	 * 
	 * @param InString	String
	 * @return Return reference to self
	 */
	FORCEINLINE TScriptStringBuf& operator=( const TCharContainerType& InString )
	{
		if ( buffer )
		{
			buffer->ReleaseRef();
		}

		if ( !InString.empty() )
		{
			buffer = new Buffer( InString );
			buffer->AddRef();
		}
		else
		{
			buffer = nullptr;
		}

		bOwned = true;
		return *this;
	}

	/**
	 * @brief Override operator of copy
	 * 
	 * @param InString	String
	 * @return Return reference to self
	 */
	FORCEINLINE TScriptStringBuf& operator=( const TCharType* InString )
	{
		if ( buffer )
		{
			buffer->ReleaseRef();
		}

		if ( InString && *InString )
		{
			buffer = new Buffer( InString );
			buffer->AddRef();
		}
		else
		{
			buffer = nullptr;
		}

		bOwned = true;
		return *this;
	}

	/**
	 * @brief Override operator of compare
	 * 
	 * @param InOther	Other a script string buffer
	 * @return Return TRUE if two buffers is equal, otherwise returns FALSE
	 */
	FORCEINLINE bool operator==( const TScriptStringBuf& InOther ) const
	{
		if ( buffer == InOther.buffer )
		{
			return true;
		}

		return !strcmp( AsChar(), InOther.AsChar() );
	}

	/**
	 * @brief Override operator of compare
	 *
	 * @param InString	String
	 * @return Return TRUE if two buffers is equal, otherwise returns FALSE
	 */
	FORCEINLINE bool operator==( const TCharType* InString ) const
	{
		return !strcmp( AsChar(), InString ? InString : EmptyString );
	}

	/**
	 * @brief Override operator of compare
	 *
	 * @param InOther	Other a script string buffer
	 * @return Return TRUE if two buffers isn't equal, otherwise returns FALSE
	 */
	FORCEINLINE bool operator!=( const TScriptStringBuf& InOther ) const
	{
		return !( operator==( InOther ) );
	}

	/**
	 * @brief Override operator of compare
	 *
	 * @param InString	String
	 * @return Return TRUE if two buffers isn't equal, otherwise returns FALSE
	 */
	FORCEINLINE bool operator!=( const TCharType* InString ) const
	{
		return !( operator!=( InString ) );
	}

private:
	/**
	 * @brief Buffer
	 */
	struct Buffer : public CRefCounted
	{
		/**
		 * @brief Constructor
		 * @param InString	String
		 */
		FORCEINLINE Buffer( const TCharContainerType& InString )
			: text( InString )
		{}

		/**
		 * @brief Constructor
		 * @param InString	String
		 */
		FORCEINLINE Buffer( const TCharType* InString )
			: text( InString )
		{}

		TCharContainerType		text;	/**< String */
	};

	Buffer*		buffer;		/**< String buffer */
	bool		bOwned;		/**< Is this buffer is owner of allocated buffer */
};

/*
==================
TScriptStringBuf<achar, std::string>::AsChar
==================
*/
template<>
const achar* TScriptStringBuf<achar, std::string>::AsChar() const
{
	return buffer ? buffer->text.c_str() : "";
}

/*
==================
TScriptStringBuf<tchar, std::wstring>::AsChar
==================
*/
template<>
const tchar* TScriptStringBuf<tchar, std::wstring>::AsChar() const
{
	return buffer ? buffer->text.c_str() : TEXT( "" );
}

/**
 * @ingroup WorldEd
 * @brief Typedef of script string buffer for ANSI
 */
typedef TScriptStringBuf<achar, std::string>				ScriptStringANSIBuf_t;

/**
 * @ingroup WorldEd
 * @brief Typedef of script string buffer for Unicode
 */
typedef TScriptStringBuf<tchar, std::wstring>				ScriptStringTCHARBuf_t;

#endif // !SCRIPTSTRINGBUFFER_H