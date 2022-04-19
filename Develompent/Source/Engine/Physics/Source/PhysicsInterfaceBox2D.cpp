#if WITH_BOX2D
#include "Logger/LoggerMacros.h"
#include "System/PhysicsMaterial.h"
#include "System/PhysicsBoxGeometry.h"
#include "PhysicsInterfaceBox2D.h"

static uint32 GCollisionChannelBits[] =
{
	0x0001,			// CC_WorldStatic
	0x0002,			// CC_Visibility
	0x0004,			// CC_Character
};
static_assert( ARRAY_COUNT( GCollisionChannelBits ) == CC_Max, "Need full init GCollisionChannelBits array" );

void FPhysicsActorHandleBox2D::OnMaterialUpdated( class FPhysicsMaterial* InPhysMaterial )
{
	for ( auto itFixture = fixtureMap.begin(), itFixtureEnd = fixtureMap.end(); itFixture != itFixtureEnd; ++itFixture )
	{
		b2Fixture*						bx2Fixture = itFixture->second;
		FPhysicsShapeHandleBox2D*		shapeHandle = ( FPhysicsShapeHandleBox2D* )bx2Fixture->GetUserData().pointer;
		check( shapeHandle );

		if ( shapeHandle->physMaterial != InPhysMaterial )
		{
			continue;
		}

		bx2Fixture->SetDensity( shapeHandle->physMaterial->GetDensity() );
		bx2Fixture->SetFriction( bx2Body->GetType() == b2_staticBody ? shapeHandle->physMaterial->GetStaticFriction() : shapeHandle->physMaterial->GetDynamicFriction() );
		bx2Fixture->SetRestitution( shapeHandle->physMaterial->GetRestitution() );
	}
}

void FPhysicsInterfaceBox2D::Init()
{
	LE_LOG( LT_Log, LC_Init, TEXT( "Box2D version: %i.%i.%i" ), b2_version.major, b2_version.minor, b2_version.revision );
}

void FPhysicsInterfaceBox2D::Shutdown()
{}

FPhysicsShapeHandleBox2D FPhysicsInterfaceBox2D::CreateShapeGeometry( const struct FPhysicsBoxGeometry& InBoxGeometry )
{
	FPhysicsShapeHandleBox2D		shapeHandle;
	b2PolygonShape*					bx2BoxGeometry		= new b2PolygonShape();
	const b2Vec2					sizeInMeters		= b2Vec2( InBoxGeometry.extent.x / BOX2D_SCALE, InBoxGeometry.extent.y / BOX2D_SCALE );
	const b2Vec2					locationInMeters	= b2Vec2( InBoxGeometry.location.x / BOX2D_SCALE, InBoxGeometry.location.y / BOX2D_SCALE );
	const float						rotationInRadians	= InBoxGeometry.rotation.roll / BOX2D_ANGLES;
	
	b2Vec2							boxVerteces[] =
	{
		b2Vec2( 0.f,				0.f ),
		b2Vec2( sizeInMeters.x,		0.f ),
		b2Vec2( sizeInMeters.x,		sizeInMeters.y ),
		b2Vec2( 0.f,				sizeInMeters.y )
	};

	b2Transform			bx2Transform( locationInMeters, b2Rot( rotationInRadians ) );
	for ( uint32 index = 0, count = ARRAY_COUNT( boxVerteces ); index < count; ++index )
	{
		boxVerteces[ index ] = b2Mul( bx2Transform, boxVerteces[ index ] );
	}
	bx2BoxGeometry->Set( &boxVerteces[ 0 ], ARRAY_COUNT( boxVerteces ) );
	
	shapeHandle.bx2Shape			= bx2BoxGeometry;
	shapeHandle.physMaterial		= InBoxGeometry.material;
	shapeHandle.collisionProfile	= InBoxGeometry.collisionProfile;
	return shapeHandle;
}

