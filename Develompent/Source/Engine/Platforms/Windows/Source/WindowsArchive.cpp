#include <memory.h>
#include <Windows.h>

#include "Core.h"
#include "Misc/Template.h"
#include "WindowsArchive.h"

// ====================================
// Archive reading
// ====================================

/**
 * Constructor
 */
WindowsArchiveReading::WindowsArchiveReading( std::ifstream* InFile ) :
	file( InFile )
{}

/**
 * Destructor
 */
WindowsArchiveReading::~WindowsArchiveReading()
{
	delete file;
}

/**
 * Get size of archive
 */
uint32 WindowsArchiveReading::GetSize()
{
	uint32			currentPosition = ( uint32 )Tell();
	uint32			sizeFile = 0;

	file->seekg( 0, std::ios::end );
	sizeFile = ( uint32 )Tell();
	file->seekg( currentPosition, std::ios::beg );

	return sizeFile;
}

/**
 * Set current position in archive
 */
void WindowsArchiveReading::Seek( uint32 InPosition )
{
	file->seekg( InPosition, std::ios::beg );
}

/**
 * Flush data
 */
void WindowsArchiveReading::Flush()
{}

/**
 * Get current position in archive
 */
uint32 WindowsArchiveReading::Tell()
{
	return ( uint32 )file->tellg();
}

/**
 * Serialize data
 */
void WindowsArchiveReading::Serialize( void* InBuffer, uint32 InSize )
{
	file->read( ( achar* )InBuffer, InSize );
}

/**
 * Is loading archive
 */
bool WindowsArchiveReading::IsLoading() const
{
	return true;
}

// ====================================
// Archive writing
// ====================================

/**
 * Constructor
 */
WindowsArchiveWriter::WindowsArchiveWriter( std::ofstream* InFile ) :
	file( InFile )
{}

/**
 * Destructor
 */
WindowsArchiveWriter::~WindowsArchiveWriter()
{
	Flush();
	delete file;
}

/**
 * Get size of archive
 */
uint32 WindowsArchiveWriter::GetSize()
{
	// Make sure that all data is written before looking at file size.
	Flush();

	uint32			currentPosition = ( uint32 )Tell();
	uint32			sizeFile = 0;

	file->seekp( 0, std::ios::end );
	sizeFile = ( uint32 )Tell();
	file->seekp( currentPosition, std::ios::beg );

	return sizeFile;
}

/**
 * Set current position in archive
 */
void WindowsArchiveWriter::Seek( uint32 InPosition )
{
	Flush();
	file->seekp( InPosition, std::ios::beg );
}

/**
 * Flush data
 */
void WindowsArchiveWriter::Flush()
{
	file->flush();
}

/**
 * Get current position in archive
 */
uint32 WindowsArchiveWriter::Tell()
{
	Flush();
	return ( uint32 )file->tellp();
}

/**
 * Serialize data
 */
void WindowsArchiveWriter::Serialize( void* InBuffer, uint32 InSize )
{
	file->write( ( achar* )InBuffer, InSize );
	Flush();
}

/**
 * Is saving archive
 */
bool WindowsArchiveWriter::IsSaving() const
{
	return true;
}