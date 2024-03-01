/**
 * @file
 * @addtogroup stdlib stdlib
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

#ifndef FILETOOLS_H
#define FILETOOLS_H

#include <string>

#include "core/platform.h"
#include "core/types.h"

/**
 * @ingroup stdlib
 * @brief Is char a path separator
 *
 * @param c   Char to check
 * @return Return TRUE if c is a path separator, otherwise FALSE
 */
FORCEINLINE bool L_IsPathSeparator( achar c )
{
    return c == '\\' || c == '/';
}

/**
 * @ingroup stdlib
 * @brief Set current directory
 * @note Need implement on each platform
 *
 * @param pDirName     Absolute path to a directory
 * @return Return TRUE if current directory was successful changed, otherwise returns FALSE
 */
FORCEINLINE bool L_SetCurrentDirectory( const achar* pDirName );

/**
 * @ingroup stdlib
 * @brief Set current directory
 *
 * @param dirName     Absolute path to a directory
 * @return Return TRUE if current directory was successful changed, otherwise returns FALSE
 */
FORCEINLINE bool L_SetCurrentDirectory( const std::string& dirName )
{
    return L_SetCurrentDirectory( dirName.c_str() );
}

/**
 * @ingroup stdlib
 * @brief Get absolute path to the current directory
 * @note Need implement on each platform
 *
 * @param pDestStr		Destination string. Must be not-NULL
 * @param maxLen		Max size of pDestStr. Must be greater than zero 
 * @return Return TRUE upon success, FALSE if fails because pDestStr was too small
 */
FORCEINLINE bool L_GetCurrentDirectory( achar* pDestStr, uint32 maxLen );

/**
 * @ingroup stdlib
 * @brief Get absolute path to the current directory
 * 
 * @param destStr	    Destination string
 * @param bShrinkToFit  If TRUE, remove all extra null terminator characters in destStr
 */
void L_GetCurrentDirectory( std::string& destStr, bool bShrinkToFit = true );

/**
 * @ingroup stdlib
 * @brief Make absolute path from relative
 *
 * @param pSrcPath		Relative path
 * @param pDestPath		Output absolute path
 * @param maxLen		Max size of pDestPath
 * @param pStartingDir	Using the current working directory as the base, or pStartingDir if it's non-NULL
 * @return Return TRUE if the absolute path was successfully created. Returns FALSE if it runs out of room in the string
 */
bool L_MakeAbsolutePath( const achar* pSrcPath, achar* pDestPath, uint32 maxLen, const achar* pStartingDir = nullptr );

/**
 * @ingroup stdlib
 * @brief Make absolute path from relative
 *
 * @param srcPath		Relative path
 * @param destPath		Output absolute path
 * @param startingDir	Using the current working directory as the base, or startingDir if it isn't empty
 * @param bShrinkToFit  If TRUE, remove all extra null terminator characters in destPath
 */
void L_MakeAbsolutePath( const std::string& srcPath, std::string& destPath, const std::string& startingDir = "", bool bShrinkToFit = true );

/**
 * @ingroup stdlib
 * @brief Make relative path from two full paths
 *
 * @param pFullPath			The full path of the file to make a relative path for (Must be absolute path)
 * @param pDirPath			The full path of the directory to make pFullPath relative to (Must be absolute path)
 * @param pRelativePath		Output relative path
 * @param maxLen			Max size of pRelativePath
 * @return Return result code. 0 - the relative path was successfully created; 1 - can't make relative because pDirPath too small; 2 - can't make relative because on separate drives or something else
 */
int32 L_MakeRelativePath( const achar* pFullPath, const achar* pDirPath, achar* pRelativePath, uint32 maxLen );

/**
 * @ingroup stdlib
 * @brief Make relative path from two full paths
 *
 * @param fullPath			The full path of the file to make a relative path for (Must be absolute path)
 * @param dirPath			The full path of the directory to make fullPath relative to (Must be absolute path)
 * @param relativePath		Output relative path
 * @param bShrinkToFit      If TRUE, remove all extra null terminator characters in relativePath
 * @return Return TRUE if the relative path was successfully created. Returns FALSE if can't make relative (on separate drives, for example
 */
