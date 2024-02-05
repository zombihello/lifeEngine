/**
 * @file
 * @addtogroup Physics Physics
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

#ifndef PHYSICSENGINE_H
#define PHYSICSENGINE_H

#include <string>
#include <unordered_map>

#include "Core/Logger/LoggerMacros.h"
#include "Core/Core.h"
#include "Physics/System/PhysicsMaterial.h"

/**
 * @ingroup Physics
 * @brief Main class of physics engine
 */
class CPhysicsEngine
{
public:
	/**
	 * @brief Constructor
	 */
	CPhysicsEngine();

	/**
	 * @brief Destructor
	 */
	~CPhysicsEngine();

	/**
	 * @brief Initialize engine
	 */
	void Init();

	/**
	 * @brief Tick engine
	 * 
	 * @param InDeltaTime The time since the last tick
	 */
	void Tick( float InDeltaTime );

	/**
	 * @brief Shutdown engine
	 */
	void Shutdown();

	/**
	 * @brief Find collision profile
	 *
	 * @param InName Name of collision profile
	 * @return Return pointer to finded collision profile. If not founded retulr nullptr
	 */
	FORCEINLINE CollisionProfile* FindCollisionProfile( const std::wstring& InName ) const
	{
		auto	itProfile = collisionProfiles.find( InName );
		if ( itProfile == collisionProfiles.end() )
		{
			Warnf( TEXT( "Collision profile '%s' not founded\n" ), InName.c_str() );
			return nullptr;
		}

		return &itProfile->second;
	}

	/**
	 * @brief Get default physics material
	 * @return Return default physics material
	 */
	FORCEINLINE TAssetHandle<CPhysicsMaterial> GetDefaultPhysMaterial() const
	{
		return defaultPhysMaterial;
	}

private:
	TAssetHandle<CPhysicsMaterial>										defaultPhysMaterial;			/**< Default physics material */
	mutable std::unordered_map< std::wstring,  CollisionProfile >		collisionProfiles;				/**< Collision profiles map */
};

#endif // !PHYSICSENGINE_H