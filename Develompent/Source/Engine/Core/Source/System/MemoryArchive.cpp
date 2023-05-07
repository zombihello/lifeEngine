#include "System/MemoryArchive.h"

/*
==================
CMemoryArchive::CMemoryArchive
==================
*/
CMemoryArchive::CMemoryArchive( std::vector<byte>& InData, const std::wstring& InPath )
	: CArchive( InPath )
	, data( InData )
	, offset( 0 )
{
	SetType( AT_BinaryFile );
}

/*
==================
CMemoryArchive::Tell
==================
*/
uint32 CMemoryArchive::Tell()
{
	return offset;
}

/*
==================
CMemoryArchive::Seek
==================
*/
void CMemoryArchive::Seek( uint32 InPosition )
{
	offset = Min( InPosition, GetSize() );
}

/*
==================
CMemoryArchive::Flush
==================
*/
void CMemoryArchive::Flush()
{}

/*
==================
CMemoryArchive::IsEndOfFile
==================
*/
bool CMemoryArchive::IsEndOfFile()
{
	uint32		sizeFile = GetSize();
	return Tell() == sizeFile;
}

/*
==================
CMemoryArchive::GetSize
==================
*/
uint32 CMemoryArchive::GetSize()
{
	return data.size();
}


/*
==================
CMemoryReading::CMemoryReading
==================
*/
CMemoryReading::CMemoryReading( std::vector<byte>& InData, const std::wstring& InPath /* = TEXT( "NOT_USED" ) */ )
	: CMemoryArchive( InData, InPath )
{}

/*
==================
CMemoryReading::Serialize
==================
*/
void CMemoryReading::Serialize( void* InBuffer, uint32 InSize )
{
	Assert( offset <= GetSize() - InSize );
	memcpy( InBuffer, data.data() + offset, InSize );
	offset += InSize;
}

/*
==================
CMemoryReading::IsLoading
==================
*/
bool CMemoryReading::IsLoading() const
{
	return true;
}

/*
==================
CMemoryWriter::CMemoryWriter
==================
*/
CMemoryWriter::CMemoryWriter( std::vector<byte>& InData, const std::wstring& InPath /* = TEXT( "NOT_USED" ) */ )
	: CMemoryArchive( InData, InPath )
{}

/*
==================
CMemoryWriter::Serialize
==================
*/
void CMemoryWriter::Serialize( void* InBuffer, uint32 InSize )
{
	if ( offset + InSize > GetSize() )
	{
		data.resize( GetSize() + InSize );
	}

	memcpy( data.data() + offset, InBuffer, InSize );
	offset += InSize;
}

/*
==================
CMemoryWriter::IsSaving
==================
*/
bool CMemoryWriter::IsSaving() const
{
	return true;
}