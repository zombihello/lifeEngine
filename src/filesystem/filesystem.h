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

#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <string>
#include <vector>

#include "filesystem/ifilesystem.h"
#include "filesystem/searchpath.h"

/**
 * @ingroup filesystem
 * @brief File system
 */
class CFileSystem : public CBaseAppSystem<IFileSystem>
{
public:
    friend CSearchPathIterator;

    /**
     * @brief Constructor
     */
    CFileSystem();

    /**
     * @brief Destructor
     */
    virtual ~CFileSystem();

    /**
     * @brief Connect application system
     *
     * @param pFactory		Pointer to interface factory
     * @return Return TRUE if successes application system is connected, otherwise return FALSE
     */
    virtual bool Connect( CreateInterfaceFn_t pFactory ) override;

    /**
     * @brief Disconnect application system
     */
    virtual void Disconnect() override;

    /**
     * @brief Init application system
     * @return Return TRUE if application system is inited
     */
    virtual bool Init() override;

    /**
     * @brief Shutdown application system
     */
    virtual void Shutdown() override;

    /**
     * @brief Create file reader
     *
     * This is method looks file in the search paths and first opened returns.
     * You can set path ID use at the begin '//', for example: //GAME/cfg/config.cfg
     *
     * @param pPath         Path to file
     * @param flags         Combinations flags of EFileRead for open mode
     * @return Return pointer to file reader, if a file isn't open returns NULL
     */
    virtual TRefPtr<IFileReader> CreateFileReader( const achar* pPath, uint32 flags = FILE_READ_NONE ) override;

    /**
     * @brief Create file writer
     *
     * This is method looks file in the search paths and at the first one create a new file.
     * You can set path ID use at the begin '//', for example: //GAME/cfg/config.cfg
     *
     * @param pPath         Path to file
     * @param flags         Combinations flags of EFileWrite for opening mode
     * @return Return pointer to file writer, if a file isn't open returns NULL
     */
    virtual TRefPtr<IFileWriter> CreateFileWriter( const achar* pPath, uint32 flags = FILE_WRITE_NONE ) override;

    /**
     * @brief Find files and directories by the path
     *
     * This is method looks files in the search paths or in the first if it set.
     * You can set path ID use at the begin '//', for example: //GAME/cfg/
     *
     * @param pPath             Path to directory
     * @param bFiles            Whether to search files
     * @param bDirectories      Whether to search directories
     * @param bLookAllPathIDs   Is need look for all the search paths. If FALSE looks for only in the first search path where found any files
     * @return Return path array with all found files and directories
     */
    virtual TRefPtr<IPathArrayResult> FindFiles( const achar* pPath, bool bFiles, bool bDirectories, bool bLookAllPathIDs = true ) override;

    /**
     * @brief Load DLL module
     *
     * This is method looks a DLL module in the search paths and first opened returns.
     * You can set path ID use at the begin '//', for example: //ENGINEBIN/engine.dll, //GAMEBIN/eleot.dll
     *
     * @param pDLLName     Path to DLL
     * @return Return DLL handle, if it isn't loaded returns NULL
     */
    virtual dllHandle_t LoadModule( const achar* pDLLName ) override;

    /**
     * @brief Unload DLL module
     * @param dllHandle   DLL handle
     */
    virtual void UnloadModule( dllHandle_t dllHandle ) override;

    /**
     * @brief Delete file
     *
     * This method looks for a file in the search paths and the first one deletes or all if this flag is set.
     * You can set path ID use at the begin '//', for example: //GAME/cfg/config.cfg
     *
     * @param pPath                 Path to file
     * @param bDeleteAllPathIDs     Is need delete all files in the search paths. If FALSE the first one deletes
     * @param bEvenReadOnly         Delete file even it's read only
     * @return Return TRUE if file is successfully deleted, otherwise returns FALSE
     */
    virtual bool DeleteFile( const achar* pPath, bool bDeleteAllPathIDs = false, bool bEvenReadOnly = false ) override;

    /**
     * @brief Make directory
     *
     * This is method looks a path in the search paths and at the first one make a new directory.
     * You can set path ID use at the begin '//', for example: //GAME/cfg/
     *
     * @param pPath   Path to directory
     * @return Return TRUE if directory is successfully made, otherwise returns FALSE
     */
    virtual bool MakeDirectory( const achar* pPath ) override;

