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

#ifndef IFILE_H
#define IFILE_H

#include "core/types.h"
#include "core/compression.h"
#include "stdlib/refcount.h"

/**
 * @ingroup filesystem
 * @brief File interface
 */
class IFile : public IRefCounted
{
public:
	/**
	 * @brief Get current position in file
	 * @return Return current position in file
	 */
	virtual uint64 Tell() const = 0;

	/**
	 * @brief Set current position in file
	 * @param position	New position in file
	 */
	virtual void Seek( uint64 position ) = 0;

	/**
	 * @brief Flush data
	 */
	virtual void Flush() = 0;

	/**
	 * @brief Is this file writer
	 * @return Return TRUE if this file is writer, otherwise returns FALSE
	 */
	virtual bool IsWriter() const = 0;

	/**
	 * @breif Is this file reader
	 * @return Return TRUE if this file is reader, otherwise returns FALSE
	 */
	virtual bool IsReader() const = 0;

	/**
	 * @breif Is end of file
	 * @return Return TRUE if the end of file, otherwise will return FALSE
	 */
	virtual bool IsEndOfFile() const = 0;

	/**
	 * @brief Get size of file
	 * @return Return file size
	 */
	virtual uint64 GetSize() const = 0;

	/**
	 * @brief Get path to file
	 * @return Return path to file
	 */
	virtual const achar* GetPath() const = 0;
};

/**
 * @ingroup filesystem
 * @brief Reader file interface
 */
class IFileReader : public IFile
{
public:
	/**
	 * @brief Read data
	 *
	 * @param pBuffer	Pointer to buffer for read
	 * @param size		Size of buffer
	 */
	virtual void Read( void* pBuffer, uint64 size ) = 0;

	/**
	 * @brief Read compression data
	 *
	 * @param pBuffer	Pointer to buffer for read
	 * @param size		Size of buffer
	 * @param flags		Compression flags (see ECompressionFlags)
	 */
	virtual void ReadCompressed( void* pBuffer, uint64 size, ECompressionFlags flags ) = 0;
};

/**
 * @ingroup filesystem
 * @brief Writer file interface
 */
class IFileWriter : public IFile
{
public:
	/**
	 * @brief Write data
	 *
	 * @param pBuffer	Pointer to buffer for write
	 * @param size		Size of buffer
	 */
	virtual void Write( void* pBuffer, uint64 size ) = 0;

	/**
	 * @brief Write compression data
	 *
	 * @param pBuffer	Pointer to buffer for write
	 * @param size		Size of buffer
	 * @param flags		Compression flags (see ECompressionFlags)
	 */
	virtual void WriteCompressed( void* pBuffer, uint64 size, ECompressionFlags flags ) = 0;
};

#endif // !IFILE_H