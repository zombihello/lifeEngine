#include "Actors/HEPlayerCharacter.h"
#include "System/World.h"
#include "System/CameraManager.h"
#include "System/Package.h"
#include "Components/BoxComponent.h"
#include "Misc/EngineGlobals.h"
#include "Logger/LoggerMacros.h"

IMPLEMENT_CLASS( AHEPlayerCharacter )

AHEPlayerCharacter::AHEPlayerCharacter()
{
	// Create box component
	LBoxComponent* boxComponent = CreateComponent< LBoxComponent >( TEXT( "BoxComponent0" ) );
	boxComponent->SetSize( FVector( 32.f, 32.f, 1.f ) );
	boxComponent->SetCollisionProfile( TEXT( "Character" ) );
	{
		FPhysicsBodyInstance*	bodyInstance = &boxComponent->GetBodyInstance();
		bodyInstance->SetLockFlags( BLF_LockMoveZ | BLF_LockRotateX | BLF_LockRotateY | BLF_LockRotateZ );
		bodyInstance->SetMass( 10.f );
		bodyInstance->SetEnableGravity( true );
		bodyInstance->SetSimulatePhysics( true );
	}
	collisionComponent = boxComponent;

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

void AHEPlayerCharacter::Tick( float InDeltaTime )
{
	Super::Tick( InDeltaTime );
	
	// For test
	FVector		startRay = GetActorLocation();
	FVector		endRay = GetActorLocation() + GetActorUpVector() * -10.f;
	FCollisionQueryParams		queryParams;
	queryParams.bReturnPhysicalMaterial = true;
	FHitResult	hitResult;
	bool		result = GWorld->LineTraceSingleByChannel( hitResult, startRay, endRay, CC_WorldStatic, queryParams );
	if ( result )
	{
	}
}