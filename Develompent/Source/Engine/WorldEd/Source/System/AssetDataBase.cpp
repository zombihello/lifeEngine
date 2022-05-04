#include "System/AssetDataBase.h"

void FAssetDataBase::Init()
{}

void FAssetDataBase::RemoveAsset( FAsset* InAsset )
{
	check( InAsset && InAsset->GetPackage() );
	auto	itPackage = loadedAssetsMap.find( InAsset->GetPackage() );
	if ( itPackage == loadedAssetsMap.end() )
	{
		return;
	}

	// Remove asset from data base
	for ( auto itAsset = itPackage->second.begin(), itAssetEnd = itPackage->second.end(); itAsset != itAssetEnd; ++itAsset )
	{
		FAssetRef		asset = *itAsset;
		if ( asset == InAsset )
		{
			itAsset = itPackage->second.erase( itAsset );	
			return;
		}
	}
}

void FAssetDataBase::RemoveAssets( FPackage* InPackage )
{
	auto	itPackage = loadedAssetsMap.find( InPackage );
	if ( itPackage == loadedAssetsMap.end() )
	{
		return;
	}

	// Remove all package assets
	loadedAssetsMap.erase( itPackage );
}

void FAssetDataBase::Shutdown()
{}