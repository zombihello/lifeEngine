/**
 * @file
 * @addtogroup WorldEd WorldEd
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef DRAGNDROP_H
#define DRAGNDROP_H

#include <string>
#include <vector>

#include "Misc/Types.h"
#include "CoreDefines.h"

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