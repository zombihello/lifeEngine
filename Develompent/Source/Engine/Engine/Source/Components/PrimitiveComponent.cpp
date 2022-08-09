#include "Components/PrimitiveComponent.h"
#include "Misc/EngineGlobals.h"
#include "System/World.h"
#include "Actors/Actor.h"
#include "Render/Scene.h"

IMPLEMENT_CLASS( CPrimitiveComponent )

CPrimitiveComponent::CPrimitiveComponent()
	: bIsDirtyDrawingPolicyLink( true )
	, bVisibility( true )
	, scene( nullptr )
{}

CPrimitiveComponent::~CPrimitiveComponent()
{
	if ( scene )
	{
		scene->RemovePrimitive( this );
	}
}

void CPrimitiveComponent::Spawned()
{
	Super::Spawned();
	GWorld->GetScene()->AddPrimitive( this );
}

void CPrimitiveComponent::Destroyed()
{
	Super::Destroyed();
	GWorld->GetScene()->RemovePrimitive( this );
}

void CPrimitiveComponent::TickComponent( float InDeltaTime )
{
	Super::TickComponent( InDeltaTime );

	// If body instance is dirty - reinit physics component
	if ( bodyInstance.IsDirty() || bodySetup != bodyInstance.GetBodySetup() )
	{
		TermPrimitivePhysics();
		InitPrimitivePhysics();
	}
}

void CPrimitiveComponent::Serialize( class CArchive& InArchive )
{
	Super::Serialize( InArchive );
	InArchive << bVisibility;
}

void CPrimitiveComponent::LinkDrawList()
{}

void CPrimitiveComponent::UnlinkDrawList()
{}

void CPrimitiveComponent::AddToDrawList( const class CSceneView& InSceneView )
{}

void CPrimitiveComponent::InitPrimitivePhysics()
{
	if ( bodySetup )
	{
		AActor*			actorOwner = GetOwner();
		bodyInstance.SetDynamic( actorOwner ? !actorOwner->IsStatic() : false );
		bodyInstance.InitBody( bodySetup, GetComponentTransform(), this );
	}
}

void CPrimitiveComponent::SyncComponentToPhysics()
{
	if ( bodyInstance.IsValid() )
	{
		AActor*		actorOwner = GetOwner();
		check( actorOwner );

		CTransform		oldTransform = actorOwner->GetActorTransform();
		CTransform		newTransform = bodyInstance.GetLEWorldTransform();

#if ENGINE_2D
		// For 2D game we copy to new transform Z coord (in 2D this is layer)
		newTransform.AddToTranslation( Vector( 0.f, 0.f, oldTransform.GetLocation().z ) );
#endif // ENGINE_2D

		if ( !oldTransform.MatchesNoScale( newTransform ) )
		{
			actorOwner->SetActorLocation( newTransform.GetLocation() );
			actorOwner->SetActorRotation( newTransform.GetRotation() );
		}
	}
}

void CPrimitiveComponent::TermPrimitivePhysics()
{
	if ( bodyInstance.IsValid() )
	{
		bodyInstance.TermBody();
	}
}