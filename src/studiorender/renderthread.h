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

#ifndef RENDERTHREAD_H
#define RENDERTHREAD_H

#include "studiorender/studiocmdbuffer.h"

/**
 * @ingroup studiorender
 * @brief The render command buffer
 */
extern CStudioCmdBuffer		g_StudioCmdBuffer;

/**
 * @ingroup studiorender
 * @brief The render thread handle
 */
extern threadHandle_t		g_RenderThreadHandle;

/**
 * @ingroup studiorender
 * @brief Starts the render thread
 */
void R_StartRenderThread();

/**
 * @ingroup studiorender
 * @brief Stops the render thread
 */
void R_StopRenderThread();

#endif // !RENDERTHREAD_H