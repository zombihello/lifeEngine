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
#include "Hashing/FastHash.h"
#include "Core.h"

/**
 * @ingroup Core
 * @brief Special value for an FName with no number
 */
#define NAME_NO_NUMBER 0

/**
 * @ingroup Core
 * @brief Enumeration for finding name
 */
enum EFindName
{
	CNAME_Find,		/** Find a name, return 0 if it doesn't exist */
	CNAME_Add		/** Find a name or add it if it doesn't exist */
};

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
		FORCEINLINE NameEntry( const std::wstring& InName, uint64 InHash = INVALID_HASH )
			: name( InName )
			, hash( InHash )
		{}

		std::wstring	name;		/**< Name in string */
		uint64			hash;		/**< Hash name */
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
			return InName.GetHash();
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
	 * 
	 * @param InString		Value for the string portion of the name
	 * @param InFindType	Action to take (see EFindName)
	 */
	FORCEINLINE CName( const tchar* InString, EFindName InFindType = CNAME_Add )
	{
		StaticInit();
		uint32		idStartNumber	= INDEX_NONE;
		uint32		strLength		= Sys_Strlen( InString );
		uint32		number			= ParseNumber( InString, idStartNumber );
		if ( idStartNumber != INDEX_NONE )
		{
			strLength = idStartNumber;
		}

		Init( InString, strLength, number, InFindType );
	}

	/**
	 * @brief Constructor
	 * 
	 * @param InString		Value for the string portion of the name
	 * @param InFindType	Action to take (see EFindName)
	 */
	FORCEINLINE CName( const std::wstring& InString, EFindName InFindType = CNAME_Add )
	{
		StaticInit();
		uint32		idStartNumber	= INDEX_NONE;
		uint32		strLength		= InString.size();
		uint32		number			= ParseNumber( InString.c_str(), idStartNumber );
		if ( idStartNumber != INDEX_NONE )
		{
			strLength = idStartNumber;
		}

		Init( InString, strLength, number, InFindType );
	}

	/**
	 * @brief Constructor
	 * 
	 * @param InString		Value for the string portion of the name
	 * @param InNumber		Number part of the name/number pair
	 * @param InFindType	Action to take (see EFindName)
	 */
	FORCEINLINE CName( const tchar* InString, uint32 InNumber, EFindName InFindType = CNAME_Add )
	{
		StaticInit();
		Init( InString, Sys_Strlen( InString ), InNumber, InFindType );
	}

	/**
	 * @brief Constructor
	 * 
	 * @param InString		Value for the string portion of the name
	 * @param InNumber		Number part of the name/number pair
	 * @param InFindType	Action to take (see EFindName)
	 */
	FORCEINLINE CName( const std::wstring& InString, uint32 InNumber, EFindName InFindType = CNAME_Add )
	{
		StaticInit();
		Init( InString, InString.size(), InNumber, InFindType );
	}

	/**
	 * @brief Create an CName with a hardcoded string index
	 * 
	 * @param InNameEnum	The hardcoded value the string portion of the name will have. The number portion will be NAME_NO_NUMBER
	 */
	FORCEINLINE CName( EName InNameEnum )
		: index( InNameEnum )
		, number( NAME_NO_NUMBER )
	{
		StaticInit();
	}

	/**
	 * @brief Create an CName with a hardcoded string index
	 *
	 * @param InNameEnum	The hardcoded value the string portion of the name will have
	 * @param InNameNumber	The hardcoded number portion
	 */
	FORCEINLINE CName( EName InNameEnum, uint32 InNameNumber )
		: index( InNameEnum )
		, number( InNameNumber )
	{
		StaticInit();
	}

	/**
	 * @brief Constructor
	 */
	FORCEINLINE CName()
		: index( INDEX_NONE )
		, number( NAME_NO_NUMBER )
	{
		StaticInit();
	}

	/**
	 * @brief Convert name to string
	 * 
	 * @param InIsWithoutNumber		Is need ignore number
	 * @return Return name in string type. If his is not valid returning "NONE"
	 */
	FORCEINLINE std::wstring ToString( bool InIsWithoutNumber = false ) const
	{
		std::wstring	result;
		ToString( result, InIsWithoutNumber );
		return result;
	}

	/**
	 * @brief Convert name to string
	 * 
	 * @param InIsWithoutNumber		Is need ignore number
	 * @param OutString				Output string
	 */
	FORCEINLINE void ToString( std::wstring& OutString, bool InIsWithoutNumber = false ) const
	{
		OutString.clear();
		AppendString( OutString, InIsWithoutNumber );
	}

	/**
	 * @brief Converts an CName to a readable format, in place
	 * 
	 * @param InIsWithoutNumber		Is need ignore number
	 * @param OutResult				String to fill with the string representation of the name
	 */
	void AppendString( std::wstring& OutResult, bool InIsWithoutNumber = false ) const;

	/**
	 * @brief Get index name in pool
	 * @return Return index name in pool. If his is not valid returning INDEX_NONE
	 */
	FORCEINLINE uint32 GetIndex() const
	{
		return index;
	}

	/**
	 * @brief Get number portion of the string/number pair
	 * @return Return number portion of the string/number pair. If this CName is EName returns NAME_NO_NUMBER
	 */
	FORCEINLINE uint32 GetNumber() const
	{
		return number;
	}

	/**
	 * @brief Get hash
	 * @return Return calculated hash
	 */
	FORCEINLINE uint64 GetHash() const
	{
		uint64	hash = FastHash( index );
		return FastHash( number, hash );
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
	FORCEINLINE static bool& GetIsInitialized()
	{
		static bool		bIsInit = false;
		return bIsInit;
	}

	/**
	 * @brief Is name ends with suffix
	 *
	 * @param InSuffix	Name suffix in InName
	 * @return Return TRUE if InName ends with InSuffix, otherwise returns FALSE
	 */
	bool EndsWith( const CName& InSuffix ) const;

	/**
	 * @brief Compare operator
	 *
	 * @param InOther	String to compare
	 * @return Return TRUE if name matches the string, FALSE otherwise
	 */
	FORCEINLINE bool operator==( const std::wstring& InOther ) const
	{
		*this == InOther.c_str();
	}

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
		uint32	idStartNumber	= INDEX_NONE;
		uint32	strLength		= InOther.size();
		uint32	number			= ParseNumber( InOther.c_str(), idStartNumber );
		if ( idStartNumber != INDEX_NONE )
		{
			strLength = idStartNumber;
		}

		Init( InOther, strLength, number, CNAME_Add );
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
		return index == InOther.index && number == InOther.number;
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
		number = InOther.number;
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
		uint32	idStartNumber	= INDEX_NONE;
		uint32	strLength		= Sys_Strlen( InString );
		uint32	number			= ParseNumber( InString, idStartNumber );
		if ( idStartNumber != INDEX_NONE )
		{
			strLength = idStartNumber;
		}

		Init( InString, strLength, number, CNAME_Add );
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
		number = NAME_NO_NUMBER;
		return *this;
	}

private:
	/**
	 * @brief Initialize name
	 * @param InString		String name of the name/number pair
	 * @param InStringSize	Size of InString
	 * @param InNumber		Number part of the name/number pair
	 * @param InFindType	Operation to perform on names
	 */
	void Init( const std::wstring& InString, uint32 InStringSize, uint32 InNumber, EFindName InFindType );

	/**
	 * @brief Parse number in a string
	 * 
	 * This function parse number from string and return it
	 * Syntax: <String>_<Number>
	 * 
	 * @param InString			String
	 * @param OutIdStartNumber	Position in InString where beginning section of number (include '_'). INDEX_NONE when string not have number
	 * @return Return parsed number in InString. If InString not have number after '_' returns NAME_NO_NUMBER
	 */
	static uint32 ParseNumber( const tchar* InString, uint32& OutIdStartNumber );

	uint32		index;		/**< Index name (used to find String portion of the string/number pair) */
	uint32		number;		/**< Number portion of the string/number pair */
};

#endif // !NAME_H