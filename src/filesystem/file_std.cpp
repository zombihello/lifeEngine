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

#include "pch_filesystem.h"
#include "filesystem/file_std.h"

/*
==================
CStdFileReader::CStdFileReader
==================
*/
CStdFileReader::CStdFileReader( FILE* pFile, const std::string& path )
	: CBaseFileReader( path )
	, pFile( pFile )
{}

/*
==================
CStdFileReader::~CStdFileReader
==================
*/
CStdFileReader::~CStdFileReader()
{
	fclose( pFile );
}

/*
==================
CStdFileReader::GetSize
==================
*/
uint64 CStdFileReader::GetSize() const
{
	uint64	currentPosition = Tell();

	fseek( pFile, 0, SEEK_END );
	uint64	sizeFile = Tell();
	fseek( pFile, ( long )currentPosition, SEEK_SET );
	return sizeFile;
}

/*
==================
CStdFileReader::Seek
==================
*/
void CStdFileReader::Seek( uint64 position )
{
	fseek( pFile, ( long )position, SEEK_SET );
}

/*
==================
CStdFileReader::Flush
==================
*/
void CStdFileReader::Flush()
{}

/*
==================
CStdFileReader::Tell
==================
*/
uint64 CStdFileReader::Tell() const
{
	return ftell( pFile );
}

/*
==================
CStdFileReader::Read
==================
*/
void CStdFileReader::Read( void* pBuffer, uint64 size )
{
	fread( pBuffer, size, 1, pFile );
}

/*
==================
CStdFileReader::IsEndOfFile
==================
*/
bool CStdFileReader::IsEndOfFile() const
{
	return feof( pFile ) != 0;
}

/*
==================
CStdFileWriter::CStdFileWriter
==================
*/
CStdFileWriter::CStdFileWriter( FILE* pFile, const std::string& path )
	: CBaseFileWriter( path )
	, pFile( pFile )
{}

/*
==================
CStdFileWriter::~CStdFileWriter
==================
*/
CStdFileWriter::~CStdFileWriter()
{
	Flush();
	fclose( pFile );
}

/*
==================
CStdFileWriter::GetSize
==================
*/
uint64 CStdFileWriter::GetSize() const
{
	// Make sure that all data is written before looking at file size
	const_cast<CStdFileWriter*>( this )->Flush();
	uint64	currentPosition = Tell();

	fseek( pFile, 0, SEEK_END );
	uint64	sizeFile = Tell();
	fseek( pFile, ( long )currentPosition, SEEK_SET );
	return sizeFile;
}

/*
==================
CStdFileWriter::Seek
==================
*/
void CStdFileWriter::Seek( uint64 position )
{
	Flush();
	fseek( pFile, ( long )position, SEEK_SET );
}

/*
==================
CStdFileWriter::Flush
==================
*/
void CStdFileWriter::Flush()
{
	fflush( pFile );
}

/*
==================
CStdFileWriter::Tell
==================
*/
uint64 CStdFileWriter::Tell() const
{
	const_cast<CStdFileWriter*>( this )->Flush();
	return ftell( pFile );
}

/*
==================
CStdFileWriter::Write
==================
*/
void CStdFileWriter::Write( void* pBuffer, uint64 size )
{
	fwrite( pBuffer, size, 1, pFile );
	Flush();
}

/*
==================
CStdFileWriter::IsEndOfFile
==================
*/
bool CStdFileWriter::IsEndOfFile() const
{
	return feof( pFile ) != 0;
}