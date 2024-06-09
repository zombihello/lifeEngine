#include <memory.h>
#include <Windows.h>

#include "Core.h"
#include "Misc/Template.h"
#include "Reflection/Class.h"
#include "WindowsArchive.h"

// ====================================
// Archive reading
// ====================================

/*
==================
CWindowsArchiveReading::CWindowsArchiveReading
==================
*/
CWindowsArchiveReading::CWindowsArchiveReading( std::ifstream* InFile, const std::wstring& InPath )
	: CArchive( InPath )
	, file( InFile )
{}

/*
==================
CWindowsArchiveReading::~CWindowsArchiveReading
==================
*/
CWindowsArchiveReading::~CWindowsArchiveReading()
{
	delete file;
}

/*
==================
CWindowsArchiveReading::GetSize
==================
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

/*
==================
CWindowsArchiveReading::Seek
==================
*/
void CWindowsArchiveReading::Seek( uint32 InPosition )
{
	file->seekg( InPosition, std::ios::beg );
}

/*
==================
CWindowsArchiveReading::Flush
==================
*/
void CWindowsArchiveReading::Flush()
{}

/*
==================
CWindowsArchiveReading::Tell
==================
*/
uint32 CWindowsArchiveReading::Tell()
{
	return ( uint32 )file->tellg();
}

/*
==================
CWindowsArchiveReading::Serialize
==================
*/
void CWindowsArchiveReading::Serialize( void* InBuffer, uint32 InSize )
{
	file->read( ( achar* )InBuffer, InSize );
}

/*
==================
CWindowsArchiveReading::IsEndOfFile
==================
*/
bool CWindowsArchiveReading::IsEndOfFile()
{
	uint32		sizeFile = GetSize();
	return Tell() == sizeFile;
}

/*
==================
CWindowsArchiveReading::IsLoading
==================
*/
bool CWindowsArchiveReading::IsLoading() const
{
	return true;
}

// ====================================
// Archive writing
// ====================================

/*
==================
CWindowsArchiveWriter::CWindowsArchiveWriter
==================
*/
CWindowsArchiveWriter::CWindowsArchiveWriter( std::ofstream* InFile, const std::wstring& InPath )
	: CArchive( InPath )
	, file( InFile )
{}

/*
==================
CWindowsArchiveWriter::~CWindowsArchiveWriter
==================
*/
CWindowsArchiveWriter::~CWindowsArchiveWriter()
{
	Flush();
	delete file;
}

/*
==================
CWindowsArchiveWriter::GetSize
==================
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

/*
==================
CWindowsArchiveWriter::Seek
==================
*/
void CWindowsArchiveWriter::Seek( uint32 InPosition )
{
	Flush();
	file->seekp( InPosition, std::ios::beg );
}

/*
==================
CWindowsArchiveWriter::Flush
==================
*/
void CWindowsArchiveWriter::Flush()
{
	file->flush();
}

/*
==================
CWindowsArchiveWriter::Tell
==================
*/
uint32 CWindowsArchiveWriter::Tell()
{
	Flush();
	return ( uint32 )file->tellp();
}

/*
==================
CWindowsArchiveWriter::Serialize
==================
*/
void CWindowsArchiveWriter::Serialize( void* InBuffer, uint32 InSize )
{
	file->write( ( achar* )InBuffer, InSize );
	Flush();
}

/*
==================
CWindowsArchiveWriter::IsEndOfFile
==================
*/
bool CWindowsArchiveWriter::IsEndOfFile()
{
	uint32		sizeFile = GetSize();
	return Tell() == sizeFile;
}

/*
==================
CWindowsArchiveWriter::IsSaving
==================
*/
bool CWindowsArchiveWriter::IsSaving() const
{
	return true;
}