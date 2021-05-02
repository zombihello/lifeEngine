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
class WindowsFileSystem : public BaseFileSystem
{
public:
    /**
     * @brief Constructor
     */
                                                    WindowsFileSystem();

    /**
     * @brief Destructor
     */
                                                    ~WindowsFileSystem();

    /**
     * @brief Create file reader
     *
     * @param[in] InFileName Path to file
     * @param[in] InFlags Flags of open file
     * @return Pointer on file reader, if file not opened return null
     *
     * @warning After use need delete file reader
     */
    virtual class BaseArchive*                      CreateFileReader( const tchar* InFileName, uint32 InFlags = AR_None ) override;

    /**
     * @brief Create file writer
     *
     * @param[in] InFileName Path to file
     * @param[in] InFlags Flags of write file
     * @return Pointer on file writer, if file not opened return null
     *
     * @warning After use need delete file writer
     */
    virtual class BaseArchive*                      CreateFileWriter( const tchar* InFileName, uint32 InFlags = AW_None ) override;

    /**
     * @brief Find files in directory
     *
     * @param[in] InDirectory Path to directory
     * @param[in] InIsFiles Whether to search for files
     * @param[in] InIsDirectories Whether to search directories
     * @return Array of paths to files in directory
     */
    virtual std::vector< std::wstring >             FindFiles( const tchar* InDirectory, bool InIsFiles, bool InIsDirectories ) override;

    /**
     * @brief Set current directory
     *
     * @param[in] InDirectory Path to directory
     */
    virtual void                                    SetCurrentDirectory( const tchar* InDirectory ) override;

    /**
     * @brief Get current directory
     * @return Return current directory
     */
    virtual const tchar*                            GetCurrentDirectory() const override;
};

#endif