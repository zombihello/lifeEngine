#include "Actors/Actor.h"
#include "Components/ActorComponent.h"

IMPLEMENT_CLASS( CActorComponent )

CActorComponent::CActorComponent() :
	owner( nullptr )
{}

CActorComponent::~CActorComponent()
{}

void CActorComponent::BeginPlay()
{}

void CActorComponent::TickComponent( float InDeltaTime )
{}