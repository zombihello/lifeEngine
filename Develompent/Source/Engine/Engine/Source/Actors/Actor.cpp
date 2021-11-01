#include "Actors/Actor.h"
#include "Components/ActorComponent.h"

IMPLEMENT_CLASS( AActor )

AActor::~AActor()
{
	ResetOwnedComponents();
}

void AActor::BeginPlay()
{
	for ( uint32 index = 0, count = ( uint32 )ownedComponents.size(); index < count; ++index )
	{
		ownedComponents[ index ]->BeginPlay();
	}
}

void AActor::Tick( float InDeltaTime )
{
	for ( uint32 index = 0, count = ( uint32 )ownedComponents.size(); index < count; ++index )
	{
		ownedComponents[ index ]->TickComponent( InDeltaTime );
	}
}

LActorComponentRef AActor::CreateComponent( LClass* InClass, const tchar* InName )
{
	check( InClass );

	LObject*				newObject = InClass->CreateObject();
	check( newObject->IsA< LActorComponent >() );

	LActorComponent*		component = newObject->Cast< LActorComponent >();
	check( component );

	// If created component is a LSceneComponent and RootComponent not setted - set it!
	if ( !rootComponent && component->IsA< LSceneComponent >() )
	{
		rootComponent = component->Cast< LSceneComponent >();
	}

	component->SetName( InName );
	AddOwnedComponent( component );
	return component;
}

void AActor::AddOwnedComponent( class LActorComponent* InComponent )
{
	check( InComponent && InComponent->GetOwner() != this );

	// If already owned exist in component - remove component from owner
	{
		AActor*		owner = InComponent->GetOwner();
		if ( owner )
		{
			owner->RemoveOwnedComponent( InComponent );
		}
	}

	// Add component
	InComponent->SetOwner( this );
	ownedComponents.push_back( InComponent );
}

void AActor::RemoveOwnedComponent( class LActorComponent* InComponent )
{
	check( InComponent && InComponent->GetOwner() == this );
	for ( uint32 index = 0, count = ( uint32 )ownedComponents.size(); index < count; ++index )
	{
		LActorComponent*		component = ownedComponents[ index ];
		if ( component == InComponent )
		{
			InComponent->SetOwner( nullptr );
			ownedComponents.erase( ownedComponents.begin() + index );
			return;
		}
	}
}

void AActor::ResetOwnedComponents()
{
	for ( uint32 index = 0, count = ( uint32 )ownedComponents.size(); index < count; ++index )
	{
		ownedComponents[ index ]->SetOwner( nullptr );
	}
	ownedComponents.clear();
}