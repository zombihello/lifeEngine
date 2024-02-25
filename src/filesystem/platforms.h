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

#ifndef FILESYSTEM_PLATFORMS_H
#define FILESYSTEM_PLATFORMS_H

#include <string>
#include <vector>

#include "core/types.h"
#include "core/platform.h"
#include "filesystem/ifilesystem.h"

/**
 * @ingroup filesystem
 * @brief Platform implementation to create file reader
 * @note Need implement on each platform
 * 
 * @param pPath         Path to file
 * @param flags         Combinations flags of EFileRead for open mode
 * @return Return pointer to file reader, if a file isn't open returns NULL
 */
TRefPtr<IFileReader> Plat_CreateFileReader( const achar* pPath, uint32 flags = FILE_READ_NONE );

/**
 * @ingroup filesystem
 * @brief Platform implementation to create file writer
 * @note Need implement on each platform
 *
 * @param pPath         Path to file
 * @param flags         Combinations flags of EFileWrite for opening mode
 * @return Return pointer to file writer, if a file isn't open returns NULL
 */
TRefPtr<IFileWriter> Plat_CreateFileWriter( const achar* pPath, uint32 flags = FILE_WRITE_NONE );

/**
 * @ingroup filesystem
 * @brief Platform implementation to make directory
 * @note Need implement on each platform
 * 
 * @param pPath     Path to directory
 * @return Return TRUE if directory is successfully made, otherwise returns FALSE
 */
bool Plat_MakeDirectory( const achar* pPath );

/**
 * @ingroup filesystem
 * @brief Platform implementation to delete directory
 * @note Need implement on each platform
 * 
 * @param pPath         Path to directory
 * @param bEvenReadOnly Delete a directory even if it is read-only or a file in it
 * @return Return TRUE if directory is successfully deleted, otherwise returns FALSE
 */
bool Plat_DeleteDirectory( const achar* pPath, bool bEvenReadOnly = false );

/**
 * @ingroup filesystem
 * @brief Platform implementation to find files and directories by the path
 * @note Need implement on each platform
 *
 * @param pPath         Path to directory
 * @param bFiles        Whether to search files
 * @param bDirectories  Whether to search directories
 * @param foundFiles    Output array of paths with all found files and directories. NOTE: This function don't clear the array before use!
 * @return Return TRUE if any files were found, otherwise returns FALSE
 */
bool Plat_FindFiles( const achar* pPath, bool bFiles, bool bDirectories, std::vector<std::string>& foundFiles );

/**
 * @ingroup filesystem
 * @brief Platform implementation to delete file
 * @note Need implement on each platform
 * 
 * @param pPath             Path to file
 * @param bEvenReadOnly     Delete file even it's read only
 * @return Return TRUE if file is successfully deleted, otherwise returns FALSE
 */
bool Plat_DeleteFile( const achar* pPath, bool bEvenReadOnly = false );

/**
 * @ingroup filesystem
 * @brief Platform implementation to copy file
 * @note Need implement on each platform
 *
 * @param pSrcPath          Source path
 * @param pDestPath         Destination path
 * @param bReplaceExisting  Is need replace existing file
 * @param bEvenReadOnly     Is even read only
 * @return Return copy result (see ECopyMoveResult)
 */
ECopyMoveResult Plat_CopyFile( const achar* pSrcPath, const achar* pDestPath, bool bReplaceExisting = false, bool bEvenReadOnly = false );

/**
 * @ingroup filesystem
 * @brief Platform implementation to check is exists file or directory
 * @note Need implement on each platform
 *
 * @param pPath   Path to directory or file
 * @return Return TRUE if file or directory is exist, otherwise FALSE
 */
bool Plat_IsFileExists( const achar* pPath );

/**
 * @ingroup filesystem
 * @brief Platform implementation to check is this file a directory
 * @note Need implement on each platform
 *
 * @param pPath   Path to file
 * @return Return TRUE if a file is directory, otherwise returns FALSE
 */
bool Plat_IsFileDirectory( const achar* pPath );

/**
 * @ingroup filesystem
 * @brief Platform implementation to move file
 * @note Need implement on each platform
 *
 * @param pSrcPath               Source path
 * @param pDestPath              Destination path
 * @param bReplaceExisting       Is need replace existing file
 * @param bEvenReadOnly          Is even read only
 * @return Return move result (see ECopyMoveResult)
 */
ECopyMoveResult Plat_MoveFile( const achar* pSrcPath, const achar* pDestPath, bool bReplaceExisting = false, bool bEvenReadOnly = false );

// Include implementation of platform specific inline functions
#if PLATFORM_WINDOWS
	#include "filesystem/platforms/windows/win_filesystem.inl"
#endif // PLATFORM_WINDOWS

#endif // FILESYSTEM_PLATFORMS_H