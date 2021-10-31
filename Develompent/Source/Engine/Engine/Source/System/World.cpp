#include "Actors/Actor.h"
#include "System/World.h"

FWorld::FWorld()
{}

FWorld::~FWorld()
{
	CleanupWorld();
}

void FWorld::BeginPlay()
{
	for ( uint32 index = 0, count = ( uint32 )actors.size(); index < count; ++index )
	{
		actors[ index ]->BeginPlay();
	}
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

	actors.push_back( actor );
	return actor;
}