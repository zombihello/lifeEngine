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
CWindowsArchiveReading::CWindowsArchiveReading( std::ifstream* InFile, const std::wstring& InPath )
	: CArchive( InPath )
	, file( InFile )
{}

/**
 * Destructor
 */
CWindowsArchiveReading::~CWindowsArchiveReading()
{
	delete file;
}

/**
 * Get size of archive
 */
uint32 CWindowsArchiveReading::GetSize()
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
void CWindowsArchiveReading::Seek( uint32 InPosition )
{
	file->seekg( InPosition, std::ios::beg );
}

/**
 * Flush data
 */
void CWindowsArchiveReading::Flush()
{}

/**
 * Get current position in archive
 */
uint32 CWindowsArchiveReading::Tell()
{
	return ( uint32 )file->tellg();
}

/**
 * Serialize data
 */
void CWindowsArchiveReading::Serialize( void* InBuffer, uint32 InSize )
{
	file->read( ( achar* )InBuffer, InSize );
}

bool CWindowsArchiveReading::IsEndOfFile()
{
	uint32		sizeFile = GetSize();
	return Tell() == sizeFile;
}

/**
 * Is loading archive
 */
bool CWindowsArchiveReading::IsLoading() const
{
	return true;
}

// ====================================
// Archive writing
// ====================================

/**
 * Constructor
 */
CWindowsArchiveWriter::CWindowsArchiveWriter( std::ofstream* InFile, const std::wstring& InPath )
	: CArchive( InPath )
	, file( InFile )
{}

/**
 * Destructor
 */
CWindowsArchiveWriter::~CWindowsArchiveWriter()
{
	Flush();
	delete file;
}

/**
 * Get size of archive
 */
uint32 CWindowsArchiveWriter::GetSize()
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
void CWindowsArchiveWriter::Seek( uint32 InPosition )
{
	Flush();
	file->seekp( InPosition, std::ios::beg );
}

/**
 * Flush data
 */
void CWindowsArchiveWriter::Flush()
{
	file->flush();
}

/**
 * Get current position in archive
 */
uint32 CWindowsArchiveWriter::Tell()
{
	Flush();
	return ( uint32 )file->tellp();
}

/**
 * Serialize data
 */
void CWindowsArchiveWriter::Serialize( void* InBuffer, uint32 InSize )
{
	file->write( ( achar* )InBuffer, InSize );
	Flush();
}

bool CWindowsArchiveWriter::IsEndOfFile()
{
	uint32		sizeFile = GetSize();
	return Tell() == sizeFile;
}

/**
 * Is saving archive
 */
bool CWindowsArchiveWriter::IsSaving() const
{
	return true;
}