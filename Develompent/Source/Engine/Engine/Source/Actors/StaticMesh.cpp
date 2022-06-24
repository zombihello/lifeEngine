#include "Misc/EngineGlobals.h"
#include "Logger/LoggerMacros.h"
#include "System/Config.h"
#include "System/World.h"
#include "Actors/StaticMesh.h"
#include "Misc/CoreGlobals.h"
#include "System/Package.h"

IMPLEMENT_CLASS( AStaticMesh )

AStaticMesh::AStaticMesh()
{
	staticMeshComponent = CreateComponent< CStaticMeshComponent >( TEXT( "StaticMeshComponent0" ) );
}

AStaticMesh::~AStaticMesh()
{}

void AStaticMesh::BeginPlay()
{
	Super::BeginPlay();
}

#if WITH_EDITOR
ActorRef_t AStaticMesh::SpawnActorAsset( const TSharedPtr<CAsset>& InAsset, const Vector& InLocation, const CRotator& InRotation )
{
	// If asset is not valid or not static mesh asset, we do nothing
	if ( !InAsset || InAsset->GetType() != AT_StaticMesh )
	{
		return nullptr;
	}

	// Spawn new actor
	TRefCountPtr<AStaticMesh>		staticMeshActor = GWorld->SpawnActor<AStaticMesh>( InLocation, InRotation );
	if ( !staticMeshActor )
	{
		return nullptr;
	}

	// Init asset in new static mesh actor
	staticMeshActor->SetStaticMesh( InAsset->GetAssetHandle() );
	return staticMeshActor;
}

std::wstring AStaticMesh::GetActorIcon() const
{
	return TEXT( "Engine/Editor/Icons/Asset_StaticMesh.png" );
}
#endif // WITH_EDITOR