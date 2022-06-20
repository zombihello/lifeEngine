#include "Misc/CoreGlobals.h"
#include "Misc/EngineGlobals.h"
#include "Misc/PhysicsGlobals.h"
#include "System/CameraManager.h"
#include "System/Package.h"
#include "PhysicsInterface.h"
#include "Actors/Actor.h"
#include "System/World.h"
#include "Logger/LoggerMacros.h"
#include "Render/Scene.h"

#if WITH_EDITOR
#include "WorldEd.h"
#endif // WITH_EDITOR

CWorld::CWorld() 
	: isBeginPlay( false )
	, scene( new CScene() )
{}

CWorld::~CWorld()
{
	CleanupWorld();
	delete scene;
}

void CWorld::BeginPlay()
{
	// Init all actors
	for ( uint32 index = 0; index < ( uint32 )actors.size(); ++index )
	{
		actors[ index ]->BeginPlay();
	}

	// Init all physics in actors
	for ( uint32 index = 0; index < actors.size(); ++index )
	{
		actors[ index ]->InitPhysics();
	}

	GCameraManager->BeginPlay();
	isBeginPlay = true;
}

void CWorld::Tick( float InDeltaTime )
{
	for ( uint32 index = 0, count = ( uint32 )actors.size(); index < count; ++index )
	{
		AActor*		actor = actors[ index ];
		actor->Tick( InDeltaTime );
		actor->SyncPhysics();
	}
}

void CWorld::Serialize( CArchive& InArchive )
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

		for ( uint32 index = 0; index < countActors; ++index )
		{
			// Serialize class name
			uint32				classNameSize = 0;
			std::wstring		className;
			InArchive << className;

			// Spawn actor, serialize and add to array
			AActor*			actor = SpawnActor( CClass::StaticFindClass( className.c_str() ), SMath::vectorZero, SMath::rotatorZero );
			actor->Serialize( InArchive );
		}

#if WITH_EDITOR
		SEditorDelegates::onWorldLoaded.Broadcast();
#endif // WITH_EDITOR
	}
}

void CWorld::CleanupWorld()
{
	GPhysicsScene.RemoveAllBodies();
	scene->Clear();
	actors.clear();
	isBeginPlay = false;
}

ActorRef_t CWorld::SpawnActor( class CClass* InClass, const Vector& InLocation, const CRotator& InRotation /* = SMath::rotatorZero */ )
{
	check( InClass );

	AActor*		actor = InClass->CreateObject< AActor >();
	check( actor );

	// Set default actor name and location with rotation
	actor->SetName( InClass->GetName().c_str() );
    actor->AddActorLocation( InLocation );
    actor->AddActorRotation( InRotation );

	// If gameplay is started - call BeginPlay in spawned actor and init physics
	if ( isBeginPlay )
	{
		actor->BeginPlay();
		actor->InitPhysics();
	}

	actors.push_back( actor );
	return actor;
}
