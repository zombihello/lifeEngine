#include "Misc/EngineGlobals.h"
#include "System/World.h"
#include "Actors/Actor.h"
#include "Components/ActorComponent.h"

IMPLEMENT_CLASS( AActor )

#if WITH_EDITOR
/*
==================
CActorVar::CActorVar
==================
*/
CActorVar::CActorVar()
	: type( AVT_Unknown )
	, value( nullptr )
{}

/*
==================
CActorVar::CActorVar
==================
*/
CActorVar::CActorVar( const CActorVar& InCopy )
{
	*this = InCopy;
}

/*
==================
CActorVar::Clear
==================
*/
void CActorVar::Clear()
{
	if ( !value )
	{
		return;
	}

	switch ( type )
	{
	case AVT_Int:		delete static_cast< int32* >( value );						break;
	case AVT_Float:		delete static_cast< float* >( value );						break;
	case AVT_Bool:		delete static_cast< bool* >( value );						break;
	case AVT_Vector2D:	delete static_cast< Vector2D* >( value );					break;
	case AVT_Vector3D:	delete static_cast< Vector* >( value );					break;
	case AVT_Vector4D:	delete static_cast< Vector4D* >( value );					break;
	case AVT_RectInt:	delete static_cast< RectInt32_t* >( value );					break;
	case AVT_RectFloat:	delete static_cast< RectFloat_t* >( value );					break;
	case AVT_Color:		delete static_cast< CColor* >( value );						break;
	case AVT_String:	delete static_cast< std::wstring* >( value );				break;
	case AVT_Material:	delete static_cast< TAssetHandle<CMaterial>* >( value );	break;
	}

	value = nullptr;
	type = AVT_Unknown;
}
#endif // WITH_EDITOR

/*
==================
AActor::AActor
==================
*/
AActor::AActor()
	: bIsStatic( false )
	, bNeedReinitCollision( false )
	, bActorIsBeingDestroyed( false )
	, bBeginPlay( false )
	, bVisibility( true )

#if WITH_EDITOR
	, bSelected( false )
#endif // WITH_EDITOR

	, worldPrivate( nullptr )
{}

/*
==================
AActor::BeginDestroy
==================
*/
void AActor::BeginDestroy()
{
	Super::BeginDestroy();
	ResetOwnedComponents();
}

/*
==================
AActor::StaticInitializeClass
==================
*/
void AActor::StaticInitializeClass()
{
	// Native properties
	new( staticClass, TEXT( "bVisibility" ), OBJECT_Public )	CBoolProperty( CPP_PROPERTY( ThisClass, bVisibility ), TEXT( "Drawing" ), TEXT( "Is actor visibility" ), CPF_Edit );
	new( staticClass, TEXT( "bIsStatic" ), OBJECT_Public )		CBoolProperty( CPP_PROPERTY( ThisClass, bIsStatic ), TEXT( "Actor" ), TEXT( "Is static actor" ), CPF_Edit );
	new( staticClass, TEXT( "Root Component" ) )				CObjectProperty( CPP_PROPERTY( ThisClass, rootComponent ), NAME_None, TEXT( "" ), CPF_None, CSceneComponent::StaticClass() );
	new( staticClass, TEXT( "Collision Component" ) )			CObjectProperty( CPP_PROPERTY( ThisClass, collisionComponent ), NAME_None, TEXT( "" ), CPF_None, CPrimitiveComponent::StaticClass() );

	CArrayProperty*		ownedComponentsProperty = new( staticClass, TEXT( "Owned Components" ) )	CArrayProperty( CPP_PROPERTY( ThisClass, ownedComponents ), NAME_None, TEXT( "" ), CPF_None );
	new( ownedComponentsProperty, NAME_None ) CObjectProperty( CppProperty, 0, NAME_None, TEXT( "" ), CPF_None, CActorComponent::StaticClass() );
}

/*
==================
AActor::BeginPlay
==================
*/
void AActor::BeginPlay()
{
	for ( uint32 index = 0, count = ( uint32 )ownedComponents.size(); index < count; ++index )
	{
		ownedComponents[ index ]->BeginPlay();
	}

	bBeginPlay = true;
}

/*
==================
AActor::EndPlay
==================
*/
void AActor::EndPlay()
{
	for ( uint32 index = 0, count = ( uint32 )ownedComponents.size(); index < count; ++index )
	{
		ownedComponents[ index ]->EndPlay();
	}

	bBeginPlay = false;
}

/*
==================
AActor::Tick
==================
*/
void AActor::Tick( float InDeltaTime )
{
	for ( uint32 index = 0, count = ( uint32 )ownedComponents.size(); index < count; ++index )
	{
		ownedComponents[ index ]->TickComponent( InDeltaTime );
	}

	// Reinit collision if need
	if ( bNeedReinitCollision )
	{
		TermPhysics();
		InitPhysics();
		bNeedReinitCollision = false;
	}
}

/*
==================
AActor::Spawned
==================
*/
void AActor::Spawned()
{
	for ( uint32 index = 0, count = ( uint32 )ownedComponents.size(); index < count; ++index )
	{
		ownedComponents[ index ]->Spawned();
	}
}

