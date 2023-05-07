#include <vector>

#include "Misc/Class.h"
#include "Misc/Misc.h"
#include "Misc/CoreGlobals.h"
#include "Misc/TableOfContents.h"
#include "Logger/LoggerMacros.h"
#include "System/BaseFileSystem.h"
#include "System/Package.h"
#include "Containers/StringConv.h"
#include "Render/StaticMesh.h"
#include "Commandlets/CookerSyncCommandlet.h"

IMPLEMENT_CLASS( CCookerSyncCommandlet )

/*
==================
CCookerSyncCommandlet::AddContentEntries
==================
*/
void CCookerSyncCommandlet::AddContentEntries( const std::wstring& InRootDir )
{
	std::vector< std::wstring >		files = g_FileSystem->FindFiles( InRootDir, true, true );
	for ( uint32 index = 0, count = files.size(); index < count; ++index )
	{
		std::wstring		file = files[ index ];
		std::size_t			dotPos = file.find_last_of( TEXT( "." ) );
		std::wstring		fullPath = InRootDir + TEXT( "/" ) + file;
		
		if ( dotPos == std::wstring::npos )
		{
			AddContentEntries( fullPath );
			continue;
		}

		std::wstring		extension = file;
		extension.erase( 0, dotPos + 1 );
		if ( extension == TEXT( "pak" ) )
		{
			PackageRef_t		package = g_PackageManager->LoadPackage( fullPath );
			Assert( package );

			Logf( TEXT( "Added package '%s'\n" ), fullPath.c_str() );
			g_TableOfContents.AddEntry( package->GetGUID(), package->GetName(), fullPath );
			g_PackageManager->UnloadPackage( fullPath );
		}
	}
}

/*
==================
CCookerSyncCommandlet::Main
==================
*/
bool CCookerSyncCommandlet::Main( const CCommandLine& InCommandLine )
{
	g_TableOfContents.Clear();
	AddContentEntries( Sys_BaseDir() );

	// Serialize table of contents
	CArchive*		archiveTOC = g_FileSystem->CreateFileWriter( g_CookedDir + CTableOfContets::GetNameTOC(), AW_NoFail );
	g_TableOfContents.Serialize( *archiveTOC );
	delete archiveTOC;

	return true;
}