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

#include <memory.h>
#include <Windows.h>

#include "Core/Core.h"
#include "Core/Misc/Template.h"
#include "Core/Misc/Class.h"
#include "Core/Platforms/Windows/WindowsArchive.h"

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