/**
 * @file
 * @addtogroup Core Core
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef TABLEOFCONTENTS_H
#define TABLEOFCONTENTS_H

#include <unordered_map>

#include "Misc/Types.h"
#include "Misc/Guid.h"
#include "System/Archive.h"
#include "Misc/Platform.h"

/**
 * @ingroup Core
 * Class for working with table of contents
 */
class CTableOfContets
{
public:
	/**
	 * Serialize archive
	 * 
	 * @param InArchive Archive
	 */
	void Serialize( CArchive& InArchive );

	/**
	 * Clear table
	 */
	FORCEINLINE void Clear()
	{
		nameEntries.clear();
		guidEntries.clear();
	}

	/**
	 * Add a package to the TOC at runtime
	 * @param InPath	Path to the package
	 */
	void AddEntry( const std::wstring& InPath );

	/**
	 * Add a package to the TOC at runtime
	 *
	 * @param InGUID GUID of the package
	 * @param InName Name of the package
	 * @param InPath Path to the package
	 */
	FORCEINLINE void AddEntry( const CGuid& InGUID, const std::wstring& InName, const std::wstring& InPath )
	{
		nameEntries.insert( std::make_pair( InName, TOCEntry{ InName, InPath } ) );
		guidEntries.insert( std::make_pair( InGUID, TOCEntry{ InName, InPath } ) );
	}

	/**
	 * Remove a package from the TOC at runtime
	 * @param InPath	Path to the package
	 */
	void RemoveEntry( const std::wstring& InPath );

	/**
	 * Remove a package from the TOC at runtime
	 * @param InGUID	GUID of the package
	 */
	FORCEINLINE void RemoveEntry( const CGuid& InGUID )
	{
		auto	it = guidEntries.find( InGUID );
		if ( it == guidEntries.end() )
		{
			return;
		}

		nameEntries.erase( it->second.name );
		guidEntries.erase( it );
	}

	/**
	 * Is initialized TOC 
	 * @return if the TOC has been initialized with any files
	 */
	FORCEINLINE bool HasBeenInitialized() const
	{
		return GetNumEntries() >= 1;
	}

	/**
	 * Get path to the package
	 * 
	 * @param InGUID GUID of the package
	 * @return Return path to the package by GUID, if not found returning empty string
	 */
	FORCEINLINE std::wstring GetPackagePath( const CGuid& InGUID ) const
	{
		auto	itEntry = guidEntries.find( InGUID );
		if ( itEntry != guidEntries.end() )
		{
			return itEntry->second.path;
		}

		return TEXT( "" );
	}

	/**
	 * Get path to the package
	 *
	 * @param InName Name of the package
	 * @return Return path to the package by name, if not found returning empty string
	 */
	FORCEINLINE std::wstring GetPackagePath( const std::wstring& InName ) const
	{
		auto	itEntry = nameEntries.find( InName );
		if ( itEntry != nameEntries.end() )
		{
			return itEntry->second.path;
		}

		return TEXT( "" );
	}

	/**
	 * Get number of entries
	 * @return Return number of entries
	 */
	FORCEINLINE uint32 GetNumEntries() const
	{
		return guidEntries.size();
	}

	/**
	 * Get name of the table of content
	 * @return Return name of the table of content
	 */
	FORCEINLINE static std::wstring GetNameTOC()
	{
		return TEXT( "TOC.txt" );
	}

private:
	/**
	 * TOC entry
	 */
	struct TOCEntry
	{
		std::wstring		name;		/**< Name of entry */
		std::wstring		path;		/**< Path to entry */
	};

	std::unordered_map< std::wstring, TOCEntry >					nameEntries;			/**< Entries of table content. Key - Name of the package, Item - Path to package */
	std::unordered_map< CGuid, TOCEntry, CGuid::GuidKeyFunc >		guidEntries;			/**< Entries of table content. Key - GUID of the package, Item - Path to package */
};

#endif // !TABLEOFCONTENTS_H