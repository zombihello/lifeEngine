#include "Actors/Actor.h"
#include "System/World.h"

FWorld::FWorld() :
	isBeginPlay( false )
{}

FWorld::~FWorld()
{
	CleanupWorld();
}

void FWorld::BeginPlay()
{
	for ( uint32 index = 0; index < ( uint32 )actors.size(); ++index )
	{
		actors[ index ]->BeginPlay();
	}

	isBeginPlay = true;
}

void FWorld::Tick( float InDeltaTime )
{
	for ( uint32 index = 0, count = ( uint32 )actors.size(); index < count; ++index )
	{
		actors[ index ]->Tick( InDeltaTime );
	}
}

void FWorld::CleanupWorld()
{
	actors.clear();
}

AActorRef FWorld::SpawnActor( class LClass* InClass )
{
	check( InClass );

	AActor*		actor = InClass->CreateObject< AActor >();
	check( actor );

	// Set default actor name
	actor->SetName( InClass->GetName().c_str() );

	// If gameplay is started - call BeginPlay in spawned actor
	if ( isBeginPlay )
	{
		actor->BeginPlay();
	}

	actors.push_back( actor );
	return actor;
}