/**
 * @file
 * @addtogroup Core Core
 *
 * @authors: Yehor Pohuliaka
 * @date: 4/13/2021
 */

#ifndef BASEARCHIVE_H
#define BASEARCHIVE_H

#include "Misc/Types.h"

/**
 * @ingroup Core
 * @brief The base class for work with file
 */
class BaseArchive
{
public:
	/**
	 * @brief Destructor
	 */
	virtual					~BaseArchive() {}

	/**
	 * @brief Serialize data
	 * 
	 * @param[in] InBuffer Pointer to buffer for serialize
	 * @param[in] InSize Size of buffer
	 */
	virtual void			Serialize( void* InBuffer, uint32 InSize ) {}

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
};

//
// Overloaded operators for serialize in archive
//

/**
 * @brief Overload operator << for serialize TCHAR string
 */
FORCEINLINE BaseArchive&		operator<<( BaseArchive& InArchive, tchar* InStringC )
{
	InArchive.Serialize( InStringC, ( uint32 )wcslen( InStringC ) * 2 );
	return InArchive;
}

/**
 * @brief Overload operator << for serialize ANSI string
 */
FORCEINLINE BaseArchive&		operator<<( BaseArchive& InArchive, achar* InStringC )
{
	InArchive.Serialize( InStringC, ( uint32 )strlen( InStringC ) );
	return InArchive;
}

/**
 * @brief Overload operator << for serialize int32
 */
FORCEINLINE BaseArchive&		operator<<( BaseArchive& InArchive, int32 InValue )
{
	InArchive.Serialize( &InValue, sizeof( int32 ) );
	return InArchive;
}

/**
 * @brief Overload operator << for serialize uint32
 */
FORCEINLINE BaseArchive&		operator<<( BaseArchive& InArchive, uint32 InValue )
{
	InArchive.Serialize( &InValue, sizeof( uint32 ) );
	return InArchive;
}

#endif // !BASEARCHIVE_H
