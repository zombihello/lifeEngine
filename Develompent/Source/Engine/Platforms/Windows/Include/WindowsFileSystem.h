/**
 * @file
 * @addtogroup WindowsPlatform Windows platform
 *
 * @authors: Yehor Pohuliaka
 * @date: 4/13/2021
 */

#ifndef WINDOWSFILESYSTEM_H
#define WINDOWSFILESYSTEM_H

#include "BaseFileSystem.h"

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
    virtual class BaseArchive*          CreateFileReader( const tchar* InFileName, uint32 InFlags ) override;

    /**
     * @brief Create file writer
     *
     * @param[in] InFileName Path to file
     * @param[in] InFlags Flags of write file
     * @return Pointer on file writer, if file not opened return null
     *
     * @warning After use need delete file writer
     */
    virtual class BaseArchive*          CreateFileWriter( const tchar* InFileName, uint32 InFlags ) override;
};

#endif