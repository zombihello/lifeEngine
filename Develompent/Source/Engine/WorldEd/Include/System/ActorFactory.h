/**
 * @file
 * @addtogroup WorldEd WorldEd
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef ACTORFACTORY_H
#define ACTORFACTORY_H

#include <unordered_map>

#include "Math/Math.h"
#include "System/Package.h"
#include "Actors/Actor.h"

/**
 * @ingroup WorldEd
 * Factory actors for spawn from assets in content browser
 */
class CActorFactory
{
public:
    /**
     * @brief Pointer to function for spawn and init actor
     * 
     * @param InAsset       Asset
     * @param InLocation    Location actor on spawn
	 * @param InRotation    Rotation actor on spawn
     * @return Return spawned actor. If failed returning NULL
     */
    typedef AActor* ( *SpawnActorFn_t )( const TSharedPtr<CAsset>& InAsset, const Vector& InLocation, const Quaternion& InRotation );

    /**
     * @brief Register actor for asset type
     * 
     * @param InAssetType       Asset type
     * @param InSpawnActorFn    Pointer to function for spawn and init actor
     */
    FORCEINLINE void Register( EAssetType InAssetType, SpawnActorFn_t InSpawnActorFn )
    {
        factoryFunctions[ InAssetType ] = InSpawnActorFn;
    }

    /**
     * @brief Spawn actor
     * 
     * @param InAsset       Asset
     * @param InLocation    Location actor on spawn
     * @param InRotation    Rotation actor on spawn
     * @return Return spawned actor. If failed returning NULL
     */
    FORCEINLINE AActor* Spawn( const TAssetHandle<CAsset>& InAsset, const Vector& InLocation, const Quaternion& InRotation = Math::quaternionZero )
    {
        // If asset is not valid, we exit from method
        if ( !InAsset.IsAssetValid() )
        {
            return nullptr;
        }

        // Getting asset and find factory function. If this function is not valid, exit from method
        TSharedPtr<CAsset>      asset = InAsset.ToSharedPtr();
        auto    itFactory = factoryFunctions.find( asset->GetType() );
        if ( itFactory == factoryFunctions.end() || !itFactory->second )
        {
            return nullptr;
        }

        // Call factory function for current asset
        return itFactory->second( asset, InLocation, InRotation );
    }

    /**
     * @brief Is registered actor for asset type
     * 
     * @param InAssetType       Asset type
     * @return Return TRUE in case successed, otherwise returning FALSE
     */
    FORCEINLINE bool IsRegistered( EAssetType InAssetType ) const
    {
        auto    itFind = factoryFunctions.find( InAssetType );
        return itFind != factoryFunctions.end() && itFind->second;
    }

private:
	std::unordered_map<EAssetType, SpawnActorFn_t>      factoryFunctions;       /**< Map of factory functions */
};

#endif // !ACTORFACTORY_H
