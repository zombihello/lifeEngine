/**
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

#include "pch_filesystem.h"
#include "filesystem/filesystem.h"
#include "filesystem/searchpath.h"

/*
==================
CSearchPathIterator::CSearchPathIterator
==================
*/
CSearchPathIterator::CSearchPathIterator( bool bForWrite, const achar* pPathID /* = nullptr */, uint32 lengthPathID /* = 0 */ )
	: currentIndex( INDEX_NONE )
{
	CFileSystem*	pFileSystem = ( CFileSystem* )g_pFileSystem;
	Assert( pFileSystem );

	// If we have a path ID try to find all search paths for that
	if ( pPathID && pPathID[0] != '\0' && lengthPathID > 0 )
	{
		// We iterate from end to beginning to be able to overload paths
		for ( int32 index = ( int32 )pFileSystem->searchPaths.size(); --index >= 0; )
		{
			CSearchPath*		pSearchPath = &pFileSystem->searchPaths[index];
			const std::string	searchPathID = pSearchPath->GetPathID();
			if ( searchPathID.size() == lengthPathID && !L_Strnicmp( searchPathID.c_str(), pPathID, lengthPathID ) )
			{
				searchPaths.push_back( pSearchPath );
			}
		}
		
		// Try to get default search path for write if we didn't found anything
		if ( bForWrite && searchPaths.empty() )
		{
			Warning( "FileSystem: Requested non-existent write path '%s'!", pPathID );
			CSearchPath*	pFirstWriteSearchPath		= nullptr;
			const achar*	pDefaultWritePathID			= "DEFAULT_WRITE_PATH";
			const uint32	lenghtDefaultWritePathID	= 19;

			// We iterate from end to beginning to be able to overload paths
			for ( int32 index = ( int32 )pFileSystem->searchPaths.size(); --index >= 0; )
			{
				CSearchPath*		pSearchPath = &pFileSystem->searchPaths[index];
				if ( !pFirstWriteSearchPath )
				{
					pFirstWriteSearchPath = pSearchPath;
				}

				const std::string	searchPathID = pSearchPath->GetPathID();
				if ( searchPathID.size() == lenghtDefaultWritePathID && !L_Strnicmp( searchPathID.c_str(), pDefaultWritePathID, lenghtDefaultWritePathID ) )
				{
					searchPaths.push_back( pSearchPath );
				}
			}

			// Didn't nothing to find? Okay, just add the first write search path
			if ( searchPaths.empty() && pFirstWriteSearchPath )
			{
				searchPaths.push_back( pFirstWriteSearchPath );
			}
		}
	}
	// Otherwise grab all search paths
	else
	{
		// We iterate from end to beginning to be able to overload paths
		for ( int32 index = ( int32 )pFileSystem->searchPaths.size(); --index >= 0; )
		{
			searchPaths.push_back( &pFileSystem->searchPaths[index] );
		}
	}

	// Init current index if search paths aren't empty
	if ( !searchPaths.empty() )
	{
		currentIndex = 0;
	}
}