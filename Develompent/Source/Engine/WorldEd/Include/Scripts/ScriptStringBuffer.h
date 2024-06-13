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

#include "Misc/RefCounted.h"
#include "Misc/StringTools.h"
#include "Core.h"

/**
 * @ingroup WorldEd
 * @brief Script string buffer (shared string)
 */
template<typename TCharType, class TCharContainerType>
class TScriptStringBuffer
{
public:
	/**
	 * @brief Constructor
	 */
	FORCEINLINE TScriptStringBuffer()
		: buffer( nullptr )
	{}

	/**
	 * @brief Constructor
	 * @param InString	String
	 */
	FORCEINLINE TScriptStringBuffer( const TCharType* InString )
		: buffer( nullptr )
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
	FORCEINLINE TScriptStringBuffer( const TCharContainerType& InString )
		: buffer( nullptr )
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
	FORCEINLINE TScriptStringBuffer( const TScriptStringBuffer& InOther )
		: buffer( InOther.buffer )
	{
		if ( buffer )
		{
			buffer->AddRef();
		}
	}

	/**
	 * @brief Destructor
	 */
	FORCEINLINE ~TScriptStringBuffer()
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
	FORCEINLINE TScriptStringBuffer& operator=( const TScriptStringBuffer& InOther )
	{
		if ( this != &InOther )
		{
			if ( buffer )
			{
				buffer->ReleaseRef();
			}

			buffer = InOther.buffer;

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
	FORCEINLINE TScriptStringBuffer& operator=( const TCharContainerType& InString )
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
		return *this;
	}

	/**
	 * @brief Override operator of copy
	 * 
	 * @param InString	String
	 * @return Return reference to self
	 */
	FORCEINLINE TScriptStringBuffer& operator=( const TCharType* InString )
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
		return *this;
	}

	/**
	 * @brief Override operator of compare
	 * 
	 * @param InOther	Other a script string buffer
	 * @return Return TRUE if two buffers is equal, otherwise returns FALSE
	 */
	FORCEINLINE bool operator==( const TScriptStringBuffer& InOther ) const
	{
		if ( buffer == InOther.buffer )
		{
			return true;
		}

		return !L_Strcmp( AsChar(), InOther.AsChar() );
	}

	/**
	 * @brief Override operator of compare
	 *
	 * @param InString	String
	 * @return Return TRUE if two buffers is equal, otherwise returns FALSE
	 */
	FORCEINLINE bool operator==( const TCharType* InString ) const
	{
		return !L_Strcmp( AsChar(), InString );
	}

	/**
	 * @brief Override operator of compare
	 *
	 * @param InOther	Other a script string buffer
	 * @return Return TRUE if two buffers isn't equal, otherwise returns FALSE
	 */
	FORCEINLINE bool operator!=( const TScriptStringBuffer& InOther ) const
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
};

/*
==================
TScriptStringBuf<achar, std::string>::AsChar
==================
*/
template<>
const achar* TScriptStringBuffer<achar, std::string>::AsChar() const
{
	return buffer ? buffer->text.c_str() : "";
}

/*
==================
TScriptStringBuf<tchar, std::wstring>::AsChar
==================
*/
template<>
const tchar* TScriptStringBuffer<tchar, std::wstring>::AsChar() const
{
	return buffer ? buffer->text.c_str() : TEXT( "" );
}

/**
 * @ingroup WorldEd
 * @brief Typedef of script string buffer for ANSI
 */
typedef TScriptStringBuffer<achar, std::string>					ScriptStringBufferANSI_t;

/**
 * @ingroup WorldEd
 * @brief Typedef of script string buffer for Unicode
 */
typedef TScriptStringBuffer<tchar, std::wstring>				ScriptStringBufferTCHAR_t;

#endif // !SCRIPTSTRINGBUFFER_H