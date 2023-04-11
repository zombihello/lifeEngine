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
class CWindowsFileSystem : public CBaseFileSystem
{
public:
    /**
     * @brief Constructor
     */
                                                    CWindowsFileSystem();

    /**
     * @brief Destructor
     */
                                                    ~CWindowsFileSystem();

    /**
     * @brief Create file reader
     *
     * @param[in] InFileName Path to file
     * @param[in] InFlags Flags of open file
     * @return Pointer on file reader, if file not opened return null
     *
     * @warning After use need delete file reader
     */
    virtual class CArchive*                     CreateFileReader( const std::wstring& InFileName, uint32 InFlags = AR_None ) override;

    /**
     * @brief Create file writer
     *
     * @param[in] InFileName Path to file
     * @param[in] InFlags Flags of write file
     * @return Pointer on file writer, if file not opened return null
     *
     * @warning After use need delete file writer
     */
    virtual class CArchive*                     CreateFileWriter( const std::wstring& InFileName, uint32 InFlags = AW_None ) override;

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
     * @brief Make directory
     *
     * @param InPath    Path to directory
     * @param InIsTree  Is need make all tree
     * @return Return TRUE if directory is seccussed maked, else returning FALSE
     */
    virtual bool MakeDirectory( const std::wstring& InPath, bool InIsTree = false ) override;

    /**
     * @brief Delete directory
     *
     * @param InPath Path to directory
     * @param InIsTree Is need delete all tree
     * @return Return true if directory is seccussed deleted, else returning false
     */
    virtual bool DeleteDirectory( const std::wstring& InPath, bool InIsTree ) override;

    /**
     * @brief Copy file
     *
     * @param InDstFile                 Destination file
     * @param InSrcFile                 Source file
     * @param InIsReplaceExisting       Is need replace existing files
     * @param InIsEvenReadOnly          Is even read only
     * @return Return copy result (see ECopyMoveResult)
     */
    virtual ECopyMoveResult Copy( const std::wstring& InDstFile, const std::wstring& InSrcFile, bool InIsReplaceExisting = false, bool InIsEvenReadOnly = false ) override;

    /**
     * @brief Move file
     *
     * @param InDstFile                 Destination file
     * @param InSrcFile                 Source file
     * @param InIsReplaceExisting       Is need replace existing files
     * @param InIsEvenReadOnly          Is even read only
     * @return Return move result (see ECopyMoveResult)
     */
    virtual ECopyMoveResult Move( const std::wstring& InDstFile, const std::wstring& InSrcFile, bool InIsReplaceExisting = false, bool InIsEvenReadOnly = false ) override;

    /**
     * @brief Is exist file or directory
     *
     * @param InPath Path to directory or file
     * @param InIsDirectory Checlable file is directory?
     * @return Return true if file or directory exist, false is not
     */
    virtual bool                                   IsExistFile( const std::wstring& InPath, bool InIsDirectory = false ) override;

    /**
	 * @brief Is file is directory
	 *
	 * @param InPath    Path to file
	 * @return Return TRUE if file is directory, otherwise will return FALSE
	 */
    virtual bool IsDirectory( const std::wstring& InPath ) const override;

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

    /**
     * @brief Get path to current exe
     * @return Return path to current exe
     */
    virtual std::wstring GetExePath() const override;
};

#endif