/**
 * @file
 * @addtogroup Core Core
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef NAME_H
#define NAME_H

#include <string>

#include "Misc/Types.h"
#include "Misc/SharedPointer.h"
#include "System/Archive.h"
#include "Core.h"

/**
 * @ingroup Core
 * @brief Class for containing IDs in name view.
 * Names are case-insensitive
 */
class CName
{
public:
	/**
	 * @brief Struct of containing name entry in global table
	 */
	struct NameEntry
	{
		/**
		 * @brief Constructor
		 */
		FORCEINLINE NameEntry()
			: hash( INVALID_HASH )
		{}

		/**
		 * @brief Constructor
		 * @param InName	Name
		 * @param InHash	Hash
		 */
		FORCEINLINE NameEntry( const std::wstring& InName, uint32 InHash = INVALID_HASH )
			: name( InName )
			, hash( InHash )
		{}

		std::wstring	name;		/**< Name in string */
		uint32			hash;		/**< Hash name */
	};

	/**
	 * @brief Hash function for STL containers
	 */
	struct HashFunction
	{
		/**
		 * @brief Calculate hash of CName
		 *
		 * @param InName Name
		 */
		FORCEINLINE std::size_t operator()( const CName& InName ) const
		{
			return InName.index;
		}
	};

	friend CArchive& operator<<( CArchive& InArchive, CName& InValue );
	friend CArchive& operator<<( CArchive& InArchive, const CName& InValue );

	/**
	 * @brief Constructor with leaving uninitialized memory
	 */
	FORCEINLINE explicit CName( ENoInit )
	{}

	/**
	 * @brief Constructor
	 * @param InString		String
	 */
	FORCEINLINE CName( const tchar* InString )
		: index( INDEX_NONE )
	{
		StaticInit();
		Init( InString );
	}

	/**
	 * @brief Constructor
	 * @param InString		String
	 */
	FORCEINLINE CName( const std::wstring& InString )
		: index( INDEX_NONE )
	{
		StaticInit();
		Init( InString );
	}

	/**
	 * @brief Constructor
	 * @param InNameEnum	Name enum
	 */
	FORCEINLINE CName( EName InNameEnum )
		: index( InNameEnum )
	{
		StaticInit();
	}

	/**
	 * @brief Constructor
	 */
	FORCEINLINE CName()
		: index( INDEX_NONE )
	{
		StaticInit();
	}

	/**
	 * @brief Convert name to string
	 * @return Return name in string type. If his is not valid returning "NONE"
	 */
	std::wstring ToString() const;

	/**
	 * @brief Convert name to string
	 * @param OutString		Output string
	 */
	void ToString( std::wstring& OutString ) const;

	/**
	 * @brief Get index name in pool
	 * @return Return index name in pool. If his is not valid returning INDEX_NONE
	 */
	FORCEINLINE uint32 GetIndex() const
	{
		return index;
	}

	/**
	 * @brief Is valid name
	 * @return Return TRUE if name is valid
	 */
	FORCEINLINE bool IsValid() const
	{
		return index != INDEX_NONE;
	}

	/**
	 * @brief Init engine names
	 */
	static void StaticInit();

	/**
	 * @brief Return the static initialized flag
	 * Use this function to get or set the variable
	 * 
	 * @return Return TRUE if engine names is initialized, otherwise returning FALSE
	 */
	FORCENOINLINE static bool& GetIsInitialized()
	{
		static bool		bIsInit = false;
		return bIsInit;
	}

	/**
	 * @brief Compare operator
	 *
	 * @param InOther	String to compare
	 * @return Return TRUE if name matches the string, FALSE otherwise
	 */
	bool operator==( const std::wstring& InOther ) const;

	/**
	 * @brief Compare operator
	 *
	 * @param InOther	String to compare
	 * @return Return TRUE if name matches the string, FALSE otherwise
	 */
	bool operator==( const tchar* InOther ) const;

	/**
	 * @brief Compare operator
	 *
	 * @param InOther	String to compare
	 * @return Return TRUE if name not equal to the string, FALSE otherwise
	 */
	FORCEINLINE bool operator!=( const std::wstring& InOther ) const
	{
		return !( *this == InOther );
	}

	/**
	 * @brief Compare operator
	 *
	 * @param InOther	String to compare
	 * @return Return TRUE if name not equal to the string, FALSE otherwise
	 */
	FORCEINLINE bool operator!=( const tchar* InOther ) const
	{
		return !( *this == InOther );
	}

	/**
	 * @brief Assignment operator
	 *
	 * @param InOther	New string
	 * @return Return reference to self
	 */
	FORCEINLINE CName& operator=( const std::wstring& InOther )
	{
		Init( InOther );
		return *this;
	}

	/**
	 * @brief Compare operator
	 *
	 * @param InOther	Name to compare
	 * @return Return TRUE if name matches InOther, FALSE otherwise
	 */
	FORCEINLINE bool operator==( const CName& InOther ) const
	{
		return index == InOther.index;
	}

	/**
	 * @brief Compare operator
	 *
	 * @param InOther	Name to compare
	 * @return Return TRUE if name matches InOther, FALSE otherwise
	 */
	FORCEINLINE bool operator==( EName InOther ) const
	{
		return index == InOther;
	}

	/**
	 * @brief Compare operator
	 *
	 * @param InOther	Name to compare
	 * @return Return TRUE if name not equal to InOther, FALSE otherwise
	 */
	FORCEINLINE bool operator!=( const CName& InOther ) const
	{
		return !( *this == InOther );
	}

	/**
	 * @brief Compare operator
	 *
	 * @param InOther	Name to compare
	 * @return Return TRUE if name not equal to InOther, FALSE otherwise
	 */
	FORCEINLINE bool operator!=( EName InOther ) const
	{
		return !( *this == InOther );
	}

	/**
	 * @brief Assignment operator
	 *
	 * @param InOther	New name
	 * @return Return reference to self
	 */
	FORCEINLINE CName& operator=( const CName& InOther )
	{
		index = InOther.index;
		return *this;
	}

	/**
	 * @brief Assignment operator
	 *
	 * @param InString		New string
	 * @return Return reference to self
	 */
	FORCEINLINE CName& operator=( const tchar* InString )
	{
		Init( InString );
		return *this;
	}

	/**
	 * @brief Assignment operator
	 *
	 * @param InOther	New name
	 * @return Return reference to self
	 */
	FORCEINLINE CName& operator=( EName InOther )
	{
		index = InOther;
		return *this;
	}

private:
	/**
	 * @brief Initialize name
	 * @param InString		String
	 */
	void Init( const std::wstring& InString );

	uint32		index;		/**< Index name */
};

#endif // !NAME_H