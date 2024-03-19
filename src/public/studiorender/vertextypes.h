/**
 * @file
 * @addtogroup studiorender studiorender
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

#ifndef VERTEXTYPES_H
#define VERTEXTYPES_H

#include "core/types.h"
#include "core/platform.h"
#include "stdlib/math/math.h"
#include "stdlib/math/color.h"

/**
 * @ingroup studiorender
 * @brief Simple element vertex
 */
struct SimpleElementVertex
{
	Vector4D	position;		/**< Position vertex */
	Vector2D	texCoord;		/**< Texture coords */
	CColor		color;			/**< Color */

	/**
	 * @brief Overload operator ==
	 * 
	 * @param other		Other simple element to compare
	 * @return Return TRUE if both elements is equal, otherwise returns FALSE
	 */
	bool FORCEINLINE operator==( const SimpleElementVertex& other ) const
	{
		return position == other.position && texCoord == other.texCoord && color == other.color;
	}
};

#endif // !VERTEXTYPES_H