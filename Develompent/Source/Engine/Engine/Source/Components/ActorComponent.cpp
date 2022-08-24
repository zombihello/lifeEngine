#include "Actors/Actor.h"
#include "Components/ActorComponent.h"

IMPLEMENT_CLASS( CActorComponent )

CActorComponent::CActorComponent() 
#if WITH_EDITOR
	: bEditorOnly( false ),
#else
	:
#endif // WITH_EDITOR
	 owner( nullptr )
{}

CActorComponent::~CActorComponent()
{}

void CActorComponent::BeginPlay()
{}

void CActorComponent::EndPlay()
{}

void CActorComponent::TickComponent( float InDeltaTime )
{}

void CActorComponent::Spawned()
{}

void CActorComponent::Destroyed()
{}