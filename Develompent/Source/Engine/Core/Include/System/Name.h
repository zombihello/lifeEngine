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
	struct SNameEntry
	{
		/**
		 * @brief Constructor
		 */
		FORCEINLINE SNameEntry()
			: index( INDEX_NONE )
		{}

		/**
		 * @brief Constructor
		 * @param InName	Name
		 * @param InIndex	Index
		 */
		FORCEINLINE SNameEntry( const std::wstring& InName, uint32 InIndex )
			: name( InName )
			, index( InIndex )
		{}

		std::wstring	name;		/**< Name in string */
		uint32			index;		/**< Index of name */
	};

	/**
	 * @brief Constructor
	 * @param InString		String
	 */
	FORCEINLINE CName( const std::wstring& InString )
	{
		Init( InString );
	}

	/**
	 * @brief Constructor
	 * @param InNameEnum	Name enum
	 */
	FORCEINLINE CName( EName InNameEnum )
	{
		*this = InNameEnum;
	}

	/**
	 * @brief Constructor
	 */
	FORCEINLINE CName()
	{}

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
		return nameEntry ? nameEntry->index : INDEX_NONE;
	}

	/**
	 * @brief Is valid name
	 * @return Return TRUE if name is valid
	 */
	FORCEINLINE bool IsValid() const
	{
		return nameEntry.IsValid();
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
		return nameEntry == InOther.nameEntry;
	}

	/**
	 * @brief Compare operator
	 *
	 * @param InOther	Name to compare
	 * @return Return TRUE if name matches InOther, FALSE otherwise
	 */
	FORCEINLINE bool operator==( EName InOther ) const
	{
		return nameEntry && nameEntry->index == InOther;
	}

	/**
	 * @brief Assignment operator
	 *
	 * @param InOther	New name
	 * @return Return reference to self
	 */
	FORCEINLINE CName& operator=( const CName& InOther )
	{
		nameEntry = InOther.nameEntry;
		return *this;
	}

	/**
	 * @brief Assignment operator
	 *
	 * @param InOther	New name
	 * @return Return reference to self
	 */
	CName& operator=( EName InOther );

private:
	/**
	 * @brief Initialize name
	 * @param InString		String
	 */
	void Init( const std::wstring& InString );

	TSharedPtr<SNameEntry>		nameEntry;	/**< Name entry */
};

#endif // !NAME_H