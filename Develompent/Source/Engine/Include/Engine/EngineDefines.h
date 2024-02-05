/**
 * @file
 * @addtogroup Engine Engine
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

#ifndef ENGINEDEFINES_H
#define ENGINEDEFINES_H

/**
 * @ingroup Engine
 * Maximum size of the world
 */
#define WORLD_MAX					524288.0

/**
 * @ingroup Engine
 * Half the maximum size of the world
 */
#define HALF_WORLD_MAX				( WORLD_MAX * 0.5 )

 /**
  * @ingroup Engine
  * Half the maximum size of the world - 1
  */
#define HALF_WORLD_MAX1				( HALF_WORLD_MAX - 1.0 )

/**
 * @ingroup Engine
 * Limit of 2D viewport zoom in
 */
#define MIN_ORTHOZOOM				250.0

/**
 * @ingroup Engine
 * Limit of 2D viewport zoom out
 */
#define MAX_ORTHOZOOM				16000000.0

#endif // !ENGINEDEFINES_H