bool L_MakeRelativePath( const std::string& fullPath, const std::string& dirPath, std::string& relativePath, bool bShrinkToFit = true );

/**
 * @ingroup stdlib
 * @brief Is absolute path
 * @note Need implement on each platform
 * 
 * @param pPath		Path to check
 * @return Return TRUE if the path is an absolute path, otherwise FALSE
 */
FORCEINLINE bool L_IsAbsolutePath( const achar* pPath );

/**
 * @ingroup stdlib
 * @brief Is absolute path
 *
 * @param path		Path to check
 * @return Return TRUE if the path is an absolute path, otherwise FALSE
 */
FORCEINLINE bool L_IsAbsolutePath( const std::string& path )
{
    return L_IsAbsolutePath( path.c_str() );
}

/**
 * @ingroup stdlib
 * @brief Adds a path separator to the end of the string if there isn't one already

 * @param pStr      String
 * @param strSize   String size
 * @return Return TRUE if a path separator was added successfully to the end of the string or it's already here, FALSE if run out of space
 */
bool L_AppendPathSeparator( achar* pStr, uint32 strSize );

/**
 * @ingroup stdlib
 * @brief Adds a path separator to the end of the string if there isn't one already
 * @param str  String
 */
void L_AppendPathSeparator( std::string& str );

/**
 * @ingroup stdlib
 * @brief Fix path separators in the path
 * @param pPath     Path to fix
 */
void L_FixPathSeparators( achar* pPath );

/**
 * @ingroup stdlib
 * @brief Fix path separators in the path
 * @param path    Path to fix
 */
FORCEINLINE void L_FixPathSeparators( std::string& path )
{
    L_FixPathSeparators( path.data() );
}

/**
 * @ingroup stdlib
 * @brief Get the file extension from a path
 *
 * @param pPath	      Path
 * @param bIncludeDot If TRUE, includes the leading dot in the result
 * @return Returns a pointer to beginning of extension (after the "."), or NULL if there is no extension
 */
const achar* L_GetFileExtension( const achar* pPath, bool bIncludeDot = false );

/**
 * @ingroup stdlib
 * @brief Get the file extension from a path
 *
 * @param path	      Path
 * @param extension   Output a file extension (after the "."). Empty if there is no extension
 * @param bIncludeDot If TRUE, includes the leading dot in the result
 */
FORCEINLINE void L_GetFileExtension( const std::string& path, std::string& extension, bool bIncludeDot = false )
{
    extension = L_GetFileExtension( path.c_str(), bIncludeDot );
}

/**
 * @ingroup stdlib
 * @brief Get the base file name from a path
 *
 * @param pPath			Path
 * @param pFileName		Output base file name
 * @param maxLen		Max size of pFileName
 * @return Return TRUE if the base file name was successfully copy, or FALSE if pFileName was too small
 */
bool L_GetFileBaseName( const achar* pPath, achar* pFileName, uint32 maxLen );

/**
 * @ingroup stdlib
 * @brief Get the base file name from a path
 *
 * @param path		    Path
 * @param fileName	    Output base file name
 * @param bShrinkToFit  If TRUE, remove all extra null terminator characters in fileName
 */
void L_GetFileBaseName( const std::string& path, std::string& fileName, bool bShrinkToFit = true );

/**
 * @ingroup stdlib
 * @brief Get the file name from a path
 *
 * @param pPath		Path
 * @return Returns a pointer to just the filename part of the path (everything after the last path seperator)
 */
const achar* L_GetFileName( const achar* pPath );

/**
 * @ingroup stdlib
 * @brief Get the file name from a path
 *
 * @param path		Path
 * @param fileName  Output file name (everything after the last path seperator)
 */
FORCEINLINE void L_GetFileName( const std::string& path, std::string& fileName )
{
    fileName = L_GetFileName( path.c_str() );
}

