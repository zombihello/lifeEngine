#include <Windows.h>

#include "Core.h"
#include "WindowsFileSystem.h"
#include "WindowsArchive.h"

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
	inputFile->open( InFileName );
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