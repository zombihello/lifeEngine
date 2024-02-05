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

#include <sstream>

#include "Core/Misc/CoreGlobals.h"
#include "Core/Misc/TableOfContents.h"
#include "Core/Logger/LoggerMacros.h"
#include "Core/Containers/StringConv.h"
#include "Core/System/Package.h"

/*
==================
CTableOfContets::Serialize
==================
*/
void CTableOfContets::Serialize( CArchive& InArchive )
{
	if ( InArchive.IsLoading() )
	{
		// Create string buffer and fill '\0'
		uint32				archiveSize = InArchive.GetSize() + 1;
		byte* buffer = new byte[ archiveSize ];
		memset( buffer, '\0', archiveSize );

		// Serialize data to string buffer
		InArchive.Serialize( buffer, archiveSize );

		// Parse table of content
		std::stringstream		strStream( ( char* )buffer );
		std::string				strGuid;
		std::string				strName;
		std::string				strPath;

		while ( strStream >> strGuid >> strName >> strPath )
		{
			// Add content to table
			CGuid			guid;
			if ( !guid.InitFromString( ANSI_TO_TCHAR( strGuid.c_str() ) ) )
			{ 
				Warnf( TEXT( "Failed init GUID for content '%s'\n" ), ANSI_TO_TCHAR( strPath.c_str() ) );
				continue;
			}

			AddEntry( guid, ANSI_TO_TCHAR( strName.c_str() ), ANSI_TO_TCHAR( strPath.c_str() ) );
		}

		delete[] buffer;
	}
	else
	{
		for ( auto itEntry = guidEntries.begin(), itEntryEnd = guidEntries.end(); itEntry != itEntryEnd; ++itEntry )
		{
			const TOCEntry&		tocEntry = itEntry->second;

			//						GUID													Name										Path to content
			InArchive << TCHAR_TO_ANSI( itEntry->first.String().c_str() ) << " " << TCHAR_TO_ANSI( tocEntry.name.c_str() ) << " " << TCHAR_TO_ANSI( tocEntry.path.c_str() ) << "\n";
		}
	}
}

/*
==================
CTableOfContets::AddEntry
==================
*/
void CTableOfContets::AddEntry( const std::wstring& InPath )
{
	PackageRef_t		package = g_PackageManager->LoadPackage( InPath );
	if ( package )
	{
		AddEntry( package->GetGUID(), package->GetName(), InPath );
		g_PackageManager->UnloadPackage( InPath );
	}
}

/*
==================
CTableOfContets::RemoveEntry
==================
*/
void CTableOfContets::RemoveEntry( const std::wstring& InPath )
{
	PackageRef_t		package = g_PackageManager->LoadPackage( InPath );
	if ( package )
	{
		RemoveEntry( package->GetGUID() );
		g_PackageManager->UnloadPackage( InPath );
	}
}