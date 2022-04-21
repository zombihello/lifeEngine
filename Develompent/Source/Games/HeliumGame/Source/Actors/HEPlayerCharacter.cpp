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
	spriteComponent->SetMaterial( ( FMaterialRef )GPackageManager->FindAsset( TEXT( "Characters_Player:Player_Mat" ), AT_Material ) );

	// Create audio component
	audioComponent = CreateComponent< LAudioComponent >( TEXT( "AudioComponent0" ) );

	// Load audio bank for walking and jumping
	walkAudioBanks[ ST_Default ]	= GPackageManager->FindAsset( TEXT( "Characters_Player_Audio:Footsteps_Concrete_Walk_01" ), AT_AudioBank );
	walkAudioBanks[ ST_Surface1 ]	= GPackageManager->FindAsset( TEXT( "Characters_Player_Audio:Footstep_Grass_Walk_01" ), AT_AudioBank );
	walkAudioBanks[ ST_Surface2 ]	= GPackageManager->FindAsset( TEXT( "Characters_Player_Audio:Footsteps_Concrete_Walk_01" ), AT_AudioBank );

	jumpAudioBanks[ ST_Default ]	= GPackageManager->FindAsset( TEXT( "Characters_Player_Audio:Footsteps_Concrete_Land_01" ), AT_AudioBank );
	jumpAudioBanks[ ST_Surface1 ]	= GPackageManager->FindAsset( TEXT( "Characters_Player_Audio:Footstep_Grass_Land_01" ), AT_AudioBank );
	jumpAudioBanks[ ST_Surface2 ]	= GPackageManager->FindAsset( TEXT( "Characters_Player_Audio:Footsteps_Concrete_Land_01" ), AT_AudioBank );
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

	// If we walking - play step sounds
	if ( collisionComponent )
	{
		FVector		velocity = collisionComponent->GetBodyInstance().GetLinearVelocity();
		velocity.y	= 0.f;
		if ( audioComponent->GetStatus() != ASS_Playing && FMath::LengthVector( velocity ) > 0.f )
		{
			FVector		startRay	= GetActorLocation();
			FVector		endRay		= startRay + GetActorUpVector() * -1.f;
			FHitResult	hitResult;

			FCollisionQueryParams				queryParams;
			queryParams.bReturnPhysicalMaterial = true;
			bool		bResult = GWorld->LineTraceSingleByChannel( hitResult, startRay, endRay, CC_WorldStatic, queryParams );
			if ( bResult )
			{
				FAudioBankRef		audioBank = walkAudioBanks[ hitResult.physMaterial->GetSurfaceType() ];
				if ( audioBank )
				{
					audioComponent->SetAudioBank( audioBank );
					audioComponent->Play();
				}
			}
		}
	}
}

void AHEPlayerCharacter::Landed()
{
	Super::Landed();

	FCollisionQueryParams		queryParams;
	queryParams.bReturnPhysicalMaterial = true;

	FVector			startRay	= GetActorLocation();
	FVector			endRay		= startRay + GetActorUpVector() * -1.f;
	FHitResult		hitResult;
	bool			bResult		= GWorld->LineTraceSingleByChannel( hitResult, startRay, endRay, CC_WorldStatic, queryParams );
	if ( bResult )
	{
		FAudioBankRef		audioBank = jumpAudioBanks[ hitResult.physMaterial->GetSurfaceType() ];
		if ( audioBank )
		{
			audioComponent->SetAudioBank( audioBank );
			audioComponent->Play();
		}
	}
}

void AHEPlayerCharacter::Walk( const FVector& InWorldDirection, float InScale )
{
	Super::Walk( InWorldDirection, InScale );
	spriteComponent->SetFlipHorizontal( InScale < 0.f );
}