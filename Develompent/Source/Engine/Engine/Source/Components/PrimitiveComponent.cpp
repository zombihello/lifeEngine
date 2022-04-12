#include "Components/PrimitiveComponent.h"
#include "Misc/EngineGlobals.h"
#include "System/World.h"
#include "Actors/Actor.h"
#include "Render/Scene.h"

IMPLEMENT_CLASS( LPrimitiveComponent )

LPrimitiveComponent::LPrimitiveComponent()
	: bVisibility( true )
	, bCollision( false )
	, bDirtyBodyInstance( false )
	, bodyLockFlags( BLF_None )
	, bodyMass( 1.f )
{}

void LPrimitiveComponent::BeginPlay()
{
	Super::BeginPlay();
	SetVisibility( bVisibility );
}

void LPrimitiveComponent::TickComponent( float InDeltaTime )
{
	Super::TickComponent( InDeltaTime );

	// If body instance is dirty - reinit physics component
	if ( bDirtyBodyInstance )
	{
		TermPrimitivePhysics();
		InitPrimitivePhysics();
		bDirtyBodyInstance = false;
	}
}

void LPrimitiveComponent::Serialize( class FArchive& InArchive )
{
	Super::Serialize( InArchive );
	InArchive << bVisibility;
	InArchive << bCollision;
	InArchive << bodyLockFlags;
}

void LPrimitiveComponent::SetVisibility( bool InNewVisibility )
{
	bVisibility = InNewVisibility;
	FBaseScene*		scene = GWorld->GetScene();
	check( scene );

	if ( bVisibility )
	{
		scene->AddPrimitive( this );
	}
	else
	{
		scene->RemovePrimitive( this );
	}
}

void LPrimitiveComponent::InitPrimitivePhysics()
{
	if ( bodySetup && bCollision )
	{
		AActor*			actorOwner = GetOwner();
		bodyInstance.SetDynamic( actorOwner ? !actorOwner->IsStatic() : false );
		bodyInstance.SetLockFlags( bodyLockFlags );
		bodyInstance.SetMass( bodyMass );
		bodyInstance.InitBody( bodySetup, GetComponentTransform(), this );
	}
}

void LPrimitiveComponent::SyncComponentToPhysics()
{
	if ( bodyInstance.IsValid() )
	{
		AActor*		actorOwner = GetOwner();
		check( actorOwner );

		FTransform		oldTransform = actorOwner->GetActorTransform();
		FTransform		newTransform = bodyInstance.GetLEWorldTransform();
		if ( !oldTransform.MatchesNoScale( newTransform ) )
		{
			actorOwner->SetActorLocation( newTransform.GetLocation() );
			actorOwner->SetActorRotation( newTransform.GetRotation() );
		}
	}
}

void LPrimitiveComponent::TermPrimitivePhysics()
{
	if ( bodyInstance.IsValid() )
	{
		bodyInstance.TermBody();
	}
}

FPhysicsBodyInstance* LPrimitiveComponent::GetBodyInstance()
{
	AActor*					actorOwner = GetOwner();
	if ( !actorOwner )
	{
		return nullptr;
	}

	LPrimitiveComponent*	collisionComponent = actorOwner->GetCollisionComponent();
	if ( !collisionComponent )
	{
		return nullptr;
	}

	return collisionComponent == this ? &bodyInstance : collisionComponent->GetBodyInstance();
}

const FPhysicsBodyInstance* LPrimitiveComponent::GetBodyInstance() const
{
	AActor*					actorOwner = GetOwner();
	if ( !actorOwner )
	{
		return nullptr;
	}

	LPrimitiveComponent*	collisionComponent = actorOwner->GetCollisionComponent();
	if ( !collisionComponent )
	{
		return nullptr;
	}

	return collisionComponent == this ? &bodyInstance : collisionComponent->GetBodyInstance();
}