/*
==================
AActor::Destroy
==================
*/
bool AActor::Destroy()
{
	// It's already pending kill or in DestroyActor(), no need to beat the corpse
	if ( !IsPendingKillPending() )
	{
		CWorld*		world = GetWorld();
		if ( world )
		{
			world->DestroyActor( this );
		}
		else
		{
			Warnf( TEXT( "Destroying %s, which doesn't have a valid world pointer" ), GetName().c_str() );
		}
	}

	return IsPendingKillPending();
}

/*
==================
AActor::Destroyed
==================
*/
void AActor::Destroyed()
{
	// If for this actor allready started play, call event of end play
	if ( bBeginPlay )
	{
		EndPlay();
	}

	// Call destroy for each component
	for ( uint32 index = 0, count = ownedComponents.size(); index < count; ++index )
	{
		ownedComponents[ index ]->Destroyed();
	}

	// Terminate physics
	TermPhysics();

	// Broadcast event of destroyed this actor
	onActorDestroyed.Broadcast( this );
}

/*
==================
AActor::InitPhysics
==================
*/
void AActor::InitPhysics()
{
	if ( collisionComponent )
	{
		collisionComponent->InitPrimitivePhysics();
	}
}

/*
==================
AActor::TermPhysics
==================
*/
void AActor::TermPhysics()
{
	if ( collisionComponent )
	{
		collisionComponent->TermPrimitivePhysics();
	}
}

/*
==================
AActor::SyncPhysics
==================
*/
void AActor::SyncPhysics()
{
	if ( collisionComponent )
	{
		collisionComponent->SyncComponentToPhysics();
	}
}

#if WITH_EDITOR
/*
==================
AActor::InitProperties
==================
*/
bool AActor::InitProperties( const std::vector<CActorVar>& InActorVars, class CCookPackagesCommandlet* InCooker )
{
	return true;
}

/*
==================
AActor::GetActorIcon
==================
*/
std::wstring AActor::GetActorIcon() const
{
	return TEXT( "Engine/Editor/Icons/Actor_Default.png" );
}
#endif // WITH_EDITOR

/*
==================
AActor::CreateComponent
==================
*/
CActorComponent* AActor::CreateComponent( CClass* InClass, const CName& InName, bool InEditorOnly /*= false*/ )
{
	Assert( InClass );

	CObject*				newObject = InClass->CreateObject( this, InName );
	Assert( IsA<CActorComponent>( newObject ) );

	CActorComponent*		component = Cast<CActorComponent>( newObject );
	bool					bIsASceneComponent	= IsA<CSceneComponent>( component );
	Assert( component );

	// If created component is a CSceneComponent and RootComponent not setted - set it!
	if ( !rootComponent && bIsASceneComponent )
	{
		rootComponent = Cast<CSceneComponent>( component );
	}
	else if ( rootComponent && bIsASceneComponent )
	{
		( ( CSceneComponent* )component )->SetupAttachment( rootComponent );
	}

#if WITH_EDITOR
	component->SetEditorOnly( InEditorOnly );
	if ( !g_IsEditor && InEditorOnly && IsA<CPrimitiveComponent>( component ) )
	{
		( ( CPrimitiveComponent* )component )->SetVisibility( false );
	}
#endif // WITH_EDITOR

	ownedComponents.push_back( component );
	return component;
}

/*
==================
AActor::AddOwnedComponent
==================
*/
void AActor::AddOwnedComponent( class CActorComponent* InComponent )
{
	Assert( InComponent && InComponent->GetOwner() != this );

	// If already owned exist in component - remove component from owner
	{
		AActor*		owner = InComponent->GetOwner();
		if ( owner )
		{
			owner->RemoveOwnedComponent( InComponent );
		}
	}

	// Add component
	InComponent->Rename( nullptr, this );
	ownedComponents.push_back( InComponent );
}

/*
==================
AActor::RemoveOwnedComponent
==================
*/
void AActor::RemoveOwnedComponent( class CActorComponent* InComponent )
{
	Assert( InComponent && InComponent->GetOwner() == this );
	for ( uint32 index = 0, count = ( uint32 )ownedComponents.size(); index < count; ++index )
	{
		CActorComponent*		component = ownedComponents[ index ];
		if ( component == InComponent )
		{
			if ( component == collisionComponent )
			{
				collisionComponent->TermPrimitivePhysics();
				collisionComponent = nullptr;
			}
			else if ( component == rootComponent )
			{
				Assert( false && "Need implement change root component" );
			}

			InComponent->MarkPendingKill();
			ownedComponents.erase( ownedComponents.begin() + index );
			return;
		}
	}
}

/*
==================
AActor::ResetOwnedComponents
==================
*/
void AActor::ResetOwnedComponents()
{
	for ( uint32 index = 0, count = ( uint32 )ownedComponents.size(); index < count; ++index )
	{
		if ( collisionComponent == ownedComponents[ index ] )
		{
			collisionComponent->TermPrimitivePhysics();
		}
		ownedComponents[index]->MarkPendingKill();
	}

	ownedComponents.clear();
	collisionComponent	= nullptr;
	rootComponent		= nullptr;
}

/*
==================
AActor::GetWorld_Uncached
==================
*/
CWorld* AActor::GetWorld_Uncached() const
{
	return ( CWorld* )GetTypedOuter( CWorld::StaticClass() );
}