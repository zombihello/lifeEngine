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
	staticMeshComponent = CreateComponent< LStaticMeshComponent >( TEXT( "StaticMeshComponent0" ) );
}

AStaticMesh::~AStaticMesh()
{}

void AStaticMesh::BeginPlay()
{
	Super::BeginPlay();
	staticMeshComponent->SetStaticMesh( ( FStaticMeshRef )GPackageManager->FindAsset( TEXT( "../../Content/Tiger.lpak" ), appCalcHash( TEXT( "Tiger_SM" ) ) ) );
}