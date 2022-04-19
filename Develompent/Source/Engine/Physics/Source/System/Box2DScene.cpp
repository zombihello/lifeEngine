#if WITH_BOX2D
#include "Misc/PhysicsGlobals.h"
#include "System/PhysicsEngine.h"
#include "System/PhysicsBodyInstance.h"
#include "System/Box2DScene.h"
#include "Components/PrimitiveComponent.h"

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
	bx2World->Step( InDeltaTime, 8, 3 );
}

void FBox2DScene::Shutdown()
{
	if ( bx2World )
	{
		delete bx2World;
		bx2World = nullptr;
	}
}

void FBox2DScene::AddBody( class FPhysicsBodyInstance* InBodyInstance )
{
	bodies.push_back( InBodyInstance );

	FPhysicsActorHandleBox2D		actorHandle = InBodyInstance->GetActorHandle();
	for ( b2Fixture* bx2Fixture = actorHandle.bx2Body->GetFixtureList(); bx2Fixture; bx2Fixture = bx2Fixture->GetNext() )
	{
		FPhysicsShapeHandleBox2D*	shapeHandle = ( FPhysicsShapeHandleBox2D* )bx2Fixture->GetUserData().pointer;
		check( shapeHandle );
		fixturesMap[ shapeHandle->collisionProfile->objectType ][ InBodyInstance ].push_back( bx2Fixture );
	}
}

void FBox2DScene::RemoveBody( FPhysicsBodyInstance* InBodyInstance )
{
	for ( uint32 index = 0, count = bodies.size(); index < count; ++index )
	{
		FPhysicsBodyInstance*		bodyInstance = bodies[ index ];
		if ( bodyInstance == InBodyInstance )
		{
			// Remove all fixtures from collision channel map
			for ( uint32 index = 0; index << CC_Max; ++index )
			{
				fixturesMap[ index ].erase( InBodyInstance );
			}

			bodies.erase( bodies.begin() + index );
			return;
		}
	}
}

bool FBox2DScene::LineTraceSingleByChannel( FHitResult& OutHitResult, const FVector& InStart, const FVector& InEnd, ECollisionChannel InTraceChannel, const FCollisionQueryParams& InCollisionQueryParams /* = FCollisionQueryParams::defaultQueryParam */ )
{
	// Ray cast input
	b2RayCastInput				bx2RayCastInput;
	bx2RayCastInput.p1			= LE2BVector( ( FVector2D )InStart / BOX2D_SCALE );
	bx2RayCastInput.p2			= LE2BVector( ( FVector2D )InEnd / BOX2D_SCALE );
	bx2RayCastInput.maxFraction = 1;

	// Check every fixture of every collision channel to find closet
	const std::unordered_map< FPhysicsBodyInstance*, std::vector< b2Fixture* > >&		fixturesOnCollisionChannel = fixturesMap[ InTraceChannel ];
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
				OutHitResult.impactNormal	= FVector( B2LEVector( bx2RayCastOutput.normal ), 0.f );
				OutHitResult.impactPoint	= FVector( B2LEVector( bx2RayCastInput.p1 + bx2RayCastOutput.fraction * ( bx2RayCastInput.p2 - bx2RayCastInput.p1 ) ) * BOX2D_SCALE, 0.f );

				// If need return physics material
				if ( InCollisionQueryParams.bReturnPhysicalMaterial )
				{
					FPhysicsShapeHandleBox2D*		shapeHandle = ( FPhysicsShapeHandleBox2D* )bx2Fixture->GetUserData().pointer;
					check( shapeHandle );
					OutHitResult.physMaterial = shapeHandle->physMaterial;
				}

				return true;
			}
		}
	}

	return false;
}
#endif // WITH_BOX2D