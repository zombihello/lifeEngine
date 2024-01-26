#include "Components/PrimitiveComponent.h"
#include "Misc/EngineGlobals.h"
#include "System/World.h"
#include "Actors/Actor.h"
#include "Render/Scene.h"

IMPLEMENT_CLASS( CPrimitiveComponent )

/*
==================
CPrimitiveComponent::CPrimitiveComponent
==================
*/
CPrimitiveComponent::CPrimitiveComponent()
	: bIsDirtyDrawingPolicyLink( true )
	, bVisibility( true )
	, scene( nullptr )
{}

/*
==================
CPrimitiveComponent::BeginDestroy
==================
*/
void CPrimitiveComponent::BeginDestroy()
{
	Super::BeginDestroy();
	if ( scene )
	{
		scene->RemovePrimitive( this );
	}
}

/*
==================
CPrimitiveComponent::StaticInitializeClass
==================
*/
void CPrimitiveComponent::StaticInitializeClass()
{
	new( staticClass, TEXT( "bVisibility" ), OBJECT_Public ) CBoolProperty( CPP_PROPERTY( ThisClass, bVisibility ), TEXT( "Drawing" ), TEXT( "Is primitive visibility" ), CPF_Edit );
}

/*
==================
CPrimitiveComponent::Spawned
==================
*/
void CPrimitiveComponent::Spawned()
{
	Super::Spawned();
	GetWorld()->GetScene()->AddPrimitive( this );
}

/*
==================
CPrimitiveComponent::Destroyed
==================
*/
void CPrimitiveComponent::Destroyed()
{
	Super::Destroyed();
	GetWorld()->GetScene()->RemovePrimitive( this );
}

/*
==================
CPrimitiveComponent::TickComponent
==================
*/
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

/*
==================
CPrimitiveComponent::LinkDrawList
==================
*/
void CPrimitiveComponent::LinkDrawList()
{}

/*
==================
CPrimitiveComponent::UnlinkDrawList
==================
*/
void CPrimitiveComponent::UnlinkDrawList()
{}

/*
==================
CPrimitiveComponent::AddToDrawList
==================
*/
void CPrimitiveComponent::AddToDrawList( const class CSceneView& InSceneView )
{}

/*
==================
CPrimitiveComponent::InitPrimitivePhysics
==================
*/
void CPrimitiveComponent::InitPrimitivePhysics()
{
	if ( bodySetup )
	{
		AActor*			actorOwner = GetOwner();
		bodyInstance.SetDynamic( actorOwner ? !actorOwner->IsStatic() : false );
		bodyInstance.InitBody( bodySetup, GetComponentTransform(), this );
	}
}

/*
==================
CPrimitiveComponent::SyncComponentToPhysics
==================
*/
void CPrimitiveComponent::SyncComponentToPhysics()
{
	if ( bodyInstance.IsValid() )
	{
		AActor*		actorOwner = GetOwner();
		Assert( actorOwner );

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

/*
==================
CPrimitiveComponent::TermPrimitivePhysics
==================
*/
void CPrimitiveComponent::TermPrimitivePhysics()
{
	if ( bodyInstance.IsValid() )
	{
		bodyInstance.TermBody();
	}
}

/*
==================
CPrimitiveComponent::IsVisibility
==================
*/
bool CPrimitiveComponent::IsVisibility() const
{
	return bVisibility && ( GetOwner() ? GetOwner()->IsVisibility() : true );
}