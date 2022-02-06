#include <sstream>

#include "Misc/TableOfContents.h"
#include "Logger/LoggerMacros.h"
#include "Containers/StringConv.h"

void FTableOfContets::Serialize( FArchive& InArchive )
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
			FGuid			guid;
			if ( !guid.InitFromString( ANSI_TO_TCHAR( strGuid.c_str() ) ) )
			{ 
				LE_LOG( LT_Warning, LC_Package, TEXT( "Failed init GUID for content '%s'" ), ANSI_TO_TCHAR( strPath.c_str() ) );
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
			const FTOCEntry&		tocEntry = itEntry->second;

			//						GUID													Name										Path to content
			InArchive << TCHAR_TO_ANSI( itEntry->first.String().c_str() ) << " " << TCHAR_TO_ANSI( tocEntry.name.c_str() ) << " " << TCHAR_TO_ANSI( tocEntry.path.c_str() ) << "\n";
		}
	}
}