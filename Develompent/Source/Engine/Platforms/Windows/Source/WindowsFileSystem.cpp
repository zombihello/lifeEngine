#include <Windows.h>

#include "Core.h"
#include "WindowsFileSystem.h"
#include "WindowsArchive.h"
#include "Containers/String.h"

/**
 * Constructor
 */
FWindowsFileSystem::FWindowsFileSystem()
{}

/**
 * Destructor
 */
FWindowsFileSystem::~FWindowsFileSystem()
{}

/**
 * Create file reader
 */
class FBaseArchive* FWindowsFileSystem::CreateFileReader( const std::wstring& InFileName, uint32 InFlags )
{
	std::ifstream*			inputFile = new std::ifstream();

	// Create file and create archive reader
	inputFile->open( InFileName, std::ios::binary );
	if ( !inputFile->is_open() )
	{
		if ( InFlags & AR_NoFail )
		{
			appErrorf( TEXT( "Failed to create file: %s, InFlags = %X" ), InFileName.c_str(), InFlags );
		}

		delete inputFile;
		return nullptr;
	}

	return new FWindowsArchiveReading( inputFile );
}

/**
 * Create file writer
 */
class FBaseArchive* FWindowsFileSystem::CreateFileWriter( const std::wstring& InFileName, uint32 InFlags )
{
	std::ofstream*			outputFile = new std::ofstream();
	int						flags = 0;

	if ( InFlags & AW_Append )
	{
		flags = std::ios::app;
	}

	// Create directory for file
	{
		std::wstring			path = InFileName;
		std::size_t				slashIndex = path.find_last_of( TEXT( "/" ) );
		if ( slashIndex == std::wstring::npos )
		{
			slashIndex = path.find_last_of( TEXT( "\\" ) );
		}

		path.erase( slashIndex, path.size() );
		CreateDirectoryW( path.c_str(), nullptr );
	}

	// Create file and create archive writer
	outputFile->open( InFileName, flags | std::ios::binary );
	if ( !outputFile->is_open() )
	{
		if ( InFlags & AW_NoFail )
		{
			appErrorf( TEXT( "Failed to create file: %s, InFlags = %X" ), InFileName.c_str(), InFlags );
		}

		delete outputFile;
		return nullptr;
	}

	return new FWindowsArchiveWriter( outputFile );
}

/**
 * Find files in directory
 */
std::vector< std::wstring > FWindowsFileSystem::FindFiles( const std::wstring& InDirectory, bool InIsFiles, bool InIsDirectories )
{
	HANDLE								handle = nullptr;
	WIN32_FIND_DATAW					data;
	std::vector< std::wstring >			result;

	handle = FindFirstFileW( FString::Format( TEXT( "%s/*" ), InDirectory.c_str() ).c_str(), &data );
	if ( handle != INVALID_HANDLE_VALUE )
	{
		do
		{
			if ( wcscmp( data.cFileName, TEXT( "." ) ) && wcscmp( data.cFileName, TEXT( ".." ) ) &&
				 ( ( data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) ? InIsDirectories : InIsFiles ) )
			{
				result.push_back( data.cFileName );
			}
		}
		while ( FindNextFileW( handle, &data ) );
	}
	if ( handle != INVALID_HANDLE_VALUE )
	{
		FindClose( handle );
	}

	return result;
}

/**
 * Convert to absolute path
 */
std::wstring FWindowsFileSystem::ConvertToAbsolutePath( const std::wstring& InPath ) const
{
	std::wstring		path = InPath;
	if ( path.find( TEXT( ".." ) ) != std::wstring::npos )
	{
		path = GetCurrentDirectory() + TEXT( "/" ) + path;
	}

	return path;
}

/**
 * Set current directory
 */
void FWindowsFileSystem::SetCurrentDirectory( const std::wstring& InDirectory )
{
	SetCurrentDirectoryW( InDirectory.c_str() );
}

/**
 * Get current directory
 */
std::wstring FWindowsFileSystem::GetCurrentDirectory() const
{
	tchar		path[ MAX_PATH ];
	::GetCurrentDirectoryW( MAX_PATH, path );
	return path;
}