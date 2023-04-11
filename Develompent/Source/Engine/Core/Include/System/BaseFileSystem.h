/**
 * @file
 * @addtogroup Core Core
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef BASEFILESYSTEM_H
#define BASEFILESYSTEM_H

#include <string>
#include <vector>

#include "Core.h"
#include "Misc/Types.h"

/**
 * @ingroup Core
 * @brief Enumeration of types opening mode archive for reading
 */
enum EArchiveRead
{
    AR_None                 = 0,            /**< None */
    AR_NoFail               = 1 << 1        /**< The archive must open, otherwise there will be a fatal error */
};

/**
 * @ingroup Core
 * @brief Enumeration of types opening mode archive for writing
 */
enum EArchiveWrite
{
    AW_None                     = 0,            /**< None */
    AW_NoFail                   = 1 << 1,       /**< The archive must open, otherwise there will be a fatal error */
	AW_Append                   = 1 << 2        /**< Clear archive before operations */
};

/**
 * @ingroup Core
 * @brief Enumeration copy/move result
 */
enum ECopyMoveResult
{
	CMR_OK,          /**< Ok */
	CMR_MiscFail,    /**< Misc fail */
	CMR_ReadFail,    /**< Read fail */
	CMR_WriteFail,   /**< Write fail */
	CMR_Canceled,    /**< Canceled */
};

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
     * 
     * @param InPath    Path to file
     */
    CFilename( const std::wstring& InPath );

    /**
     * @brief Get file's extension
     * 
     * @param InIsIncludeDot        If TRUE, includes the leading dot in the result
     * @return Return the extension of this filename, or an empty string if the filename doesn't have an extension
     */
    std::wstring GetExtension( bool InIsIncludeDot = false ) const;

    /**
     * @brief Get full path
     * @return Return full path with extension
     */
    FORCEINLINE const std::wstring& GetFullPath() const
    {
        return path;
    }

    /**
     * @brief Get base filename
     * @return Return filename (without extension and any path information)
     */
    std::wstring GetBaseFilename() const;

    /**
     * @brief Get filename with extension
     * @return Return filename with extension
     */
    FORCEINLINE std::wstring GetFilename() const
    {
        return GetBaseFilename() + GetExtension( true );
    }

    /**
     * @brief Get path to the file
     * @return Return path to the file
     */
    std::wstring GetPath() const;

    /**
     * @brief Get localized filename by appending the language suffix before the extension
     * 
     * @param InLanguage    Language to use
     * @return Return localized filename
     */
    std::wstring GetLocalizedFilename( const std::wstring& InLanguage = TEXT( "" ) ) const;

    /**
     * @brief Is in directory
     * 
     * @param InPath    Path to directory
     * @return Return TRUE if that filename containing in InPath, otherwise will return FALSE
     */
    bool IsInDirectory( const std::wstring& InPath ) const;

private:
    std::wstring        path;       /**< Path to file */
};

/**
 * @ingroup Core
 * @brief The base class for work with file system
 */
class CBaseFileSystem
{
public:
    /**
     * @brief Destructor
     */
    virtual                                         ~CBaseFileSystem() {}

    /**
     * @brief Create file reader
     * @warning You must manually delete the selected object 
     * 
     * @param[in] InFileName Path to file
     * @param[in] InFlags Combinations flags of EArchiveRead for open mode
     * @return Pointer on file reader, if file not opened return null
     * 
     * @warning After use need delete file reader
     */
    virtual class CArchive*                     CreateFileReader( const std::wstring& InFileName, uint32 InFlags = AR_None )            { return nullptr; }

    /**
	 * @brief Create file writer
     * @warning You must manually delete the selected object
	 *
	 * @param[in] InFileName Path to file
	 * @param[in] InFlags Combinations flags of EArchiveWrite for opening mode
	 * @return Pointer on file writer, if file not opened return null
	 *
	 * @warning After use need delete file writer
	 */
    virtual class CArchive*                     CreateFileWriter( const std::wstring& InFileName, uint32 InFlags = AW_None )            { return nullptr; }

