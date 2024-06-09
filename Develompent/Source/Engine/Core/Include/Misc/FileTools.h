/**
 * @file
 * @addtogroup Core Core
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef FILETOOLS_H
#define FILETOOLS_H

#include <string>

#include "Misc/Platform.h"
#include "Misc/Types.h"
#include "Misc/StringTools.h"
#include "Core.h"

/**
 * @ingroup Core
 * @brief Enumeration of result code for L_MakeRelativePath
 */
enum EMakeRelativePathResult
{
    MRPR_Ok                     = 0,    /**< The relative path was successfully created */
    MRPR_ErrorDestTooSmall      = 1,    /**< Can't make relative because InDirPath too small */
    MRPR_ErrorOnSeparateDrives  = 2     /**< Can't make relative because on separate drives or something else */
};

/**
 * @ingroup Core
 * @brief Is char a path separator
 *
 * @param InC   Char to check
 * @return Return TRUE if InC is a path separator, otherwise FALSE
 */
FORCEINLINE bool L_IsPathSeparator( tchar InC )
{
    return InC == TEXT( '\\' ) || InC == TEXT( '/' );
}

/**
 * @ingroup Core
 * @brief Set current directory
 * @note Need implement on each platform
 *
 * @param InDirName     Absolute path to a directory
 * @return Return TRUE if current directory was successful changed, otherwise returns FALSE
 */
FORCEINLINE bool L_SetCurrentDirectory( const tchar* InDirName );

/**
 * @ingroup Core
 * @brief Set current directory
 *
 * @param InDirName     Absolute path to a directory
 * @return Return TRUE if current directory was successful changed, otherwise returns FALSE
 */
FORCEINLINE bool L_SetCurrentDirectory( const std::wstring& InDirName )
{
    return L_SetCurrentDirectory( InDirName.c_str() );
}

/**
 * @ingroup Core
 * @brief Get absolute path to the current directory
 * @note Need implement on each platform
 *
 * @param OutDestStr		Destination string. Must be not-NULL
 * @param InMaxLen		    Max size of OutDestStr. Must be greater than zero 
 * @return Return TRUE upon success, FALSE if fails because OutDestStr was too small
 */
FORCEINLINE bool L_GetCurrentDirectory( tchar* OutDestStr, uint32 InMaxLen );

/**
 * @ingroup Core
 * @brief Get absolute path to the current executable module
 * @note Need implement on each platform
 *
 * This function returns absolute path to the current executable module.
 * For example: C:/Game/Binaries/Win64/EleotGame-Win64-DebugWithEditor.exe
 *
 * @return Return absolute path to the current executable module
 */
const tchar* L_GetExecutablePath();

/**
 * @ingroup Core
 * @brief Get absolute path to the current directory
 * 
 * @param OutDestStr	    Destination string
 * @param InIsShrinkToFit   If TRUE, remove all extra null terminator characters in OutDestStr
 */
void L_GetCurrentDirectory( std::wstring& OutDestStr, bool InIsShrinkToFit = true );

/**
 * @ingroup Core
 * @brief Make absolute path from relative
 *
 * @param InSrcPath		Relative path
 * @param OutDestPath	Output absolute path
 * @param InMaxLen		Max size of OutDestPath
 * @param InStartingDir	Using the current working directory as the base, or InStartingDir if it's non-NULL
 * @return Return TRUE if the absolute path was successfully created. Returns FALSE if it runs out of room in the string
 */
bool L_MakeAbsolutePath( const tchar* InSrcPath, tchar* OutDestPath, uint32 InMaxLen, const tchar* InStartingDir = nullptr );

/**
 * @ingroup Core
 * @brief Make absolute path from relative
 *
 * @param InSrcPath		    Relative path
 * @param OutDestPath		Output absolute path
 * @param InStartingDir	    Using the current working directory as the base, or InStartingDir if it isn't empty
 * @param InIsShrinkToFit  If TRUE, remove all extra null terminator characters in OutDestPath
 */
void L_MakeAbsolutePath( const std::wstring& InSrcPath, std::wstring& OutDestPath, const std::wstring& InStartingDir = TEXT( "" ), bool InIsShrinkToFit = true );

/**
 * @ingroup Core
 * @brief Make relative path from two full paths
 *
 * @param InFullPath		The full path of the file to make a relative path for (Must be absolute path)
 * @param InDirPath			The full path of the directory to make InFullPath relative to (Must be absolute path)
 * @param OutRelativePath	Output relative path
 * @param InMaxLen			Max size of OutRelativePath
 * @return Return result code (see EMakeRelativePathResult)
 */
