/**
 * @file
 * @addtogroup Editor Editor
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

#ifndef ACTORFACTORY_H
#define ACTORFACTORY_H

#include <unordered_map>

#include "Core/Math/Math.h"
#include "Core/System/Package.h"
#include "Engine/Actors/Actor.h"

/**
 * @ingroup Editor
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
    typedef ActorRef_t ( *SpawnActorFn_t )( const TSharedPtr<CAsset>& InAsset, const Vector& InLocation, const Quaternion& InRotation );

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
    FORCEINLINE ActorRef_t Spawn( const TAssetHandle<CAsset>& InAsset, const Vector& InLocation, const Quaternion& InRotation = Math::quaternionZero )
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
