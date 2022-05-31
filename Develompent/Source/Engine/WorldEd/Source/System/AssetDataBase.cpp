#include "Misc/CoreGlobals.h"
#include "Logger/LoggerMacros.h"
#include "System/BaseFileSystem.h"
#include "System/AssetDataBase.h"
#include "System/SplashScreen.h"
#include "WorldEd.h"

void CAssetDataBase::Init()
{
	// If failed serialize TOC file, we generate new TOC file and serialize data to archive
	if ( !SerializeTOC() )
	{
		appSetSplashText( STT_StartupProgress, TEXT( "Prepare engine content" ) );
		AddTOCEntries( GetEngineContentDir() );

		appSetSplashText( STT_StartupProgress, TEXT( "Prepare game content" ) );
		AddTOCEntries( GetGameContentDir() );

		SerializeTOC( true );
	}
}

void CAssetDataBase::AddTOCEntries( const std::wstring& InRootDir )
{
	std::vector< std::wstring >		files = GFileSystem->FindFiles( InRootDir, true, true );
	for ( uint32 index = 0, count = files.size(); index < count; ++index )
	{
		std::wstring		file = files[ index ];
		std::size_t			dotPos = file.find_last_of( TEXT( "." ) );
		std::wstring		fullPath = InRootDir + TEXT( "/" ) + file;

		if ( dotPos == std::wstring::npos )
		{
			AddTOCEntries( fullPath );
			continue;
		}

		std::wstring		extension = file;
		extension.erase( 0, dotPos + 1 );
		if ( extension == TEXT( "lpak" ) )
		{
			GTableOfContents.AddEntry( fullPath );
		}
	}
}

bool CAssetDataBase::SerializeTOC( bool InIsSave /* = false */ )
{
	std::wstring	pathTOC		= appGameDir() + ( PATH_SEPARATOR TEXT( "EditorCache" ) ) + PATH_SEPARATOR + GTableOfContents.GetNameTOC();
	CArchive*		archiveTOC	= !InIsSave ? GFileSystem->CreateFileReader( pathTOC ) : GFileSystem->CreateFileWriter( pathTOC ) ;
	if ( archiveTOC )
	{
		GTableOfContents.Serialize( *archiveTOC );
		delete archiveTOC;
		return true;
	}
	else
	{
		LE_LOG( LT_Warning, LC_Editor, TEXT( "Failed serialize TOC file '%s'" ), pathTOC.c_str() );
		return false;
	}
}

void CAssetDataBase::Shutdown()
{}