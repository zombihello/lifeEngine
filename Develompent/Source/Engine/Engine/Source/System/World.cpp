#include "Actors/Actor.h"
#include "System/World.h"
#include "Logger/LoggerMacros.h"

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

void FWorld::Serialize( FArchive& InArchive )
{
	if ( InArchive.IsSaving() )
	{
		InArchive << ( uint32 )actors.size();
		for ( uint32 index = 0, count = ( uint32 )actors.size(); index < count; ++index )
		{
			AActor*			actor = actors[ index ];
			InArchive << actor->GetClass()->GetName();
			actor->Serialize( InArchive );
		}
	}
	else
	{
		// Clear world
		CleanupWorld();

		uint32		countActors = 0;
		InArchive << countActors;

		actors.resize( countActors );
		for ( uint32 index = 0; index < countActors; ++index )
		{
			// Serialize class name
			uint32				classNameSize = 0;
			std::wstring		className;
			InArchive << className;

			// Spawn actor, serialize and add to array
			AActor*			actor = SpawnActor( LClass::StaticFindClass( className.c_str() ), FMath::vectorZero, FMath::rotatorZero );
			actor->Serialize( InArchive );
			actors[ index ] = actor;
		}
	}
}

void FWorld::CleanupWorld()
{
	actors.clear();
	isBeginPlay = false;
}

AActorRef FWorld::SpawnActor( class LClass* InClass, const FVector& InLocation, const FRotator& InRotation /* = FMath::rotatorZero */ )
{
	check( InClass );

	AActor*		actor = InClass->CreateObject< AActor >();
	check( actor );

	// Set default actor name and location with rotation
	actor->SetName( InClass->GetName().c_str() );
	actor->SetActorLocation( InLocation );
	actor->SetActorRotation( InRotation );

	// If gameplay is started - call BeginPlay in spawned actor
	if ( isBeginPlay )
	{
		actor->BeginPlay();
	}

	actors.push_back( actor );
	return actor;
}