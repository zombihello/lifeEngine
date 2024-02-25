/**
 * @file
 * @addtogroup filesystem filesystem
 *
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

#ifndef FILE_BASE_H
#define FILE_BASE_H

#include <string>
#include "filesystem/ifile.h"

/**
 * @ingroup filesystem
 * @brief Base file to implement file
 */
template<class TBaseClass>
class CBaseFile : public TBaseClass
{
public:
	/**
	 * @brief Constructor
	 * @param path	Path to file
	 */
	CBaseFile( const std::string& path )
		: path( path )
	{}

	/**
	 * @brief Is this file writer
	 * @return Return TRUE if this file is writer, otherwise returns FALSE
	 */
	virtual bool IsWriter() const override
	{
		return false;
	}

	/**
	 * @breif Is this file reader
	 * @return Return TRUE if this file is reader, otherwise returns FALSE
	 */
	virtual bool IsReader() const override
	{
		return false;
	}

	/**
	 * @brief Get path to file
	 * @return Return path to file
	 */
	virtual const achar* GetPath() const override
	{
		return !path.empty() ? path.c_str() : "";
	}

private:
	std::string		path;	/**< Path to file */
};

/**
 * @ingroup filesystem
 * @brief Base file reader
 */
class CBaseFileReader : public CBaseFile<TRefCounted<IFileReader>>
{
public:
	/**
	 * @brief Constructor
	 * @param path	Path to file
	 */
	CBaseFileReader( const std::string& path )
		: CBaseFile<TRefCounted<IFileReader>>( path )
	{}

	/**
	 * @brief Read compression data
	 *
	 * @param pBuffer	Pointer to buffer for read
	 * @param size		Size of buffer
	 * @param flags		Compression flags (see ECompressionFlags)
	 */
	virtual void ReadCompressed( void* pBuffer, uint64 size, ECompressionFlags flags ) override;

	/**
	 * @breif Is this file reader
	 * @return Return TRUE if this file is reader, otherwise returns FALSE
	 */
	virtual bool IsReader() const override
	{
		return true;
	}
};

/**
 * @ingroup filesystem
 * @brief Base file writer
 */
class CBaseFileWriter : public CBaseFile<TRefCounted<IFileWriter>>
{
public:
	/**
	 * @brief Constructor
	 * @param path	Path to file
	 */
	CBaseFileWriter( const std::string& path )
		: CBaseFile<TRefCounted<IFileWriter>>( path )
	{}

	/**
	 * @brief Write compression data
	 *
	 * @param pBuffer	Pointer to buffer for write
	 * @param size		Size of buffer
	 * @param flags		Compression flags (see ECompressionFlags)
	 */
	virtual void WriteCompressed( void* pBuffer, uint64 size, ECompressionFlags flags ) override;

	/**
	 * @brief Is this file writer
	 * @return Return TRUE if this file is writer, otherwise returns FALSE
	 */
	virtual bool IsWriter() const override
	{
		return true;
	}
};

#endif // !FILE_BASE_H