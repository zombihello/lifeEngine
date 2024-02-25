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

#ifndef PATHARRAYRESULT_H
#define PATHARRAYRESULT_H

#include <vector>
#include <string>

#include "filesystem/ifilesystem.h"

/**
 * @ingroup filesystem
 * @brief Path array result
 * 
 * This is simple array who contains path elements
 */
class CPathArrayResult : public TRefCounted<IPathArrayResult>
{
public:
    /**
     * @brief Constructor
     * @param pathArray     Array with paths
     */
    CPathArrayResult( const std::vector<std::string>& pathArray );

    /**
     * @brief Get path by index
     *
     * @param index     Item index
     * @return Return path at index
     */
    virtual const achar* GetPath( uint32 index ) const override;

    /**
     * @brief Get path count in array
     * @return Return path count in array
     */
    virtual uint32 GetNum() const override;

    /**
     * @brief Is empty array
     * @return Return TRUE if array is empty, otherwise returns FALSE
     */
    virtual bool IsEmpty() const override;

private:
	std::vector<std::string>	pathArray;	/**< Array with paths */
};

#endif // !PATHARRAYRESULT_H