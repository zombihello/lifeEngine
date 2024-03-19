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

#ifndef SEARCHPATH_H
#define SEARCHPATH_H

#include <string>
#include <vector>
#include <algorithm>

#include "core/types.h"
#include "core/platform.h"
#include "stdlib/filetools.h"

/**
 * @ingroup filesystem
 * @brief Search path
 */
class CSearchPath
{
public:
    /**
     * @brief Constructor
     */
    CSearchPath() {}

    /**
     * @brief Constructor
     * 
     * @param pNewPathID   New path ID
     * @param pNewPath     New path
     */
    CSearchPath( const achar* pNewPathID, const achar* pNewPath )
        : pathID( pNewPathID )
        , path( pNewPath )
    {
        if ( !L_IsAbsolutePath( pNewPath ) )
        {
            L_MakeAbsolutePath( pNewPath, path );
        }
        L_RemoveDotPathSeparators( path );
    }

    /**
     * @brief Set path ID
     * @param pNewPathID     New path ID
     */
    FORCEINLINE void SetPathID( const achar* pNewPathID )
    {
        pathID = pNewPathID;
    }

    /**
     * @brief Set path
     * @param pNewPath     New path 
     */
    FORCEINLINE void SetPath( const achar* pNewPath )
    {
        path = pNewPath;
    }

    /**
     * @brief Get path ID
     * @return Return path ID
     */
    FORCEINLINE const std::string& GetPathID() const
    {
        return pathID;
    }

    /**
     * @brief Get path
     * @return Return path
     */
    FORCEINLINE const std::string& GetPath() const
    {
        return path;
    }

private:
    std::string     pathID;     /**< Path ID */
    std::string     path;       /**< Path */
};

/**
 * @ingroup filesystem
 * @brief Search path iterator
 * 
 * Class for iterate over all search paths by path ID
 */
class CSearchPathIterator
{
public:
    /**
     * @brief Constructor
     * 
     * Class for iterate over all search path by path ID.
     * If pPathID isn't valid (pPathID is NULL or pPathID is empty, or lengthPathID is zero)
     * iterate over all search paths in the file system
     * 
     * @param pFilePath     Path to file (without path ID)
     * @param bForWrite     Is should be this path used for writing
	 * @param pPathID       Path ID
	 * @param lengthPathID  Length of pPathID
     */
    CSearchPathIterator( const achar* pFilePath, bool bForWrite, const achar* pPathID = nullptr, uint32 lengthPathID = 0 );

    /**
     * @brief Override operator ++
     */
    FORCEINLINE void operator++()
    {
        if ( currentIndex + 1 <= searchPaths.size() )
        {
            ++currentIndex;
        }
    }

    /**
     * @brief Override operator +=
     * @param offset	Offset
     */
    FORCEINLINE void operator+=( uint32 offset )
    {
        while ( offset-- )
        {
            operator++();
        }
    }

    /**
     * @brief Override operator *
     * @return Return current a search path
     */
    FORCEINLINE CSearchPath* operator*() const
    {
        return GetSearchPath();
    }

    /**
     * @brief Override operator ->
     * @return Return current a search path
     */
    FORCEINLINE CSearchPath* operator->() const
    {
        return GetSearchPath();
    }

    /**
     * @brief Override operator cast to bool
     * @return Return TRUE if iterator is valid, otherwise returns FALSE
     */
    FORCEINLINE operator bool() const
    {
        return currentIndex < searchPaths.size();
    }

    /**
     * @brief Override operator !
     * @return Return TRUE if iterator isn't valid, otherwise returns FALSE
     */
    FORCEINLINE bool operator !() const
    {
        return !operator bool();
    }

protected:
    /**
     * @brief Get current a search path
     * @return Return current a search path
     */
    FORCEINLINE CSearchPath* GetSearchPath() const
    {
        return !searchPaths.empty() ? searchPaths[currentIndex] : nullptr;
    }

    uint32                          currentIndex;   /**< Current a search path index */
    std::vector<CSearchPath*>       searchPaths;    /**< Search paths for this iterator */
};

/**
 * @ingroup filesystem
 * @brief Search path reverse iterator
 * 
 * Class for reverse iterate over all search paths by path ID
 */
class CSearchPathReverseIterator : public CSearchPathIterator
{
public:
    /**
     * @brief Constructor
     *
     * Class for reverse iterate over all search path by path ID.
     * If pPathID isn't valid (pPathID is NULL or pPathID is empty, or lengthPathID is zero)
     * iterate over all search paths in the file system
     *
     * @param pFilePath     Path to file (without path ID)
     * @param bForWrite     Is should be this path used for writing
     * @param pPathID       Path ID
     * @param lengthPathID  Length of pPathID
     */
    CSearchPathReverseIterator( const achar* pFilePath, bool bForWrite, const achar* pPathID = nullptr, uint32 lengthPathID = 0 )
        : CSearchPathIterator( pFilePath, bForWrite, pPathID, lengthPathID )
    {
        std::reverse( std::begin( searchPaths ), std::end( searchPaths ) );
    }
};

#endif // !SEARCHPATH_H