    /**
     * @brief Find files in directory
     * 
     * @param[in] InDirectory Path to directory
     * @param[in] InIsFiles Whether to search for files
     * @param[in] InIsDirectories Whether to search directories
     * @return Array of paths to files in directory
     */
    virtual std::vector< std::wstring >             FindFiles( const std::wstring& InDirectory, bool InIsFiles, bool InIsDirectories )     { return std::vector< std::wstring >(); }

    /**
     * @brief Delete file
     * 
     * @param InPath Path to file
     * @param InIsEvenReadOnly Is even read only
     * @return Return true if file is seccussed deleted, else returning false
     */
    virtual bool                                    Delete( const std::wstring& InPath, bool InIsEvenReadOnly = false )                    { return false; }

    /**
	 * @brief Make directory
	 *
	 * @param InPath    Path to directory
	 * @param InIsTree  Is need make all tree
	 * @return Return TRUE if directory is seccussed maked, else returning FALSE
	 */
    virtual bool MakeDirectory( const std::wstring& InPath, bool InIsTree = false );

    /**
     * @brief Delete directory
     * 
     * @param InPath Path to directory
     * @param InIsTree Is need delete all tree
     * @return Return true if directory is seccussed deleted, else returning false
     */
    virtual bool DeleteDirectory( const std::wstring& InPath, bool InIsTree = false );

    /**
     * @brief Copy file
     * 
     * @param InDstFile                 Destination file
     * @param InSrcFile                 Source file
     * @param InIsReplaceExisting       Is need replace existing files
     * @param InIsEvenReadOnly          Is even read only
     * @return Return copy result (see ECopyMoveResult)
     */
    virtual ECopyMoveResult Copy( const std::wstring& InDstFile, const std::wstring& InSrcFile, bool InIsReplaceExisting = false, bool InIsEvenReadOnly = false ) { return CMR_Canceled; }

    /**
     * @brief Move file
     * 
	 * @param InDstFile                 Destination file
	 * @param InSrcFile                 Source file
	 * @param InIsReplaceExisting       Is need replace existing files
	 * @param InIsEvenReadOnly          Is even read only
	 * @return Return move result (see ECopyMoveResult)
     */
    virtual ECopyMoveResult Move( const std::wstring& InDstFile, const std::wstring& InSrcFile, bool InIsReplaceExisting = false, bool InIsEvenReadOnly = false ) { return CMR_Canceled; }

    /**
     * @brief Is exist file or directory
     * 
     * @param InPath Path to directory or file
     * @param InIsDirectory Checlable file is directory?
     * @return Return true if file or directory exist, false is not
     */
    virtual bool                                   IsExistFile( const std::wstring& InPath, bool InIsDirectory = false )                    { return false; }

    /**
     * @brief Is file is directory
     * 
     * @param InPath    Path to file
     * @return Return TRUE if file is directory, otherwise will return FALSE
     */
    virtual bool IsDirectory( const std::wstring& InPath ) const { return false; }

    /**
     * @brief Convert to absolute path
     * 
     * @param[in] InPath Path
     * @return Absolute path
     */
    virtual std::wstring                          ConvertToAbsolutePath( const std::wstring& InPath ) const                                 { return TEXT( "" ); }

    /**
     * @brief Set current directory
     * 
     * @param[in] InDirectory Path to directory
     */
    virtual void                                    SetCurrentDirectory( const std::wstring& InDirectory )                                  {}

    /**
     * @brief Get current directory
     * @return Return current directory
     */
    virtual std::wstring                            GetCurrentDirectory() const                                                             { return TEXT( "" ); }

    /**
     * @brief Get path to current exe
     * @return Return path to current exe
     */
    virtual std::wstring GetExePath() const
    {
        return TEXT( "" );
    }

protected:
    /**
     * @brief Does Path refer to a drive letter or BNC path
     * 
     * @param InPath    Path
     * @return Return TRUE in case 'yes'
     */
    virtual bool IsDrive( const std::wstring& InPath ) const;
};

#endif