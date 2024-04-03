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
#include "Misc/FileTools.h"

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
 * @brief The base class for work with file system
 */
class CBaseFileSystem
{
public:
    /**
     * @brief Destructor
     */
    virtual ~CBaseFileSystem() {}

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
    virtual class CArchive* CreateFileReader( const std::wstring& InFileName, uint32 InFlags = AR_None ) { return nullptr; }

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
    virtual class CArchive* CreateFileWriter( const std::wstring& InFileName, uint32 InFlags = AW_None ) { return nullptr; }

    /**
     * @brief Find files in directory
     * 
     * @param[in] InDirectory Path to directory
     * @param[in] InIsFiles Whether to search for files
     * @param[in] InIsDirectories Whether to search directories
     * @return Array of paths to files in directory
     */
    virtual std::vector< std::wstring > FindFiles( const std::wstring& InDirectory, bool InIsFiles, bool InIsDirectories ) { return std::vector< std::wstring >(); }

    /**
     * @brief Delete file
     * 
     * @param InPath Path to file
     * @param InIsEvenReadOnly Is even read only
     * @return Return true if file is seccussed deleted, else returning false
     */
    virtual bool Delete( const std::wstring& InPath, bool InIsEvenReadOnly = false ) { return false; }

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
    virtual bool IsExistFile( const std::wstring& InPath, bool InIsDirectory = false ) { return false; }

    /**
     * @brief Is file is directory
     * 
     * @param InPath    Path to file
     * @return Return TRUE if file is directory, otherwise will return FALSE
     */
    virtual bool IsDirectory( const std::wstring& InPath ) const { return false; }

    /**
	 * @brief Find files in the directory and any directories under it
	 *
	 * @param OutResults            The output array that is filled out with a file paths
	 * @param InRootDirectory       The root of the directory structure to recurse through
	 * @param InIsFindPackages      Should this function add package files to the OutResults
	 * @param InIsFindNonPackages   Should this function add non-package files to the OutResults
	 */
    void FindFilesInDirectory( std::vector<std::wstring>& OutResults, const tchar* InRootDirectory, bool InIsFindPackages, bool InIsFindNonPackages );

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