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

#ifndef FILE_STD_H
#define FILE_STD_H

#include <stdio.h>

#include "core/platform.h"
#include "filesystem/file_base.h"

/**
 * @ingroup filesystem
 * @brief Std file reader
 */
class CStdFileReader : public CBaseFileReader
{
public:
	/**
	 * @brief Constructor
	 * 
	 * @param pFile	Pointer to std file
	 * @param path	Path to file
	 */
	CStdFileReader( FILE* pFile, const std::string& path );

	/**
	 * @brief Destructor
	 */
	virtual ~CStdFileReader();

	/**
	 * @brief Read data
	 *
	 * @param pBuffer	Pointer to buffer for read
	 * @param size		Size of buffer
	 */
	virtual void Read( void* pBuffer, uint64 size ) override;

	/**
	 * @brief Get current position in file
	 * @return Return current position in file
	 */
	virtual uint64 Tell() const override;

	/**
	 * @brief Set current position in file
	 * @param position	New position in file
	 */
	virtual void Seek( uint64 position ) override;

	/**
	 * @brief Flush data
	 */
	virtual void Flush() override;

	/**
	 * @breif Is end of file
	 * @return Return TRUE if the end of file, otherwise will return FALSE
	 */
	virtual bool IsEndOfFile() const override;

	/**
	 * @brief Get size of file
	 * @return Return file size
	 */
	virtual uint64 GetSize() const override;

	/**
	 * @brief Get file handle
	 * @return Return pointer to std file
	 */
	FORCEINLINE FILE* GetHandle() const
	{
		return pFile;
	}

private:
	FILE*	pFile;	/**< Pointer to file */
};

/**
 * @ingroup filesystem
 * @brief Std file writer
 */
class CStdFileWriter : public CBaseFileWriter
{
public:
	/**
	 * @brief Constructor
	 *
	 * @param pFile	Pointer to std file
	 * @param path	Path to file
	 */
	CStdFileWriter( FILE* pFile, const std::string& path );

	/**
	 * @brief Destructor
	 */
	virtual ~CStdFileWriter();

	/**
	 * @brief Write data
	 *
	 * @param pBuffer	Pointer to buffer for write
	 * @param size		Size of buffer
	 */
	virtual void Write( void* pBuffer, uint64 size ) override;

	/**
	 * @brief Get current position in file
	 * @return Return current position in file
	 */
	virtual uint64 Tell() const override;

	/**
	 * @brief Set current position in file
	 * @param position	New position in file
	 */
	virtual void Seek( uint64 position ) override;

	/**
	 * @brief Flush data
	 */
	virtual void Flush() override;

	/**
	 * @breif Is end of file
	 * @return Return TRUE if the end of file, otherwise will return FALSE
	 */
	virtual bool IsEndOfFile() const override;

	/**
	 * @brief Get size of file
	 * @return Return file size
	 */
	virtual uint64 GetSize() const override;

	/**
	 * @brief Get file handle
	 * @return Return pointer to std file
	 */
	FORCEINLINE FILE* GetHandle() const
	{
		return pFile;
	}

private:
	FILE*	pFile;	/**< Pointer to file */
};

#endif // !FILE_STD_H