/**
 * @file
 * @addtogroup Core Core
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef Archive_H
#define Archive_H

#include <string>

#include "Core.h"
#include "Misc/Types.h"

/**
 * @ingroup Core
 * Enumeration of type archive
 */
enum EArchiveType
{
	AT_TextFile,		/**< Archive is text file */
	AT_ShaderCache,		/**< Archive contains shader cache */
	AT_TextureCache,	/**< Archive contains texture cache */
	AT_World,			/**< Archive contains world */
	AT_Package			/**< Archive contains assets */
};

/**
 * @ingroup Core
 * @brief The base class for work with archive
 */
class FArchive
{
public:
	/**
	 * Constructor
	 */
	FArchive();

	/**
	 * @brief Destructor
	 */
	virtual					~FArchive() {}

	/**
	 * @brief Serialize data
	 * 
	 * @param[in] InBuffer Pointer to buffer for serialize
	 * @param[in] InSize Size of buffer
	 */
	virtual void			Serialize( void* InBuffer, uint32 InSize ) {}

	/**
	 * Serialize archive header
	 */
	void SerializeHeader();

	/**
	 * @brief Get current position in archive
	 * @return Current position in archive
	 */
	virtual uint32			Tell() { return 0; };

	/**
	 * @brief Set current position in archive
	 * 
	 * @param[in] InPosition New position in archive
	 */
	virtual void			Seek( uint32 InPosition ) {}

	/**
	 * @brief Flush data
	 */
	virtual void			Flush() {}

	/**
	 * Set archive type
	 * 
	 * @param[in] InType Archive type
	 */
	FORCEINLINE void SetType( EArchiveType InType )
	{
		arType = InType;
	}

	/**
	 * @brief Is saving archive
	 * @return True if archive saving, false if archive loading
	 */
	virtual bool			IsSaving() const { return false; }

	/**
	 * @breif Is loading archive
	 * @return True if archive loading, false if archive saving
	 */
	virtual bool			IsLoading() const { return false; }

	/**
	 * Is end of file
	 * @return Return true if end of file, else return false
	 */
	virtual bool			IsEndOfFile() { return false; }

	/**
	 * @brief Get size of archive
	 * @return Size of archive
	 */
	virtual uint32			GetSize() { return 0; }

	/**
	 * Get archive version
	 * @return Return archive version
	 */
	FORCEINLINE uint32		Ver() const
	{
		return arVer;
	}

	/**
	 * Get archive type
	 * @return Return type archive
	 */
	FORCEINLINE EArchiveType Type() const
	{
		return arType;
	}

protected:
	uint32					arVer;		/**< Archive version (look ELifeEnginePackageVersion) */
	EArchiveType			arType;		/**< Archive type */
};

//
// Overloaded operators for serialize in archive
//

FORCEINLINE FArchive& operator<<( FArchive& InArchive, int8& InValue )
{
	InArchive.Serialize( &InValue, sizeof( InValue ) );
	return InArchive;
}

FORCEINLINE FArchive& operator<<( FArchive& InArchive, const int8& InValue )
{
	check( InArchive.IsSaving() );
	InArchive.Serialize( ( void* ) &InValue, sizeof( InValue ) );
	return InArchive;
}

FORCEINLINE FArchive& operator<<( FArchive& InArchive, uint8& InValue )
{
	InArchive.Serialize( &InValue, sizeof( InValue ) );
	return InArchive;
}

FORCEINLINE FArchive& operator<<( FArchive& InArchive, const uint8& InValue )
{
	check( InArchive.IsSaving() );
	InArchive.Serialize( ( void* ) &InValue, sizeof( InValue ) );
	return InArchive;
}

FORCEINLINE FArchive& operator<<( FArchive& InArchive, int16& InValue )
{
	InArchive.Serialize( &InValue, sizeof( InValue ) );
	return InArchive;
}

FORCEINLINE FArchive& operator<<( FArchive& InArchive, const int16& InValue )
{
	check( InArchive.IsSaving() );
	InArchive.Serialize( ( void* ) &InValue, sizeof( InValue ) );
	return InArchive;
}

FORCEINLINE FArchive& operator<<( FArchive& InArchive, uint16& InValue )
{
	InArchive.Serialize( &InValue, sizeof( InValue ) );
	return InArchive;
}

