/**
 * ************************************************************
 *                  This file is part of:
 *                      LIFEENGINE
 *          https://github.com/zombihello/lifeEngine
 * ************************************************************
 * Copyright (C) 2024 Yehor Pohuliaka.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "Core/System/MemoryArchive.h"

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