/**
 * @file
 * @addtogroup WorldEd WorldEd
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

#ifndef DRAGNDROP_H
#define DRAGNDROP_H

#include <string>
#include <vector>

#include "Core/Misc/Types.h"
#include "Core/CoreDefines.h"

/**
 * @ingroup WorldEd
 * @brief Drag & drop type of asset reference
 * Format data: tchar string
 */
#define DND_ASSETREFERENCEETYPE		"DND::AssetReference"

/**
 * @ingroup WorldEd
 * @brief Make drag & drop asset reference data
 * 
 * @param InAssetReferences		Array of asset references to drag
 * @param OutResult				Result string
 */
void DND_MakeAssetReferenceData( const std::vector<std::wstring>& InAssetReferences, std::vector<tchar>& OutResult );

/**
 * @ingroup WorldEd
 * @brief Parse drag & drop asset reference data
 * 
 * @param InData				Dragging data
 * @param OutResult				Parsed result
 * @param InMaxParseElements	Max count elements to parse. If zero then will be parse all elements
 */
void DND_ParseAssetReferenceData( const tchar* InData, std::vector<std::wstring>& OutResult, uint32 InMaxParseElements = 0 );

#endif // !DRAGNDROP_H