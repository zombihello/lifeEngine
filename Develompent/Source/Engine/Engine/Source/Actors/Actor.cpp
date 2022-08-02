#include "Misc/EngineGlobals.h"
#include "System/World.h"
#include "Actors/Actor.h"
#include "Components/ActorComponent.h"

IMPLEMENT_CLASS( AActor )

#if WITH_EDITOR
CActorVar::CActorVar()
	: type( AVT_Unknown )
	, value( nullptr )
{}

CActorVar::CActorVar( const CActorVar& InCopy )
{
	*this = InCopy;
}

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

AActor::AActor()
	: bIsStatic( false )
	, bNeedReinitCollision( false )
	, bActorIsBeingDestroyed( false )
	, bBeginPlay( false )
{}

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

	bBeginPlay = true;
}

void AActor::EndPlay()
{
	for ( uint32 index = 0, count = ( uint32 )ownedComponents.size(); index < count; ++index )
	{
		ownedComponents[ index ]->EndPlay();
	}

	bBeginPlay = false;
}

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

void AActor::Serialize( class CArchive& InArchive )
{
	Super::Serialize( InArchive );
	InArchive << bIsStatic;

	for ( uint32 index = 0, count = ( uint32 )ownedComponents.size(); index < count; ++index )
	{
		ownedComponents[ index ]->Serialize( InArchive );
	}
}

void AActor::Spawned()
{
	for ( uint32 index = 0, count = ( uint32 )ownedComponents.size(); index < count; ++index )
	{
		ownedComponents[ index ]->Spawned();
	}
}

bool AActor::Destroy()
{
	if ( !bActorIsBeingDestroyed )
	{
		GWorld->DestroyActor( this );
		bActorIsBeingDestroyed = true;
	}
	return bActorIsBeingDestroyed;
}

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

void AActor::InitPhysics()
{
	if ( collisionComponent )
	{
		collisionComponent->InitPrimitivePhysics();
	}
}

void AActor::TermPhysics()
{
	if ( collisionComponent )
	{
		collisionComponent->TermPrimitivePhysics();
	}
}

void AActor::SyncPhysics()
{
	if ( collisionComponent )
	{
		collisionComponent->SyncComponentToPhysics();
	}
}

#if WITH_EDITOR
bool AActor::InitProperties( const std::vector<CActorVar>& InActorVars, class CCookPackagesCommandlet* InCooker )
{
	return true;
}

std::wstring AActor::GetActorIcon() const
{
	return TEXT( "Engine/Editor/Icons/Actor_Default.png" );
}
#endif // WITH_EDITOR

ActorComponentRef_t AActor::CreateComponent( CClass* InClass, const tchar* InName )
{
	check( InClass );

	CObject*				newObject = InClass->CreateObject();
	check( newObject->IsA< CActorComponent >() );

	CActorComponent*		component = newObject->Cast< CActorComponent >();
	check( component );

	bool		bIsASceneComponent		= component->IsA< CSceneComponent >();

	// If created component is a CSceneComponent and RootComponent not setted - set it!
	if ( !rootComponent && bIsASceneComponent )
	{
		rootComponent = component->Cast< CSceneComponent >();
	}
	else if ( rootComponent && bIsASceneComponent )
	{
		( ( CSceneComponent* )component )->SetupAttachment( rootComponent );
	}

	component->SetName( InName );
	AddOwnedComponent( component );
	return component;
}

void AActor::AddOwnedComponent( class CActorComponent* InComponent )
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

void AActor::RemoveOwnedComponent( class CActorComponent* InComponent )
{
	check( InComponent && InComponent->GetOwner() == this );
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
				check( false && "Need implement change root component" );
			}

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
		if ( collisionComponent == ownedComponents[ index ] )
		{
			collisionComponent->TermPrimitivePhysics();
		}
		ownedComponents[ index ]->SetOwner( nullptr );
	}

	ownedComponents.clear();
	collisionComponent	= nullptr;
	rootComponent		= nullptr;
}