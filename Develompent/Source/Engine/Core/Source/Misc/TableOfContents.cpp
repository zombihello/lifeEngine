#include <sstream>

#include "Misc/CoreGlobals.h"
#include "Misc/TableOfContents.h"
#include "Logger/LoggerMacros.h"
#include "Misc/StringConv.h"
#include "System/Package.h"

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
		Memory::Memzero( buffer, archiveSize );

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