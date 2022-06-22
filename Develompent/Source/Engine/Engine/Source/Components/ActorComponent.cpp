#include "Actors/Actor.h"
#include "Components/ActorComponent.h"

IMPLEMENT_CLASS( CActorComponent )

CActorComponent::CActorComponent() 
	: owner( nullptr )
{}

CActorComponent::~CActorComponent()
{}

void CActorComponent::BeginPlay()
{}

void CActorComponent::EndPlay()
{}

void CActorComponent::TickComponent( float InDeltaTime )
{}

void CActorComponent::Destroyed()
{}