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

#if WITH_BOX2D
#include "Physics/Misc/PhysicsGlobals.h"
#include "Physics/System/PhysicsEngine.h"
#include "Physics/System/PhysicsBodyInstance.h"
#include "Physics/System/Box2DScene.h"
#include "Physics/System/PhysicsMaterial.h"
#include "Engine/Components/PrimitiveComponent.h"

/*
==================
CBox2DScene::CBox2DScene
==================
*/
CBox2DScene::CBox2DScene()
	: bx2World( nullptr )
{}

/*
==================
CBox2DScene::~CBox2DScene
==================
*/
CBox2DScene::~CBox2DScene()
{
	Shutdown();
}

/*
==================
CBox2DScene::Init
==================
*/
void CBox2DScene::Init()
{
	Assert( !bx2World );
	bx2World = new b2World( b2Vec2( 0.f, -9.81f ) );
}

/*
==================
CBox2DScene::Tick
==================
*/
void CBox2DScene::Tick( float InDeltaTime )
{
	bx2World->Step( InDeltaTime, 8, 3 );
}

/*
==================
CBox2DScene::Shutdown
==================
*/
void CBox2DScene::Shutdown()
{
	if ( bx2World )
	{
		delete bx2World;
		bx2World = nullptr;
	}
}

/*
==================
CBox2DScene::AddBody
==================
*/
void CBox2DScene::AddBody( class CPhysicsBodyInstance* InBodyInstance )
{
	bodies.push_back( InBodyInstance );

	PhysicsActorHandleBox2D		actorHandle = InBodyInstance->GetActorHandle();
	for ( b2Fixture* bx2Fixture = actorHandle.bx2Body->GetFixtureList(); bx2Fixture; bx2Fixture = bx2Fixture->GetNext() )
	{
		PhysicsShapeHandleBox2D*	shapeHandle = ( PhysicsShapeHandleBox2D* )bx2Fixture->GetUserData().pointer;
		Assert( shapeHandle );
		fixturesMap[ shapeHandle->collisionProfile->objectType ][ InBodyInstance ].push_back( bx2Fixture );
	}
}

/*
==================
CBox2DScene::RemoveBody
==================
*/
void CBox2DScene::RemoveBody( CPhysicsBodyInstance* InBodyInstance )
{
	for ( uint32 index = 0, count = bodies.size(); index < count; ++index )
	{
		CPhysicsBodyInstance*		bodyInstance = bodies[ index ];
		if ( bodyInstance == InBodyInstance )
		{
			// Remove all fixtures from collision channel map
			for ( uint32 index = 0; index < CC_Max; ++index )
			{
				fixturesMap[ index ].erase( InBodyInstance );
			}

			bodies.erase( bodies.begin() + index );
			return;
		}
	}
}

/*
==================
CBox2DScene::LineTraceSingleByChannel
==================
*/
bool CBox2DScene::LineTraceSingleByChannel( HitResult& OutHitResult, const Vector& InStart, const Vector& InEnd, ECollisionChannel InTraceChannel, const CollisionQueryParams& InCollisionQueryParams /* = CollisionQueryParams::defaultQueryParam */ )
{
	// Ray cast input
	b2RayCastInput				bx2RayCastInput;
	bx2RayCastInput.p1			= LE2BVector( ( Vector2D )InStart / BOX2D_SCALE );
	bx2RayCastInput.p2			= LE2BVector( ( Vector2D )InEnd / BOX2D_SCALE );
	bx2RayCastInput.maxFraction = 1;

	// Check every fixture of every collision channel to find closet
	const std::unordered_map< CPhysicsBodyInstance*, std::vector< b2Fixture* > >&		fixturesOnCollisionChannel = fixturesMap[ InTraceChannel ];
	for ( auto it = fixturesOnCollisionChannel.begin(), itEnd = fixturesOnCollisionChannel.end(); it != itEnd; ++it )
	{
		for ( uint32 index = 0, count = it->second.size(); index < count; ++index )
		{
			b2RayCastOutput			bx2RayCastOutput;
			b2Fixture*				bx2Fixture = it->second[ index ];
			if ( bx2Fixture->RayCast( &bx2RayCastOutput, bx2RayCastInput, 0 ) )
			{
				// Fill hit result
				OutHitResult.component		= it->first->GetOwnerComponent();
				OutHitResult.actor			= OutHitResult.component->GetOwner();
				OutHitResult.impactNormal	= Vector( B2LEVector( bx2RayCastOutput.normal ), 0.f );
				OutHitResult.impactPoint	= Vector( B2LEVector( bx2RayCastInput.p1 + bx2RayCastOutput.fraction * ( bx2RayCastInput.p2 - bx2RayCastInput.p1 ) ) * BOX2D_SCALE, 0.f );

				// If need return physics material
				if ( InCollisionQueryParams.bReturnPhysicalMaterial )
				{
					PhysicsShapeHandleBox2D*		shapeHandle = ( PhysicsShapeHandleBox2D* )bx2Fixture->GetUserData().pointer;
					Assert( shapeHandle );
					OutHitResult.physMaterial	= shapeHandle->physMaterial.ToSharedPtr();
				}

				return true;
			}
		}
	}

	return false;
}
#endif // WITH_BOX2D