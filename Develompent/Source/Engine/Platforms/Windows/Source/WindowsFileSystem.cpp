#include <Windows.h>

#include "Core.h"
#include "WindowsFileSystem.h"
#include "WindowsArchive.h"
#include "Containers/String.h"

/**
 * Constructor
 */
WindowsFileSystem::WindowsFileSystem()
{}

/**
 * Destructor
 */
WindowsFileSystem::~WindowsFileSystem()
{}

/**
 * Create file reader
 */
class BaseArchive* WindowsFileSystem::CreateFileReader( const tchar* InFileName, uint32 InFlags )
{
	std::ifstream*			inputFile = new std::ifstream();

	// Create file and create archive reader
	inputFile->open( InFileName, std::ios::binary );
	if ( !inputFile->is_open() )
	{
		if ( InFlags & AR_NoFail )
		{
			appErrorf( TEXT( "Failed to create file: %s, InFlags = %X" ), InFileName, InFlags );
		}

		delete inputFile;
		return nullptr;
	}

	return new WindowsArchiveReading( inputFile );
}

/**
 * Create file writer
 */
class BaseArchive* WindowsFileSystem::CreateFileWriter( const tchar* InFileName, uint32 InFlags )
{
	std::ofstream*			outputFile = new std::ofstream();
	int						flags = 0;

	if ( InFlags & AW_Append )
	{
		flags = std::ios::app;
	}

	// Create file and create archive writer
	outputFile->open( InFileName, flags | std::ios::binary );
	if ( !outputFile->is_open() )
	{
		if ( InFlags & AW_NoFail )
		{
			appErrorf( TEXT( "Failed to create file: %s, InFlags = %X" ), InFileName, InFlags );
		}

		delete outputFile;
		return nullptr;
	}

	return new WindowsArchiveWriter( outputFile );
}

/**
 * Find files in directory
 */
std::vector< std::wstring > WindowsFileSystem::FindFiles( const tchar* InDirectory, bool InIsFiles, bool InIsDirectories )
{
	HANDLE								handle = nullptr;
	WIN32_FIND_DATAW					data;
	std::vector< std::wstring >			result;

	handle = FindFirstFileW( String::Format( TEXT( "%s/*" ), InDirectory ), &data );
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
 * Set current directory
 */
void WindowsFileSystem::SetCurrentDirectory( const tchar* InDirectory )
{
	SetCurrentDirectoryW( InDirectory );
}

/**
 * Get current directory
 */
const tchar* WindowsFileSystem::GetCurrentDirectory() const
{
	// TODO BG yehor.pohuliaka - Need add return current directory
	return TEXT( "" );
}