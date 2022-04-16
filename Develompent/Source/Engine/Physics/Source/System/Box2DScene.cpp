#if WITH_BOX2D
#include "Misc/PhysicsGlobals.h"
#include "System/PhysicsEngine.h"
#include "System/PhysicsBodyInstance.h"
#include "System/Box2DScene.h"

FBox2DScene::FBox2DScene()
	: bx2World( nullptr )
{}

FBox2DScene::~FBox2DScene()
{
	Shutdown();
}

void FBox2DScene::Init()
{
	check( !bx2World );
	bx2World = new b2World( b2Vec2( 0.f, -9.81f ) );
}

void FBox2DScene::Tick( float InDeltaTime )
{
	bx2World->Step( 1.f / 60.f, 8, 3 );
}

void FBox2DScene::Shutdown()
{
	if ( bx2World )
	{
		delete bx2World;
		bx2World = nullptr;
	}
}

void FBox2DScene::RemoveBody( FPhysicsBodyInstance* InBodyInstance )
{
	for ( uint32 index = 0, count = bodies.size(); index < count; ++index )
	{
		FPhysicsBodyInstance*		bodyInstance = bodies[ index ];
		if ( bodyInstance == InBodyInstance )
		{
			bodies.erase( bodies.begin() + index );
			return;
		}
	}
}
#endif // WITH_BOX2D