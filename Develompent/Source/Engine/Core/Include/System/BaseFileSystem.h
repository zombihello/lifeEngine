/**
 * @file
 * @addtogroup Core Core
 *
 * @authors: Yehor Pohuliaka
 * @date: 4/13/2021
 */

#ifndef BASEFILESYSTEM_H
#define BASEFILESYSTEM_H

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
class BaseFileSystem
{
public:
    /**
     * @brief Destructor
     */
    virtual                           ~BaseFileSystem() {}

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
    virtual class BaseArchive*          CreateFileReader( const tchar* InFileName, uint32 InFlags = AR_None )         { return nullptr; }

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
    virtual class BaseArchive*          CreateFileWriter( const tchar* InFileName, uint32 InFlags = AW_None )         { return nullptr; }

    /**
     * @brief Set current directory
     * 
     * @param[in] InDirectory Path to directory
     */
    virtual void                        SetCurrentDirectory( const tchar* InDirectory )                     {}

    /**
     * @brief Get current directory
     * @return Return current directory
     */
    virtual const tchar*                GetCurrentDirectory() const                                         { return TEXT( "" ); }
};

#endif