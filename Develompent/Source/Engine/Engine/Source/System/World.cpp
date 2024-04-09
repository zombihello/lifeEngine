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

IMPLEMENT_CLASS( CWorld )

/*
==================
CWorld::CWorld
==================
*/
CWorld::CWorld() 
	: isBeginPlay( false )
	, scene( new CScene() )
{}

/*
==================
CWorld::StaticInitializeClass
==================
*/
void CWorld::StaticInitializeClass()
{
	// Native properties
	CArrayProperty*		actorsArray = new( staticClass, TEXT( "Actors" ) )		CArrayProperty( CPP_PROPERTY( ThisClass, actors ), NAME_None, TEXT( "" ), CPF_None );
	new( actorsArray, NAME_None )												CObjectProperty( CppProperty, 0, NAME_None, TEXT( "" ), CPF_None, AActor::StaticClass() );
}

/*
==================
CWorld::BeginDestroy
==================
*/
void CWorld::BeginDestroy()
{
	Super::BeginDestroy();

	CleanupWorld();
	delete scene;
}

/*
==================
CWorld::BeginPlay
==================
*/
void CWorld::BeginPlay()
{
	// If allready playing started, do nothing
	if ( isBeginPlay )
	{
		return;
	}

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

	g_CameraManager->BeginPlay();
	isBeginPlay = true;
}

/*
==================
CWorld::EndPlay
==================
*/
void CWorld::EndPlay()
{
	// If not playing, do nothing
	if ( !isBeginPlay )
	{
		return;
	}

	// End play and destroy physics for all actors
	for ( uint32 index = 0; index < ( uint32 ) actors.size(); ++index )
	{
		AActor*		actor = actors[ index ];
		actor->EndPlay();
		actor->TermPhysics();
	}

	g_CameraManager->EndPlay();
	isBeginPlay = false;
}

/*
==================
CWorld::Tick
==================
*/
void CWorld::Tick( float InDeltaTime )
{
	// Tick all actors
	for ( uint32 index = 0, count = ( uint32 )actors.size(); index < count; ++index )
	{
		AActor*		actor = actors[ index ];
		actor->Tick( InDeltaTime );
		actor->SyncPhysics();
	}

	// Destroy actors if need
	if ( !actorsToDestroy.empty() )
	{
		for ( uint32 index = 0, count = actorsToDestroy.size(); index < count; ++index )
		{
			// Tell this actor it's about to be destroyed
			actorsToDestroy[index]->Destroyed();
		}
		actorsToDestroy.clear();
	}
}

/*
==================
CWorld::Serialize
==================
*/
void CWorld::Serialize( CArchive& InArchive )
{
	// Clear the world
	if ( InArchive.IsLoading() )
	{
		CleanupWorld();
	}
	
	Super::Serialize( InArchive );
}

/*
==================
CWorld::PostLoad
==================
*/
void CWorld::PostLoad()
{
	Super::PostLoad();
	bool	bDirtyPackage = false;

	// Call event of spawn for all actors
	for ( uint32 index = 0, count = actors.size(); index < count; )
	{
		// Make sure that the actor was loaded
		AActor*		actor = actors[index];
		if ( actor )
		{
			actor->Spawned();
			++index;
		}
		// Otherwise remove the one from array
		else
		{
			actors.erase( actors.begin() + index );
			count			= actors.size();
			bDirtyPackage	= true;
		}
	}

	// Mark the package as dirty if it need
	if ( bDirtyPackage )
	{
		MarkPackageDirty();
	}
}

/*
==================
CWorld::CleanupWorld
==================
*/
void CWorld::CleanupWorld()
{
	// If we playing, end it
	if ( isBeginPlay )
	{
		EndPlay();
	}

	// Wait while render thread is rendering of the frame
	FlushRenderingCommands();

	// Call event of destroyed actor in each object
	for ( uint32 index = 0, count = actors.size(); index < count; ++index )
	{
		actors[ index ]->Destroyed();
	}

	// Broadcast event of destroyed actors
#if WITH_EDITOR
	if ( !actors.empty() )
	{
		EditorDelegates::onActorsDestroyed.Broadcast( actors );
	}
#endif // WITH_EDITOR

	g_PhysicsScene.RemoveAllBodies();
	scene->Clear();
	actors.clear();
	actorsToDestroy.clear();

#if WITH_EDITOR
	selectedActors.clear();
#endif // WITH_EDITOR
}

/*
==================
CWorld::SpawnActor
==================
*/
AActor* CWorld::SpawnActor( class CClass* InClass, const Vector& InLocation, const CRotator& InRotation /* = Math::rotatorZero */, const CName& InName /* = NAME_None */)
{
	Assert( InClass );

	AActor*		actor = InClass->CreateObject<AActor>( this, InName );
	Assert( actor );

	// Set default actor location with rotation
    actor->AddActorLocation( InLocation );
    actor->AddActorRotation( InRotation );

	// Call event of spawn actor
	actor->Spawned();

	// If gameplay is started - call BeginPlay in spawned actor and init physics
	if ( isBeginPlay )
	{
		actor->BeginPlay();
		actor->InitPhysics();
	}

	actors.push_back( actor );
	
	// Broadcast event of spawned actor
#if WITH_EDITOR
	std::vector<AActor*>		spawnedActors = { actor };
	EditorDelegates::onActorsSpawned.Broadcast( spawnedActors );
	MarkPackageDirty();
#endif // WITH_EDITOR

	return actor;
}

