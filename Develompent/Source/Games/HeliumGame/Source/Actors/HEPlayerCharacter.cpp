#include "Actors/HEPlayerCharacter.h"
#include "System/CameraManager.h"
#include "System/Package.h"
#include "Misc/EngineGlobals.h"

IMPLEMENT_CLASS( AHEPlayerCharacter )

AHEPlayerCharacter::AHEPlayerCharacter()
{
	// Create camera component
	cameraComponent = CreateComponent< LCameraComponent >( TEXT( "CameraComponent0" ) );
	cameraComponent->SetProjectionMode( CPM_Orthographic );
	cameraComponent->SetIgnoreRotateByMouse( true );
	cameraComponent->SetNearClipPlane( -100.f );
	cameraComponent->SetFarClipPlane( 100.f );

	// Create sprite component
	spriteComponent = CreateComponent< LSpriteComponent >( TEXT( "Player" ) );
	spriteComponent->AddRelativeRotate( FRotator( 0.f, 180.f, 0.f ) );
	spriteComponent->SetSpriteSize( FVector2D( 32.f, 32.f ) );
	spriteComponent->SetType( ST_Static );
}

AHEPlayerCharacter::~AHEPlayerCharacter()
{
	// Deactive player camera if need
	if ( cameraComponent->IsActive() )
	{
		GCameraManager->SetActiveCamera( nullptr );
	}
}

void AHEPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Set player camera to active
	GCameraManager->SetActiveCamera( cameraComponent );
}