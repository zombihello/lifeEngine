#include <memory.h>
#include <Windows.h>

#include "Core.h"
#include "Misc/Template.h"
#include "Misc/Class.h"
#include "WindowsArchive.h"

// ====================================
// Archive reading
// ====================================

/**
 * Constructor
 */
FWindowsArchiveReading::FWindowsArchiveReading( std::ifstream* InFile ) :
	file( InFile )
{}

/**
 * Destructor
 */
FWindowsArchiveReading::~FWindowsArchiveReading()
{
	delete file;
}

/**
 * Get size of archive
 */
uint32 FWindowsArchiveReading::GetSize()
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
void FWindowsArchiveReading::Seek( uint32 InPosition )
{
	file->seekg( InPosition, std::ios::beg );
}

/**
 * Flush data
 */
void FWindowsArchiveReading::Flush()
{}

/**
 * Get current position in archive
 */
uint32 FWindowsArchiveReading::Tell()
{
	return ( uint32 )file->tellg();
}

/**
 * Serialize data
 */
void FWindowsArchiveReading::Serialize( void* InBuffer, uint32 InSize )
{
	file->read( ( achar* )InBuffer, InSize );
}

/**
 * Is loading archive
 */
bool FWindowsArchiveReading::IsLoading() const
{
	return true;
}

// ====================================
// Archive writing
// ====================================

/**
 * Constructor
 */
FWindowsArchiveWriter::FWindowsArchiveWriter( std::ofstream* InFile ) :
	file( InFile )
{}

/**
 * Destructor
 */
FWindowsArchiveWriter::~FWindowsArchiveWriter()
{
	Flush();
	delete file;
}

/**
 * Get size of archive
 */
uint32 FWindowsArchiveWriter::GetSize()
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
void FWindowsArchiveWriter::Seek( uint32 InPosition )
{
	Flush();
	file->seekp( InPosition, std::ios::beg );
}

/**
 * Flush data
 */
void FWindowsArchiveWriter::Flush()
{
	file->flush();
}

/**
 * Get current position in archive
 */
uint32 FWindowsArchiveWriter::Tell()
{
	Flush();
	return ( uint32 )file->tellp();
}

/**
 * Serialize data
 */
void FWindowsArchiveWriter::Serialize( void* InBuffer, uint32 InSize )
{
	file->write( ( achar* )InBuffer, InSize );
	Flush();
}

/**
 * Is saving archive
 */
bool FWindowsArchiveWriter::IsSaving() const
{
	return true;
}