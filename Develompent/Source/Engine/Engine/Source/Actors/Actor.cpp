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
{}

/*
==================
AActor::~AActor
==================
*/
AActor::~AActor()
{
	ResetOwnedComponents();
}

/*
==================
AActor::StaticInitializeClass
==================
*/
void AActor::StaticInitializeClass()
{
	new( staticClass, TEXT( "bVisibility" ) )	CBoolProperty( TEXT( "Drawing" ), TEXT( "Is actor visibility" ), CPP_PROPERTY( bVisibility ), 0 );
	new( staticClass, TEXT( "bIsStatic" ) )		CBoolProperty( TEXT( "Actor" ), TEXT( "Is static actor" ), CPP_PROPERTY( bIsStatic ), 0 );
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
AActor::Serialize
==================
*/
void AActor::Serialize( class CArchive& InArchive )
{
	Super::Serialize( InArchive );
	InArchive << bIsStatic;
	InArchive << bVisibility;

	Assert( InArchive.Ver() >= VER_FixedSerializeComponents );
	uint32		startToNumComponents = InArchive.Tell();
	uint32		numComponents = 0;
	InArchive << numComponents;

	for ( uint32 index = 0, count = InArchive.IsSaving() ? ownedComponents.size() : numComponents; index < count; ++index )
	{
		uint32					componentSize = 0;
		ActorComponentRef_t		ownedComponent;
		std::wstring			name;
		std::wstring			className;

		if ( InArchive.IsSaving() )
		{
			ownedComponent = ownedComponents[index];
			name = ownedComponent->GetName();
			className = ownedComponent->GetClass()->GetName();

			// Skip editor only component if we cooking packages
#if WITH_EDITOR
			if ( ownedComponent->IsEditorOnly() && g_IsCooker )
			{
				continue;
			}
#endif // WITH_EDITOR
		}

		InArchive << name;
		InArchive << className;
		InArchive << componentSize;

		if ( InArchive.IsLoading() )
		{
			CClass*		cclass = CClass::StaticFindClass( className.c_str() );
			for ( uint32 indexComponent = 0, countComponents = ownedComponents.size(); indexComponent < countComponents; ++indexComponent )
			{
				ActorComponentRef_t		component = ownedComponents[indexComponent];
				if ( name == component->GetName() && cclass == component->GetClass() )
				{
					ownedComponent = component;
					break;
				}
			}

			if ( !ownedComponent )
			{
				Warnf( TEXT( "In actor %s didn't find component %s (%s) for serialize\n" ), GetName(), name.c_str(), className.c_str() );
				InArchive.Seek( InArchive.Tell() + componentSize );
				continue;
			}
		}

		uint32	startOffset = InArchive.Tell();
		ownedComponent->Serialize( InArchive );

		if ( InArchive.IsSaving() )
		{
			uint32	currentOffset = InArchive.Tell();
			componentSize = currentOffset - startOffset;
			++numComponents;

			InArchive.Seek( startOffset - sizeof( componentSize ) );
			InArchive << componentSize;
			InArchive.Seek( currentOffset );
		}
		else
		{
			Assert( InArchive.Tell() - startOffset == componentSize );
		}
	}

	if ( InArchive.IsSaving() )
	{
		uint32	currentOffset = InArchive.Tell();
		InArchive.Seek( startToNumComponents );
		InArchive << numComponents;
		InArchive.Seek( currentOffset );
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
	if ( !bActorIsBeingDestroyed )
	{
		g_World->DestroyActor( this );
		bActorIsBeingDestroyed = true;
	}
	return bActorIsBeingDestroyed;
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
ActorComponentRef_t AActor::CreateComponent( CClass* InClass, const CName& InName, bool InEditorOnly /*= false*/ )
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

	component->SetCName( InName );
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
	InComponent->SetOuter( this );
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

			InComponent->SetOuter( nullptr );
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
		ownedComponents[index]->SetOuter( nullptr );
	}

	ownedComponents.clear();
	collisionComponent	= nullptr;
	rootComponent		= nullptr;
}