#include "Actors/ELPlayerCharacter.h"
#include "System/World.h"
#include "System/CameraManager.h"
#include "System/Package.h"
#include "Components/BoxComponent.h"
#include "Misc/EngineGlobals.h"
#include "Logger/LoggerMacros.h"

IMPLEMENT_CLASS( AELPlayerCharacter )

AELPlayerCharacter::AELPlayerCharacter()
{
	// Create camera component
	cameraComponent = CreateComponent<CCameraComponent>( TEXT( "CameraComponent0" ) );
	cameraComponent->SetProjectionMode( CPM_Perspective );
	cameraComponent->SetAutoViewData( true );
	cameraComponent->SetFieldOfView( 90.f );
}

AELPlayerCharacter::~AELPlayerCharacter()
{
	// Deactive player camera if need
	if ( cameraComponent->IsActive() )
	{
		GCameraManager->SetActiveCamera( nullptr );
	}
}

void AELPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Set player camera to active
	GCameraManager->SetActiveCamera( cameraComponent );
}