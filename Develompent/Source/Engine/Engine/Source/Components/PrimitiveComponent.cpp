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

void LPrimitiveComponent::TickComponent( float InDeltaTime )
{
	Super::TickComponent( InDeltaTime );

	// If body instance is dirty - reinit physics component
	if ( bodyInstance.IsDirty() || bodySetup != bodyInstance.GetBodySetup() )
	{
		TermPrimitivePhysics();
		InitPrimitivePhysics();
	}
}

void LPrimitiveComponent::Serialize( class FArchive& InArchive )
{
	Super::Serialize( InArchive );
	InArchive << bVisibility;
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

#if ENGINE_2D
		// For 2D game we copy to new transform Z coord (in 2D this is layer)
		newTransform.AddToTranslation( FVector( 0.f, 0.f, oldTransform.GetLocation().z ) );
#endif // ENGINE_2D

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