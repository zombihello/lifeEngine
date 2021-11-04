/**
 * @file
 * @addtogroup Core Core
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef Archive_H
#define Archive_H

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
	AT_Scripts,			/**< Archive contains scripts */
	AT_World			/**< Archive contains world */
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
	 * Serialize package header
	 */
	void SerializePackageHeader();

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

/**
 * @brief Overload operator << for serialize TCHAR string
 */
FORCEINLINE FArchive&		operator<<( FArchive& InArchive, tchar* InStringC )
{
	InArchive.Serialize( InStringC, ( uint32 )wcslen( InStringC ) * 2 );
	return InArchive;
}

/**
 * @brief Overload operator << for serialize ANSI string
 */
FORCEINLINE FArchive&		operator<<( FArchive& InArchive, achar* InStringC )
{
	InArchive.Serialize( InStringC, ( uint32 )strlen( InStringC ) );
	return InArchive;
}

/**
 * @brief Overload operator << for all types
 */
template< typename TType >
FORCEINLINE FArchive&		operator<<( FArchive& InArchive, const TType& InValue )
{
	InArchive.Serialize( ( void* ) &InValue, sizeof( InValue ) );
	return InArchive;
}

#endif // !Archive_H