    /**
     * @brief Delete directory
     *
     * This method looks for a directory in the search paths and the first one deletes or all if this flag is set.
     * You can set path ID use at the begin '//', for example: //GAME/cfg/
     *
     * @param pPath                 Path to directory
     * @param bTree                 Is need delete all tree
     * @param bDeleteAllPathIDs     Is need delete all directories in the search paths. If FALSE the first one deletes
     * @param bEvenReadOnly         Delete a directory even if it is read-only or a file in it
     * @return Return TRUE if directory is successfully deleted, otherwise returns FALSE
     */
    virtual bool DeleteDirectory( const achar* pPath, bool bDeleteAllPathIDs = false, bool bEvenReadOnly = false ) override;

    /**
     * @brief Copy file
     *
     * This method looks for a file in the search paths and the first one copy into
     * the first found search path in pDestPath.
     * You can set path ID use at the begin '//', for example: //GAME/cfg/, //ENGINE/materials/default.lmf
     *
     * @param pSrcPath          Source path
     * @param pDestPath         Destination path
     * @param bReplaceExisting  Is need replace existing file
     * @param bEvenReadOnly     Is even read only
     * @return Return copy result (see ECopyMoveResult)
     */
    virtual ECopyMoveResult CopyFile( const achar* pSrcPath, const achar* pDestPath, bool bReplaceExisting = false, bool bEvenReadOnly = false ) override;

    /**
     * @brief Copy directory
     *
     * This method looks for directories in the search paths and copy they into
     * the first found search path in pDestPath.
     * You can set path ID use at the begin '//', for example: //GAME/cfg/, //ENGINE/materials/default.lmf
     *
     * @param pSrcPath          Source path
     * @param pDestPath         Destination path
     * @param bCopyAllPathIDs   Is need copy all directories in the search paths. If FALSE copy only the first one
     * @param bReplaceExisting  Is need replace existing files/directories
     * @param bEvenReadOnly     Is even read only
     * @return Return copy result (see ECopyMoveResult)
     */
    virtual ECopyMoveResult CopyDirectory( const achar* pSrcPath, const achar* pDestPath, bool bCopyAllPathIDs = false, bool bReplaceExisting = false, bool bEvenReadOnly = false ) override;

    /**
     * @brief Move file
     *
     * This method looks for a file in the search paths and the first one move into
     * the first found search path in pDestPath.
     * You can set path ID use at the begin '//', for example: //GAME/cfg/, //ENGINE/materials/default.lmf
     *
     * @param pSrcPath               Source path
     * @param pDestPath              Destination path
     * @param bReplaceExisting       Is need replace existing file
     * @param bEvenReadOnly          Is even read only
     * @return Return move result (see ECopyMoveResult)
     */
    virtual ECopyMoveResult MoveFile( const achar* pSrcPath, const achar* pDestPath, bool bReplaceExisting = false, bool bEvenReadOnly = false ) override;

    /**
     * @brief Move directory
     *
     * This method looks for directories in the search paths and move they into
     * the first found search path in pDestPath.
     * You can set path ID use at the begin '//', for example: //GAME/cfg/, //ENGINE/materials/default.lmf
     *
     * @param pSrcPath               Source path
     * @param pDestPath              Destination path
     * @param bMoveAllPathIDs        Is need move all directories in the search paths. If FALSE move only the first one
     * @param bReplaceExisting       Is need replace existing files/directories
     * @param bEvenReadOnly          Is even read only
     * @return Return move result (see ECopyMoveResult)
     */
    virtual ECopyMoveResult MoveDirectory( const achar* pSrcPath, const achar* pDestPath, bool bMoveAllPathIDs = false, bool bReplaceExisting = false, bool bEvenReadOnly = false ) override;

    /**
     * @brief Is file or directory exists
     *
     * This is method looks for a file or a directory in the search paths and when first was found returns TRUE.
     * You can set path ID use at the begin '//', for example: //GAME/cfg/config.cfg, //ENGINE/materials/
     *
     * @param pPath   Path to directory or file
     * @return Return TRUE if file or directory is exist, otherwise FALSE
     */
    virtual bool IsFileExists( const achar* pPath ) const override;

