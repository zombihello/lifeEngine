/**
 * @file
 * @addtogroup Core Core
 *
 * ************************************************************
 *                  This file is part of:
 *                      LIFEENGINE
 *          https://github.com/zombihello/lifeEngine
 * ************************************************************
 * Copyright (C) 2024 Yehor Pohuliaka.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef TABLEOFCONTENTS_H
#define TABLEOFCONTENTS_H

#include <unordered_map>

#include "Core/Misc/Types.h"
#include "Core/Misc/Guid.h"
#include "Core/System/Archive.h"
#include "Core/CoreDefines.h"

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