#include "System/MemoryArchive.h"

CMemoryArchive::CMemoryArchive( std::vector<byte>& InData, const std::wstring& InPath )
	: CArchive( InPath )
	, data( InData )
	, offset( 0 )
{
	SetType( AT_BinaryFile );
}

uint32 CMemoryArchive::Tell()
{
	return offset;
}

void CMemoryArchive::Seek( uint32 InPosition )
{
	offset = Min( InPosition, GetSize() );
}

void CMemoryArchive::Flush()
{}

bool CMemoryArchive::IsEndOfFile()
{
	uint32		sizeFile = GetSize();
	return Tell() == sizeFile;
}

uint32 CMemoryArchive::GetSize()
{
	return data.size();
}

CMemoryReading::CMemoryReading( std::vector<byte>& InData, const std::wstring& InPath /* = TEXT( "NOT_USED" ) */ )
	: CMemoryArchive( InData, InPath )
{}

void CMemoryReading::Serialize( void* InBuffer, uint32 InSize )
{
	check( offset <= GetSize() - InSize );
	memcpy( InBuffer, data.data() + offset, InSize );
	offset += InSize;
}

bool CMemoryReading::IsLoading() const
{
	return true;
}

CMemoryWriter::CMemoryWriter( std::vector<byte>& InData, const std::wstring& InPath /* = TEXT( "NOT_USED" ) */ )
	: CMemoryArchive( InData, InPath )
{}

void CMemoryWriter::Serialize( void* InBuffer, uint32 InSize )
{
	if ( offset + InSize > GetSize() )
	{
		data.resize( GetSize() + InSize );
	}

	memcpy( data.data() + offset, InBuffer, InSize );
	offset += InSize;
}

bool CMemoryWriter::IsSaving() const
{
	return true;
}