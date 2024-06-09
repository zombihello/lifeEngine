#include "System/Memory.h"
#include "System/DragNDrop.h"

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