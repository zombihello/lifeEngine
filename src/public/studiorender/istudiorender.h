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

#ifndef ISTUDIORENDER_H
#define ISTUDIORENDER_H

#include "studiorender/studioapi/istudioviewport.h"
#include "appframework/iappsystem.h"

/**
 * @ingroup studiorender
 * @brief Studio render interface version
 */
#define STUDIORENDER_INTERFACE_VERSION "LStudioRender001"

/**
 * @ingroup studiorender
 * @brief Studio render interface
 */
class IStudioRender : public IAppSystem
{
public:
	/**
	 * @brief Set Studio API
	 * Sets which API we should be using. Has to be done before connect
	 * 
	 * @param pStudioAPIDLL		Studio API module name (e.g studioapi_dx11.dll)
	 * @return Return TRUE if Studio API successfully set, otherwise returns FALSE
	 */
	virtual void SetStudioAPI( const achar* pStudioAPIDLL ) = 0;

	/**
	 * @brief Begin render frame
	 * @param pStudioViewport	Studio viewport
	 */
	virtual void BeginFrame( IStudioViewport* pStudioViewport ) = 0;

	/**
	 * @brief End render frame
	 * @param pStudioViewport	Studio viewport
	 */
	virtual void EndFrame( IStudioViewport* pStudioViewport ) = 0;
};

#endif // !ISTUDIORENDER_H