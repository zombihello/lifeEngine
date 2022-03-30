/**
 * @file
 * @addtogroup WindowsPlatform Windows platform
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef WINDOWSFILESYSTEM_H
#define WINDOWSFILESYSTEM_H

#include "System/BaseFileSystem.h"

/**
 * @ingroup WindowsPlatform
 * @brief Class for work with file system in Windows
 */
class FWindowsFileSystem : public FBaseFileSystem
{
public:
    /**
     * @brief Constructor
     */
                                                    FWindowsFileSystem();

    /**
     * @brief Destructor
     */
                                                    ~FWindowsFileSystem();

    /**
     * @brief Create file reader
     *
     * @param[in] InFileName Path to file
     * @param[in] InFlags Flags of open file
     * @return Pointer on file reader, if file not opened return null
     *
     * @warning After use need delete file reader
     */
    virtual class FArchive*                     CreateFileReader( const std::wstring& InFileName, uint32 InFlags = AR_None ) override;

    /**
     * @brief Create file writer
     *
     * @param[in] InFileName Path to file
     * @param[in] InFlags Flags of write file
     * @return Pointer on file writer, if file not opened return null
     *
     * @warning After use need delete file writer
     */
    virtual class FArchive*                     CreateFileWriter( const std::wstring& InFileName, uint32 InFlags = AW_None ) override;

    /**
     * @brief Find files in directory
     *
     * @param[in] InDirectory Path to directory
     * @param[in] InIsFiles Whether to search for files
     * @param[in] InIsDirectories Whether to search directories
     * @return Array of paths to files in directory
     */
    virtual std::vector< std::wstring >             FindFiles( const std::wstring& InDirectory, bool InIsFiles, bool InIsDirectories ) override;

    /**
     * @brief Delete file
     *
     * @param InPath Path to file
     * @param InIsEvenReadOnly Is even read only
     * @return Return true if file is seccussed deleted, else returning false
     */
    virtual bool                                    Delete( const std::wstring& InPath, bool InIsEvenReadOnly = false ) override;

    /**
     * @brief Delete directory
     *
     * @param InPath Path to directory
     * @param InIsTree Is need delete all tree
     * @return Return true if directory is seccussed deleted, else returning false
     */
    virtual bool                                    DeleteDirectory( const std::wstring& InPath, bool InIsTree ) override;

    /**
     * @brief Convert to absolute path
     *
     * @param[in] InPath Path
     * @return Absolute path
     */
    virtual std::wstring                           ConvertToAbsolutePath( const std::wstring& InPath ) const override;

    /**
     * @brief Set current directory
     *
     * @param[in] InDirectory Path to directory
     */
    virtual void                                    SetCurrentDirectory( const std::wstring& InDirectory ) override;

    /**
     * @brief Get current directory
     * @return Return current directory
     */
    virtual std::wstring                            GetCurrentDirectory() const override;
};

#endif