/**
 * @ingroup stdlib
 * @brief Get the path to file from a full path
 *
 * @param pSrcPath		Full path
 * @param pDestPath		Output the path to file. Copy to this all the path except for the stuff after the final path separator
 * @param maxLen		Max size of pDestPath
 * @return Return TRUE if the path to file was successfully copy, or FALSE if pDestPath was too small
 */
bool L_GetFilePath( const achar* pSrcPath, achar* pDestPath, uint32 maxLen );

/**
 * @ingroup stdlib
 * @brief Get the path to file from a full path
 *
 * @param srcPath	    Full path
 * @param destPath	    Output the path to file. Copy to this all the path except for the stuff after the final path separator
 * @param bShrinkToFit  If TRUE, remove all extra null terminator characters in destPath
 */
void L_GetFilePath( const std::string& srcPath, std::string& destPath, bool bShrinkToFit = true );

/**
 * @ingroup stdlib
 * @brief Removes "./" and "../" from the path
 * 
 * @param pPath                     Path to fix, should be a full path
 * @param bRemoveDoubleSeparators   Is need remove double separators
 */
void L_RemoveDotPathSeparators( achar* pPath, bool bRemoveDoubleSeparators = true );

/**
 * @ingroup stdlib
 * @brief Removes "./" and "../" from the path
 *
 * @param path                      Path to fix, should be a full path
 * @param bRemoveDoubleSeparators   Is need remove double separators
 */
FORCEINLINE void L_RemoveDotPathSeparators( std::string& path, bool bRemoveDoubleSeparators = true )
{
    L_RemoveDotPathSeparators( path.data(), bRemoveDoubleSeparators );
}

/**
 * @ingroup stdlib
 * @brief Utility class for quick inquiries against filenames
 */
class CFilename
{
public:
    /**
     * @brief Constructor
     */
    CFilename();

    /**
     * @brief Constructor
     * @param path   Path to the file
     */
    CFilename( const std::string& path );

    /**
     * @brief Get file's extension
     *
     * @param bIncludeDot     If TRUE, includes the leading dot in the result
     * @return Return the extension of this filename, or an empty string if the filename doesn't have an extension
     */
    FORCEINLINE std::string GetExtension( bool bIncludeDot = false ) const
    {
        return L_GetFileExtension( path.c_str(), bIncludeDot );
    }

    /**
     * @brief Get full path
     * @return Return full path with extension
     */
    FORCEINLINE const std::string& GetFullPath() const
    {
        return path;
    }

    /**
     * @brief Get file name without without extension and any path information
     * @return Return base file name (without extension and any path information)
     */
    FORCEINLINE std::string GetBaseName() const
    {
		std::string		result;
		L_GetFileBaseName( path, result );
		return result;
    }

    /**
     * @brief Get file name with extension
     * @return Return file name with extension
     */
    FORCEINLINE std::string GetFileName() const
    {
		std::string		result;
		L_GetFileName( path, result );
		return result;
    }

    /**
     * @brief Get path to the file
     * @return Return path to the file
     */
    FORCEINLINE std::string GetPath() const
    {
		std::string		result;
		L_GetFilePath( path, result );
		return result;
    }

    /**
     * @brief Is in directory
     *
     * @param dirPath    Path to directory
	 * @return Return TRUE if this filename containing in dirPath, otherwise returns FALSE
	 */
    FORCEINLINE bool IsInDirectory( const std::string& dirPath ) const
    {
		std::string		absoluteDirPath;
		std::string		absolutePath;
		L_MakeAbsolutePath( dirPath, absoluteDirPath );
		L_MakeAbsolutePath( CFilename::path, absolutePath );

		// Compare two string
		return !L_Strnicmp( absolutePath.c_str(), absoluteDirPath.c_str(), ( uint32 )absoluteDirPath.size() );
    }

private:
    std::string     path;       /**< Path to file */
};

// Include implementation of platform specific inline functions
#if PLATFORM_WINDOWS
    #include "stdlib/platforms/windows/win_filetools.inl"
#else
    #error Unknown platform
#endif // PLATFORM_WINDOWS

#endif // !FILETOOLS_H