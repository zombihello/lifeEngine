/**
 * @file
 * @addtogroup Core Core
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

#ifndef MEMORYARCHIVE_H
#define MEMORYARCHIVE_H

#include <vector>
#include "Core/System/Archive.h"

/**
 * @ingroup Core
 * @brief The base class for serialize data to memory
 */
class CMemoryArchive : public CArchive
{
public:
	/**
	 * @brief Constructor
	 *
	 * @param InFile Link to file
	 * @param InPath Path to archive
	 */
	CMemoryArchive( std::vector<byte>& InData, const std::wstring& InPath = TEXT( "NOT_USED" ) );

	/**
	 * @brief Get current position in archive
	 * @return Current position in archive
	 */
	virtual uint32 Tell() override;

	/**
	 * @brief Set current position in archive
	 *
	 * @param[in] InPosition New position in archive
	 */
	virtual void Seek( uint32 InPosition ) override;

	/**
	 * @brief Flush data
	 */
	virtual void Flush() override;

	/**
	 * Is end of file
	 * @return Return true if end of file, else return false
	 */
	virtual bool IsEndOfFile() override;

	/**
	 * @brief Get size of archive
	 * @return Size of archive
	 */
	virtual uint32 GetSize() override;

protected:
	std::vector<byte>&		data;		/**< Array with data */
	uint32					offset;		/**< Offset in data array */
};

/**
 * @ingroup Core
 * @brief The class for reading data from memory
 */
class CMemoryReading : public CMemoryArchive
{
public:
	/**
	 * @brief Constructor
	 *
	 * @param InFile Link to file
	 * @param InPath Path to archive
	 */
	CMemoryReading( std::vector<byte>& InData, const std::wstring& InPath = TEXT( "NOT_USED" ) );

	/**
	 * @brief Serialize data
	 *
	 * @param[in] InBuffer Pointer to buffer for serialize
	 * @param[in] InSize Size of buffer
	 */
	virtual void Serialize( void* InBuffer, uint32 InSize ) override;

	/**
	 * @breif Is loading archive
	 * @return True if archive loading, false if archive saving
	 */
	virtual bool IsLoading() const;
};

/**
 * @ingroup Core
 * @brief The class for write data to memory
 */
class CMemoryWriter : public CMemoryArchive
{
public:
	/**
	 * @brief Constructor
	 *
	 * @param InFile Link to file
	 * @param InPath Path to archive
	 */
	CMemoryWriter( std::vector<byte>& InData, const std::wstring& InPath = TEXT( "NOT_USED" ) );

	/**
	 * @brief Serialize data
	 *
	 * @param[in] InBuffer Pointer to buffer for serialize
	 * @param[in] InSize Size of buffer
	 */
	virtual void Serialize( void* InBuffer, uint32 InSize ) override;

	/**
	 * @brief Is saving archive
	 * @return True if archive saving, false if archive loading
	 */
	virtual bool IsSaving() const;
};

#endif // !MEMORYARCHIVE_H