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
 * @brief The base class for work with file system
 */
class FBaseFileSystem
{
public:
    /**
     * @brief Destructor
     */
    virtual                                         ~FBaseFileSystem() {}

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
    virtual class FArchive*                     CreateFileReader( const std::wstring& InFileName, uint32 InFlags = AR_None )            { return nullptr; }

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
    virtual class FArchive*                     CreateFileWriter( const std::wstring& InFileName, uint32 InFlags = AW_None )            { return nullptr; }

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
     * @brief Delete directory
     * 
     * @param InPath Path to directory
     * @param InIsTree Is need delete all tree
     * @return Return true if directory is seccussed deleted, else returning false
     */
    virtual bool                                    DeleteDirectory( const std::wstring& InPath, bool InIsTree );

    /**
     * @brief Is exist file or directory
     * 
     * @param InPath Path to directory or file
     * @param InIsDirectory Checlable file is directory?
     * @return Return true if file or directory exist, false is not
     */
    virtual bool                                   IsExistFile( const std::wstring& InPath, bool InIsDirectory = false )                    { return false; }

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
};

#endif