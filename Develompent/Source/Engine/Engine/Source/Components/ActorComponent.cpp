#include "Actors/Actor.h"
#include "Components/ActorComponent.h"

IMPLEMENT_CLASS( CActorComponent )
IMPLEMENT_DEFAULT_INITIALIZE_CLASS( CActorComponent )

/*
==================
CActorComponent::CActorComponent
==================
*/
CActorComponent::CActorComponent() 
#if WITH_EDITOR
	: bEditorOnly( false )
#endif // WITH_EDITOR
{}

/*
==================
CActorComponent::BeginPlay
==================
*/
void CActorComponent::BeginPlay()
{}

/*
==================
CActorComponent::EndPlay
==================
*/
void CActorComponent::EndPlay()
{}

/*
==================
CActorComponent::TickComponent
==================
*/
void CActorComponent::TickComponent( float InDeltaTime )
{}

/*
==================
CActorComponent::Spawned
==================
*/
void CActorComponent::Spawned()
{}

/*
==================
CActorComponent::Destroyed
==================
*/
void CActorComponent::Destroyed()
{}

/*
==================
CActorComponent::GetOwner
==================
*/
AActor* CActorComponent::GetOwner() const
{
	return Cast<AActor>( GetOuter() );
}