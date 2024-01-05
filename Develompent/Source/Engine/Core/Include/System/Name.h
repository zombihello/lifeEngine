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
	 * 
	 * @param InString		Value for the string portion of the name
	 * @param InFindType	Action to take (see EFindName)
	 */
	FORCEINLINE CName( const tchar* InString, EFindName InFindType = CNAME_Add )
	{
		StaticInit();
		Init( InString, NAME_NO_NUMBER, InFindType );
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
		Init( InString, NAME_NO_NUMBER, InFindType );
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
		Init( InString, InNumber, InFindType );
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
		Init( InString, InNumber, InFindType );
	}

	/**
	 * @brief Create an CName with a hardcoded string index
	 * 
	 * @param InNameEnum	The harcdcoded value the string portion of the name will have. The number portion will be NAME_NO_NUMBER
	 */
	FORCEINLINE CName( EName InNameEnum )
		: index( InNameEnum )
		, number( NAME_NO_NUMBER )
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
	 * @return Return name in string type. If his is not valid returning "NONE"
	 */
	FORCEINLINE std::wstring ToString() const
	{
		std::wstring	result;
		ToString( result );
		return result;
	}

	/**
	 * @brief Convert name to string
	 * @param OutString		Output string
	 */
	FORCEINLINE void ToString( std::wstring& OutString ) const
	{
		OutString.clear();
		AppendString( OutString );
	}

	/**
	 * @brief Converts an CName to a readable format, in place
	 * @param OutResult		String to fill with the string representation of the name
	 */
	void AppendString( std::wstring& OutResult ) const;

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
		Init( InOther, NAME_NO_NUMBER, CNAME_Add );
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
		Init( InString, NAME_NO_NUMBER, CNAME_Add );
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
	 * @param InNumber		Number part of the name/number pair
	 * @param InFindType	Operation to perform on names
	 */
	void Init( const std::wstring& InString, uint32 InNumber, EFindName InFindType );

	uint32		index;		/**< Index name (used to find String portion of the string/number pair) */
	uint32		number;		/**< Number portion of the string/number pair */
};

#endif // !NAME_H