#include "Actors/Actor.h"
#include "Components/ActorComponent.h"

IMPLEMENT_CLASS( LActorComponent )

LActorComponent::LActorComponent() :
	owner( nullptr )
{}

LActorComponent::~LActorComponent()
{}

void LActorComponent::BeginPlay()
{}

void LActorComponent::TickComponent( float InDeltaTime )
{}