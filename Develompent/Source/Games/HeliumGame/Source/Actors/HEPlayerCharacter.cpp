#include "Actors/HEPlayerCharacter.h"
#include "System/CameraManager.h"
#include "System/Package.h"
#include "Components/BoxComponent.h"
#include "Misc/EngineGlobals.h"

IMPLEMENT_CLASS( AHEPlayerCharacter )

AHEPlayerCharacter::AHEPlayerCharacter()
{
	// Create camera component
	cameraComponent = CreateComponent< LCameraComponent >( TEXT( "CameraComponent0" ) );
	cameraComponent->SetProjectionMode( CPM_Orthographic );
	cameraComponent->SetAutoViewData( true );
	cameraComponent->SetNearClipPlane( -100.f );
	cameraComponent->SetFarClipPlane( 100.f );
	cameraComponent->AddRelativeLocation( FVector( 0, 0, -10.f ) );

	// Create sprite component
	spriteComponent = CreateComponent< LSpriteComponent >( TEXT( "SpriteComponent0" ) );
	spriteComponent->SetSpriteSize( FVector2D( 32.f, 32.f ) );
	spriteComponent->SetType( ST_Static );

	// Create box component
	collisionComponent = CreateComponent< LBoxComponent >( TEXT( "BoxComponent0" ) );
	collisionComponent->SetBodyLockFlags( BLF_LockMoveZ | BLF_LockRotateX | BLF_LockRotateY | BLF_LockRotateZ );
	collisionComponent->SetBodyMass( 1000.f );
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