    /**
     * @brief Is file a directory
     *
     * This is method looks for a file or a directory in the search paths and when first was found returns TRUE if it is directory.
     * You can set path ID use at the begin '//', for example: //GAME/cfg/config.cfg, //GAME/cfg/
     *
     * @param pPath   Path to file
     * @return Return TRUE if a file is directory, otherwise returns FALSE
     */
    virtual bool IsFileDirectory( const achar* pPath ) const override;

    /**
     * @brief Add search path
     *
     * @param pSearchPath   New search path to add
     * @param pPathID       Path ID
     */
    virtual void AddSearchPath( const achar* pSearchPath, const achar* pPathID ) override;

    /**
     * @brief Remove all search paths
     */
    virtual void RemoveAllSearchPaths() override;

    /**
     * @brief Remove search path by ID
     * This method removes all search paths with same ID
     *
     * @param pPathID   Path ID
     */
    virtual void RemoveSearchPath( const achar* pPathID ) override;

    /**
     * @brief Get search path by ID
     *
     * @param pPathID      Path ID
     * @return Return path array with all search paths of pPathID
     */
    virtual TRefPtr<IPathArrayResult> GetSearchPath( const achar* pPathID ) const override;

    /**
     * @brief Get number of search paths in the file system
     * @return Return number of search paths
     */
    virtual uint32 GetNumSearchPaths() const override;

private:
    /**
     * @brief Internal implementation of make directory
     * @warning Not support path IDs
     * 
     * @param pPath     Path to directory
     * @return Return TRUE if directory tree is successfully made, otherwise returns FALSE
     */
    bool MakeDirectoryInternal( const achar* pPath );

	/**
	* @brief Internal implementation of delete directory
	* @warning Not support path IDs
	*
	* @param pPath          Path to directory
    * @param bEvenReadOnly  Delete a directory even if it is read-only or a file in it
	* @return Return TRUE if directory is successfully deleted, otherwise will return FALSE
	*/
	bool DeleteDirectoryInternal( const achar* pPath, bool bEvenReadOnly = false );

    /**
     * @brief Internal implementation of copy directory
     * @warning Not support path IDs
     *
     * @param pSrcPath          Source path
     * @param pDestPath         Destination path
     * @param bReplaceExisting  Is need replace existing files/directories
     * @param bEvenReadOnly     Is even read only
     * @return Return copy result (see ECopyMoveResult)
     */
    ECopyMoveResult CopyDirectoryInternal( const achar* pSrcPath, const achar* pDestPath, bool bReplaceExisting = false, bool bEvenReadOnly = false );

    /**
     * @brief Internal implementation of move directory
     * @warning Not support path IDs
     *
     * @param pSrcPath               Source path
     * @param pDestPath              Destination path
     * @param bReplaceExisting       Is need replace existing files/directories
     * @param bEvenReadOnly          Is even read only
     * @return Return move result (see ECopyMoveResult)
     */
    ECopyMoveResult MoveDirectoryInternal( const achar* pSrcPath, const achar* pDestPath, bool bReplaceExisting = false, bool bEvenReadOnly = false );

    /**
     * @brief Parse the path ID in a file path
     * 
     * This method looks for UNC-type filename specifier, which should be used instead of passing in path ID. 
     * So if it finds //GAME/cfg/config.cfg, set pointer pFilePath to the begin of file path, set pointer pPathID 
     * to the begin of path ID and set length of pPathID into lengthPathID
     * 
     * @param pPath         Path to file
     * @param pFilePath     Output, pointer to the begin of file path in path
     * @param pPathID       Output, pointer to the begin of path ID in path
     * @param lengthPathID  Output, length of pPathID
     */
    void ParsePathID( const achar* pPath, const achar*& pFilePath, const achar*& pPathID, uint32& lengthPathID ) const;

    /**
     * @brief Computes a full path from a search path
     * 
     * @param pFilePath     Relative path to file
     * @param pSearchPath   Search path. Must be valid!
     * @param destPath      Output a full write path
     */
    void ComputeFullPath( const achar* pFilePath, const CSearchPath* pSearchPath, std::string& destPath ) const;

    std::vector<CSearchPath>        searchPaths;    /**< Array of search paths */
};

#endif // !FILESYSTEM_H