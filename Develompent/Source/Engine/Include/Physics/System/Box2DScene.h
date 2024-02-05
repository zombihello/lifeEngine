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

#ifndef BOX2DSSCENE_H
#define BOX2DSSCENE_H

#if WITH_BOX2D
#include <box2d/box2d.h>
#include <vector>

#include "Core/Math/Math.h"
#include "Core/CoreDefines.h"
#include "Physics/Misc/PhysicsTypes.h"

/**
 * @ingroup Physics
 * @brief Class of PhysX scene
 */
class CBox2DScene
{
public:
	/**
	 * @brief Constructor
	 */
	CBox2DScene();

	/**
	 * @brief Destructor
	 */
	~CBox2DScene();

	/**
	 * @brief Initialize scene
	 */
	void Init();

	/**
	 * @brief Tick scene
	 * 
	 * @param InDeltaTime The time since the last tick
	 */
	void Tick( float InDeltaTime );

	/**
	 * @brief Shutdown scene
	 */
	void Shutdown();

	/**
	 * @brief Add body on scene
	 * @param InBodyInstance Pointer to body instance
	 */
	void AddBody( class CPhysicsBodyInstance* InBodyInstance );

	/**
	 * @brief Remove body from scene
	 * @param InBodyInstance Pointer to body instance
	 */
	void RemoveBody( class CPhysicsBodyInstance* InBodyInstance );

	/**
	 * @brief Remove all bodies from scene
	 */
	FORCEINLINE void RemoveAllBodies()
	{
		bodies.clear();
		for ( uint32 index = 0; index < CC_Max; ++index )
		{
			fixturesMap[ index ].clear();
		}
	}

	/**
	 * Trace a ray against the world using a specific channel and return the first blocking hit
	 *
	 * @param OutHitResult Hit result
	 * @param InStart Start ray
	 * @param InEnd End ray
	 * @param InTraceChannel Trace channel
	 * @param InCollisionQueryParams Collision query params
	 * @return Return TRUE if a blocking hit is found, else false
	 */
	bool LineTraceSingleByChannel( HitResult& OutHitResult, const Vector& InStart, const Vector& InEnd, ECollisionChannel InTraceChannel, const CollisionQueryParams& InCollisionQueryParams = CollisionQueryParams::defaultQueryParam );

	/**
	 * @brief Get Box2D world
	 * @return Return Box2D world
	 */
	FORCEINLINE b2World* GetBox2DWorld() const
	{
		return bx2World;
	}

private:
	b2World*																		bx2World;						/**< Box2D world */
	std::vector< class CPhysicsBodyInstance* >										bodies;							/**< Array of bodies on scene */
	std::unordered_map< class CPhysicsBodyInstance*, std::vector< b2Fixture* > >	fixturesMap[ CC_Max ];			/**< Map of fixtures each collision channel */
};
#endif // WITH_BOX2D

#endif // !BOX2DSSCENE_H