FORCEINLINE FArchive& operator<<( FArchive& InArchive, const uint16& InValue )
{
	check( InArchive.IsSaving() );
	InArchive.Serialize( ( void* ) &InValue, sizeof( InValue ) );
	return InArchive;
}

FORCEINLINE FArchive& operator<<( FArchive& InArchive, int32& InValue )
{
	InArchive.Serialize( &InValue, sizeof( InValue ) );
	return InArchive;
}

FORCEINLINE FArchive& operator<<( FArchive& InArchive, const int32& InValue )
{
	check( InArchive.IsSaving() );
	InArchive.Serialize( ( void* ) &InValue, sizeof( InValue ) );
	return InArchive;
}

FORCEINLINE FArchive& operator<<( FArchive& InArchive, uint32& InValue )
{
	InArchive.Serialize( &InValue, sizeof( InValue ) );
	return InArchive;
}

FORCEINLINE FArchive& operator<<( FArchive& InArchive, const uint32& InValue )
{
	check( InArchive.IsSaving() );
	InArchive.Serialize( ( void* ) &InValue, sizeof( InValue ) );
	return InArchive;
}

FORCEINLINE FArchive& operator<<( FArchive& InArchive, bool& InValue )
{
	InArchive.Serialize( &InValue, sizeof( InValue ) );
	return InArchive;
}

FORCEINLINE FArchive& operator<<( FArchive& InArchive, const bool& InValue )
{
	check( InArchive.IsSaving() );
	InArchive.Serialize( ( void* ) &InValue, sizeof( InValue ) );
	return InArchive;
}

FORCEINLINE FArchive& operator<<( FArchive& InArchive, float& InValue )
{
	InArchive.Serialize( &InValue, sizeof( InValue ) );
	return InArchive;
}

FORCEINLINE FArchive& operator<<( FArchive& InArchive, const float& InValue )
{
	check( InArchive.IsSaving() );
	InArchive.Serialize( ( void* ) &InValue, sizeof( InValue ) );
	return InArchive;
}

FORCEINLINE FArchive& operator<<( FArchive& InArchive, EArchiveType& InValue )
{
	InArchive.Serialize( &InValue, sizeof( InValue ) );
	return InArchive;
}

FORCEINLINE FArchive& operator<<( FArchive& InArchive, const EArchiveType& InValue )
{
	check( InArchive.IsSaving() );
	InArchive.Serialize( ( void* ) &InValue, sizeof( InValue ) );
	return InArchive;
}

FORCEINLINE FArchive& operator<<( FArchive& InArchive, std::string& InValue )
{
	uint32		stringSize = InValue.size();
	InArchive << stringSize;

	if ( stringSize > 0 )
	{
		if ( InArchive.IsLoading() )
		{
			InValue.resize( stringSize );
		}

		InArchive.Serialize( InValue.data(), stringSize );
	}

	return InArchive;
}

FORCEINLINE FArchive& operator<<( FArchive& InArchive, const std::string& InValue )
{
	check( InArchive.IsSaving() );
	uint32		stringSize = InValue.size();
	InArchive << stringSize;

	if ( stringSize > 0 )
	{
		InArchive.Serialize( ( void* )InValue.data(), stringSize );
	}

	return InArchive;
}

FORCEINLINE FArchive& operator<<( FArchive& InArchive, std::wstring& InValue )
{
	uint32		stringSize = InValue.size() * sizeof( std::wstring::value_type );
	InArchive << stringSize;

	if ( stringSize > 0 )
	{
		if ( InArchive.IsLoading() )
		{
			InValue.resize( stringSize / sizeof( std::wstring::value_type ) );
		}

		InArchive.Serialize( InValue.data(), stringSize );
	}

	return InArchive;
}

FORCEINLINE FArchive& operator<<( FArchive& InArchive, const std::wstring& InValue )
{
	check( InArchive.IsSaving() );
	uint32		stringSize = InValue.size() * sizeof( std::wstring::value_type );
	InArchive << stringSize;

	if ( stringSize > 0 )
	{
		InArchive.Serialize( ( void* )InValue.data(), stringSize );
	}

	return InArchive;
}

#endif // !Archive_H
