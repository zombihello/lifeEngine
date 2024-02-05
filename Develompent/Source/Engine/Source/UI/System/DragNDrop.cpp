/**
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

#include "Core/Core.h"
#include "UI/System/DragNDrop.h"

/*
==================
DND_MakeAssetReferenceData
==================
*/
void DND_MakeAssetReferenceData( const std::vector<std::wstring>& InAssetReferences, std::vector<tchar>& OutResult )
{
	for ( uint32 index = 0, count = InAssetReferences.size(); index < count; ++index )
	{
		const std::wstring& assetRef = InAssetReferences[index];
		uint32					oldSize = OutResult.size();
		uint32					newSize = oldSize + assetRef.size() + 1;				// +1 for terminate of string
		OutResult.resize( newSize, TEXT( '\0' ) );
		Memory::Memcpy( OutResult.data() + oldSize, assetRef.data(), sizeof( tchar ) * assetRef.size() );
	}
	OutResult.push_back( TEXT( '\0' ) );	// Terminate element
}

/*
==================
DND_ParseAssetReferenceData
==================
*/
void DND_ParseAssetReferenceData( const tchar* InData, std::vector<std::wstring>& OutResult, uint32 InMaxParseElements /* = 0 */ )
{
	// We do nothing if InData is not valid 
	if ( !InData )
	{
		return;
	}

	// Parse data
	uint32			indexItem = 0;
	const tchar*	pStartString = InData;
	bool			bEnd = false;
	while ( !bEnd && ( InMaxParseElements == 0 || indexItem < InMaxParseElements ) )
	{
		if ( *InData == TEXT( '\0' ) )
		{
			// Copy string to output array
			if ( pStartString != InData )
			{
				OutResult.push_back( std::wstring( pStartString, InData ) );
				pStartString = InData + 1;
				++indexItem;
			}
			// Otherwise this is empty string. It's mean what this is the end of array
			else
			{
				bEnd = true;
			}
		}

		if ( !bEnd )
		{
			++InData;
		}
	}
}