#include "Components/PrimitiveComponent.h"
#include "Misc/EngineGlobals.h"
#include "System/World.h"
#include "Actors/Actor.h"
#include "Render/Scene.h"

IMPLEMENT_CLASS( LPrimitiveComponent )

LPrimitiveComponent::LPrimitiveComponent()
	: bVisibility( true )
{}

void LPrimitiveComponent::BeginPlay()
{
	Super::BeginPlay();
	SetVisibility( bVisibility );
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
	if ( bodySetup )
	{
		AActor*			actorOwner = GetOwner();
		bodyInstance.SetDynamic( actorOwner ? !actorOwner->IsStatic() : false );
		bodyInstance.SetLockFlags( BLF_LockMoveZ | BLF_LockRotateX | BLF_LockRotateY );
		bodyInstance.InitBody( bodySetup, GetComponentTransform(), this );
	}
}

void LPrimitiveComponent::SyncComponentToPhysics()
{}

void LPrimitiveComponent::TermPrimitivePhysics()
{
	if ( bodyInstance.IsValid() )
	{
		bodyInstance.TermBody();
	}
}