EMakeRelativePathResult L_MakeRelativePath( const tchar* InFullPath, const tchar* InDirPath, tchar* OutRelativePath, uint32 InMaxLen );

/**
 * @ingroup Core
 * @brief Make relative path from two full paths
 *
 * @param InFullPath			The full path of the file to make a relative path for (Must be absolute path)
 * @param InDirPath			    The full path of the directory to make fullPath relative to (Must be absolute path)
 * @param OutRelativePath		Output relative path
 * @param InIsShrinkToFit       If TRUE, remove all extra null terminator characters in OutRelativePath
 * @return Return TRUE if the relative path was successfully created. Returns FALSE if can't make relative (on separate drives, for example)
 */
bool L_MakeRelativePath( const std::wstring& InFullPath, const std::wstring& InDirPath, std::wstring& OutRelativePath, bool InIsShrinkToFit = true );

/**
 * @ingroup Core
 * @brief Is absolute path
 * @note Need implement on each platform
 * 
 * @param InPath	Path to check
 * @return Return TRUE if the path is an absolute path, otherwise FALSE
 */
FORCEINLINE bool L_IsAbsolutePath( const tchar* InPath );

/**
 * @ingroup Core
 * @brief Is absolute path
 *
 * @param InPath	Path to check
 * @return Return TRUE if the path is an absolute path, otherwise FALSE
 */
FORCEINLINE bool L_IsAbsolutePath( const std::wstring& InPath )
{
    return L_IsAbsolutePath( InPath.c_str() );
}

/**
 * @ingroup Core
 * @brief Adds a path separator to the end of the string if there isn't one already

 * @param InOutStr    String
 * @param InStrSize   String size
 * @return Return TRUE if a path separator was added successfully to the end of the string or it's already here, FALSE if run out of space
 */
bool L_AppendPathSeparator( tchar* InOutStr, uint32 InStrSize );

/**
 * @ingroup Core
 * @brief Adds a path separator to the end of the string if there isn't one already
 * @param InStr  String
 */
void L_AppendPathSeparator( std::wstring& InStr );

/**
 * @ingroup Core
 * @brief Fix path separators in the path
 * @param InOutPath     Path to fix
 */
void L_FixPathSeparators( tchar* InOutPath );

/**
 * @ingroup Core
 * @brief Fix path separators in the path
 * @param InPath    Path to fix
 */
FORCEINLINE void L_FixPathSeparators( std::wstring& InPath )
{
    L_FixPathSeparators( InPath.data() );
}

/**
 * @ingroup Core
 * @brief Get the file extension from a path
 *
 * @param InPath	        Path
 * @param InIsIncludeDot    If TRUE, includes the leading dot in the result
 * @return Returns a pointer to beginning of extension (after the "."), or NULL if there is no extension
 */
const tchar* L_GetFileExtension( const tchar* InPath, bool InIsIncludeDot = false );

/**
 * @ingroup Core
 * @brief Get the file extension from a path
 *
 * @param InPath	     Path
 * @param OutExtension   Output a file extension (after the "."). Empty if there is no extension
 * @param InIsIncludeDot If TRUE, includes the leading dot in the result
 */
FORCEINLINE void L_GetFileExtension( const std::wstring& InPath, std::wstring& OutExtension, bool InIsIncludeDot = false )
{
    OutExtension = L_GetFileExtension( InPath.c_str(), InIsIncludeDot );
}

/**
 * @ingroup Core
 * @brief Get the base file name from a path
 *
 * @param InPath		Path
 * @param OutFileName	Output base file name
 * @param InMaxLen		Max size of OutFileName
 * @return Return TRUE if the base file name was successfully copy, or FALSE if OutFileName was too small
 */
bool L_GetFileBaseName( const tchar* InPath, tchar* OutFileName, uint32 InMaxLen );

/**
 * @ingroup Core
 * @brief Get the base file name from a path
 *
 * @param InPath		   Path
 * @param OutFileName	   Output base file name
 * @param InIsShrinkToFit  If TRUE, remove all extra null terminator characters in fileName
 */
void L_GetFileBaseName( const std::wstring& InPath, std::wstring& OutFileName, bool InIsShrinkToFit = true );

/**
 * @ingroup Core
 * @brief Get the file name from a path
 *
 * @param InPath	Path
 * @return Returns a pointer to just the filename part of the path (everything after the last path seperator)
 */
const tchar* L_GetFileName( const tchar* InPath );

/**
 * @ingroup Core
 * @brief Get the file name from a path
 *
 * @param InPath	    Path
 * @param OutFileName   Output file name (everything after the last path seperator)
 */