FPhysicsActorHandleBox2D FPhysicsInterfaceBox2D::CreateActor( const FActorCreationParams& InParams )
{
	FPhysicsActorHandle			actorHandle;
	b2BodyDef					bx2BodyDef;
	bx2BodyDef.type				= InParams.bStatic ? b2_staticBody : b2_dynamicBody;
	{
		b2Transform				bx2Transform = LE2BTransform( InParams.initialTM );
		bx2BodyDef.position		= bx2Transform.p;
		bx2BodyDef.angle		= bx2Transform.q.GetAngle();
	}
	bx2BodyDef.awake			= InParams.bStartAwake;
	bx2BodyDef.fixedRotation	= LE2BLockFlags( InParams.lockFlags );
	bx2BodyDef.enabled			= InParams.bSimulatePhysics;
	bx2BodyDef.gravityScale		= InParams.bEnableGravity ? 1.f : 0.f;
	actorHandle.bx2Body			= GPhysicsScene.GetBox2DWorld()->CreateBody( &bx2BodyDef );

	b2MassData			bx2MassData;
	bx2MassData.mass	= InParams.mass;
	bx2MassData.center	= b2Vec2( 0.f, 0.f );
	bx2MassData.I		= 0.f;
	actorHandle.bx2Body->SetMassData( &bx2MassData );

	return actorHandle;
}

void FPhysicsInterfaceBox2D::AttachShape( FPhysicsActorHandleBox2D& InActorHandle, const FPhysicsShapeHandleBox2D& InShapeHandle )
{
	check( IsValidActor( InActorHandle ) && IsValidShapeGeometry( InShapeHandle ) );

	// If we already attach this shape - exit from function
	{
		auto		it = InActorHandle.fixtureMap.find( InShapeHandle.bx2Shape );
		if ( it != InActorHandle.fixtureMap.end() )
		{
			return;
		}
	}

	b2FixtureUserData			bx2FixtureUserData;
	bx2FixtureUserData.pointer	= ( uintptr_t )( &InShapeHandle );

	b2FixtureDef						bx2FixtureDef;
	bx2FixtureDef.shape					= InShapeHandle.bx2Shape;
	bx2FixtureDef.friction				= InActorHandle.bx2Body->GetType() == b2_staticBody ? InShapeHandle.physMaterial->GetStaticFriction() : InShapeHandle.physMaterial->GetDynamicFriction();
	bx2FixtureDef.density				= InShapeHandle.physMaterial->GetDensity();
	bx2FixtureDef.restitution			= InShapeHandle.physMaterial->GetRestitution();
	bx2FixtureDef.userData				= bx2FixtureUserData;
	bx2FixtureDef.filter.categoryBits	= GCollisionChannelBits[ InShapeHandle.collisionProfile->objectType ];
	bx2FixtureDef.filter.maskBits		= 0x0;
	for ( uint32 indexCollisionChannel = 0; indexCollisionChannel < CC_Max; ++indexCollisionChannel )
	{
		if ( InShapeHandle.collisionProfile->responses[ indexCollisionChannel ] != CR_Block )
		{
			continue;
		}

		bx2FixtureDef.filter.maskBits |= GCollisionChannelBits[ indexCollisionChannel ];
	}

	b2Fixture*		bx2Fixture = InActorHandle.bx2Body->CreateFixture( &bx2FixtureDef );
	InActorHandle.fixtureMap[ InShapeHandle.bx2Shape ]		= bx2Fixture;
	InShapeHandle.physMaterial->OnPhysicsMaterialUpdate().Add( &InActorHandle, &FPhysicsActorHandleBox2D::OnMaterialUpdated );
}

void FPhysicsInterfaceBox2D::DetachShape( FPhysicsActorHandleBox2D& InActorHandle, const FPhysicsShapeHandleBox2D& InShapeHandle )
{
	check( IsValidActor( InActorHandle ) && IsValidShapeGeometry( InShapeHandle ) );

	// Remove fixture
	auto		itFixture = InActorHandle.fixtureMap.find( InShapeHandle.bx2Shape );
	if ( itFixture == InActorHandle.fixtureMap.end() )
	{
		return;
	}

	InActorHandle.bx2Body->DestroyFixture( itFixture->second );
	InActorHandle.fixtureMap.erase( itFixture );
	InShapeHandle.physMaterial->OnPhysicsMaterialUpdate().Remove( &InActorHandle, &FPhysicsActorHandleBox2D::OnMaterialUpdated );
}
#endif // WITH_BOX2D