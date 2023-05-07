#include "Actors/Actor.h"
#include "Components/ActorComponent.h"

IMPLEMENT_CLASS( CActorComponent )

/*
==================
CActorComponent::CActorComponent
==================
*/
CActorComponent::CActorComponent() 
#if WITH_EDITOR
	: bEditorOnly( false ),
#else
	:
#endif // WITH_EDITOR
	 owner( nullptr )
{}

/*
==================
CActorComponent::~CActorComponent
==================
*/
CActorComponent::~CActorComponent()
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