/*
==================
CWorld::DestroyActor
==================
*/
void CWorld::DestroyActor( AActor* InActor, bool InIsIgnorePlaying )
{
	Assert( InActor );

	// If already on list to be deleted, pretend the call was successful
	// We don't want recursive calls to trigger destruction notifications multiple times
	if ( InActor->IsPendingKillPending() )
	{
		return;
	}

	// Prevent recursion
	MarkActorIsBeingDestroyed	markActorIsBeingDestroyed( InActor );

	// If world in play, put this actor to actorsToDestroy for remove after tick
	if ( !InIsIgnorePlaying && InActor->IsPlaying() )
	{
		actorsToDestroy.push_back( InActor );
		return;
	}

	// Broadcast event of destroy actor
#if WITH_EDITOR
	// Unselect actor if him is selected
	if ( InActor->IsSelected() )
	{
		UnselectActor( InActor );
	}

	std::vector<AActor*>		destroyedActors = { InActor };
	EditorDelegates::onActorsDestroyed.Broadcast( destroyedActors );
	MarkPackageDirty();
#endif // WITH_EDITOR

	// Tell this actor it's about to be destroyed
	InActor->Destroyed();

	// Remove actor from array of all actors in world
	for ( uint32 index = 0, count = actors.size(); index < count; ++index )
	{
		if ( actors[ index ] == InActor )
		{
			actors.erase( actors.begin() + index );
			break;
		}
	}
}

#if ENABLE_HITPROXY
/*
==================
CWorld::UpdateHitProxiesId
==================
*/
void CWorld::UpdateHitProxiesId()
{
	for ( uint32 index = 0, count = actors.size(); index < count; ++index )
	{
		actors[ index ]->SetHitProxyId( index+1 );
	}
}
#endif // ENABLE_HITPROXY

#if WITH_EDITOR
/*
==================
CWorld::SelectActor
==================
*/
void CWorld::SelectActor( AActor* InActor )
{
	Assert( InActor );
	if ( InActor->IsSelected() )
	{
		return;
	}

	InActor->SetSelected( true );
	selectedActors.push_back( InActor );

	EditorDelegates::onActorsSelected.Broadcast( std::vector<AActor*>{ InActor } );
}

/*
==================
CWorld::UnselectActor
==================
*/
void CWorld::UnselectActor( AActor* InActor )
{
	Assert( InActor );
	if ( !InActor->IsSelected() )
	{
		return;
	}

	InActor->SetSelected( false );
	for ( uint32 index = 0, count = selectedActors.size(); index < count; ++index )
	{
		if ( selectedActors[ index ] == InActor )
		{
			selectedActors.erase( selectedActors.begin() + index );
			break;
		}
	}

	EditorDelegates::onActorsUnselected.Broadcast( std::vector<AActor*>{ InActor } );
}

/*
==================
CWorld::SelectActors
==================
*/
void CWorld::SelectActors( const std::vector<AActor*>& InActors )
{
	bool		bNeedBroadcast = false;
	for ( uint32 index = 0, count = InActors.size(); index < count; ++index )
	{
		AActor* actor = InActors[index];
		if ( !actor->IsSelected() )
		{
			bNeedBroadcast = true;
			actor->SetSelected( true );
			selectedActors.push_back( actor );
		}
	}

	if ( bNeedBroadcast )
	{
		EditorDelegates::onActorsSelected.Broadcast( InActors );
	}
}

/*
==================
CWorld::UnselectActors
==================
*/
void CWorld::UnselectActors( const std::vector<AActor*>& InActors )
{
	std::vector<AActor*>		unselectedActors;
	for ( uint32 index = 0, count = InActors.size(); index < count; ++index )
	{
		AActor* actor = InActors[index];
		if ( actor->IsSelected() )
		{
			actor->SetSelected( false );
			unselectedActors.push_back( actor );

			// Remove actor from selected array
			// FIXME: Maybe need optimize it
			for ( uint32 indexSelected = 0, countSelected = selectedActors.size(); indexSelected < countSelected; ++indexSelected )
			{
				if ( selectedActors[indexSelected] == actor )
				{
					selectedActors.erase( selectedActors.begin() + index );
					break;
				}
			}
		}
	}

	if ( !unselectedActors.empty() )
	{
		EditorDelegates::onActorsUnselected.Broadcast( unselectedActors );
	}
}

/*
==================
CWorld::UnselectAllActors
==================
*/
void CWorld::UnselectAllActors()
{
	for ( uint32 index = 0, count = selectedActors.size(); index < count; ++index )
	{
		selectedActors[ index ]->SetSelected( false );
	}

	std::vector<AActor*>		unselectedActors;
	std::swap( selectedActors, unselectedActors );
	EditorDelegates::onActorsUnselected.Broadcast( unselectedActors );
}
#endif // WITH_EDITOR