FORCEINLINE void L_GetFileName( const std::wstring& InPath, std::wstring& OutFileName )
{
    OutFileName = L_GetFileName( InPath.c_str() );
}

/**
 * @ingroup Core
 * @brief Get the path to file from a full path
 *
 * @param InSrcPath		Full path
 * @param OutDestPath	Output the path to file. Copy to this all the path except for the stuff after the final path separator
 * @param InMaxLen		Max size of OutDestPath
 * @return Return TRUE if the path to file was successfully copy, or FALSE if OutDestPath was too small
 */
bool L_GetFilePath( const tchar* InSrcPath, tchar* OutDestPath, uint32 InMaxLen );

/**
 * @ingroup Core
 * @brief Get the path to file from a full path
 *
 * @param InSrcPath	        Full path
 * @param OutDestPath	    Output the path to file. Copy to this all the path except for the stuff after the final path separator
 * @param InIsShrinkToFit   If TRUE, remove all extra null terminator characters in OutDestPath
 */
void L_GetFilePath( const std::wstring& InSrcPath, std::wstring& OutDestPath, bool InIsShrinkToFit = true );

/**
 * @ingroup Core
 * @brief Removes "./" and "../" from the path
 * 
 * @param InOutPath                     Path to fix, should be a full path
 * @param InIsRemoveDoubleSeparators    Is need remove double separators
 */
void L_RemoveDotPathSeparators( tchar* InOutPath, bool InIsRemoveDoubleSeparators = true );

/**
 * @ingroup Core
 * @brief Removes "./" and "../" from the path
 *
 * @param InOutPath                     Path to fix, should be a full path
 * @param InIsRemoveDoubleSeparators    Is need remove double separators
 */
FORCEINLINE void L_RemoveDotPathSeparators( std::wstring& InOutPath, bool InIsRemoveDoubleSeparators = true )
{
    L_RemoveDotPathSeparators( InOutPath.data(), InIsRemoveDoubleSeparators );
    InOutPath.resize( L_Strlen( InOutPath.c_str() ) );
}

/**
 * @ingroup Core
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
     * @param InPath    Path to the file
     */
    CFilename( const std::wstring& InPath );

    /**
     * @brief Get file's extension
     *
     * @param InIsIncludeDot     If TRUE, includes the leading dot in the result
     * @return Return the extension of this filename, or an empty string if the filename doesn't have an extension
     */
    FORCEINLINE std::wstring GetExtension( bool InIsIncludeDot = false ) const
    {
        return L_GetFileExtension( path.c_str(), InIsIncludeDot );
    }

    /**
     * @brief Get full path
     * @return Return full path with extension
     */
    FORCEINLINE const std::wstring& GetFullPath() const
    {
        return path;
    }

    /**
     * @brief Get file name without without extension and any path information
     * @return Return base file name (without extension and any path information)
     */
    FORCEINLINE std::wstring GetBaseFileName() const
    {
        std::wstring	result;
        L_GetFileBaseName( path, result );
        return result;
    }

    /**
     * @brief Get file name with extension
     * @return Return file name with extension
     */
    FORCEINLINE std::wstring GetFileName() const
    {
        std::wstring	result;
        L_GetFileName( path, result );
        return result;
    }

    /**
     * @brief Get path to the file
     * @return Return path to the file
     */
    FORCEINLINE std::wstring GetPath() const
    {
        std::wstring	result;
        L_GetFilePath( path, result );
        return result;
    }

    /**
     * @brief Get localized file name by appending the language suffix before the extension
     * 
     * @param InLanguage    Language to use
     * @return Return localized file name (with extension)
     */
    std::wstring GetLocalizedFileName( const std::wstring& InLanguage = TEXT( "" ) ) const;

    /**
     * @brief Is in directory
     *
     * @param InDirPath    Path to directory
     * @return Return TRUE if this filename containing in dirPath, otherwise returns FALSE
     */
    FORCEINLINE bool IsInDirectory( const std::wstring& InDirPath ) const
    {
		std::wstring	absoluteDirPath;
		std::wstring	absolutePath;
        L_MakeAbsolutePath( InDirPath, absoluteDirPath, TEXT( "" ), false );
        L_MakeAbsolutePath( path, absolutePath, TEXT( "" ), false );

        // Compare two string
        return !L_Strnicmp( absolutePath.c_str(), absoluteDirPath.c_str(), ( uint32 )absoluteDirPath.size() );
    }

private:
    std::wstring        path;       /**< Path to file */
};

// Include implementation of platform specific inline functions
#if PLATFORM_WINDOWS
    #include "WindowsFileTools.inl"
#else
    #error Unknown platform
#endif // PLATFORM_WINDOWS

#endif // !FILETOOLS_H