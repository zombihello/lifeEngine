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
	// Create box component
	CBoxComponent* boxComponent = CreateComponent< CBoxComponent >( TEXT( "BoxComponent0" ) );
	boxComponent->SetSize( Vector( 32.f, 32.f, 1.f ) );
	boxComponent->SetCollisionProfile( TEXT( "Character" ) );
	{
		CPhysicsBodyInstance*	bodyInstance = &boxComponent->GetBodyInstance();
		bodyInstance->SetLockFlags( BLF_LockMoveZ | BLF_LockRotateX | BLF_LockRotateY | BLF_LockRotateZ );
		bodyInstance->SetMass( 10.f );
		bodyInstance->SetEnableGravity( false );
		bodyInstance->SetSimulatePhysics( true );
	}
	collisionComponent = boxComponent;

	// Create camera component
	cameraComponent = CreateComponent< CCameraComponent >( TEXT( "CameraComponent0" ) );
	cameraComponent->SetProjectionMode( CPM_Perspective );
	cameraComponent->SetAutoViewData( true );
	cameraComponent->SetFieldOfView( 90.f );
	cameraComponent->AddRelativeLocation( Vector( 0, 0, -10.f ) );

	// Create audio component
	audioComponent = CreateComponent< CAudioComponent >( TEXT( "AudioComponent0" ) );

	// Load audio bank for walking and jumping
	walkAudioBanks[ ST_Default ]	= GPackageManager->FindAsset( TEXT( "AudioBank'Characters_Player_Audio:Footsteps_Concrete_Walk_01" ), AT_AudioBank );
	walkAudioBanks[ ST_Surface1 ]	= GPackageManager->FindAsset( TEXT( "AudioBank'Characters_Player_Audio:Footstep_Grass_Walk_01" ), AT_AudioBank );
	walkAudioBanks[ ST_Surface2 ]	= GPackageManager->FindAsset( TEXT( "AudioBank'Characters_Player_Audio:Footsteps_Concrete_Walk_01" ), AT_AudioBank );

	jumpAudioBanks[ ST_Default ]	= GPackageManager->FindAsset( TEXT( "AudioBank'Characters_Player_Audio:Footsteps_Concrete_Land_01" ), AT_AudioBank );
	jumpAudioBanks[ ST_Surface1 ]	= GPackageManager->FindAsset( TEXT( "AudioBank'Characters_Player_Audio:Footstep_Grass_Land_01" ), AT_AudioBank );
	jumpAudioBanks[ ST_Surface2 ]	= GPackageManager->FindAsset( TEXT( "AudioBank'Characters_Player_Audio:Footsteps_Concrete_Land_01" ), AT_AudioBank );
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

void AELPlayerCharacter::Tick( float InDeltaTime )
{
	Super::Tick( InDeltaTime );

	// If we walking - play step sounds
	if ( collisionComponent )
	{
		Vector		velocity = collisionComponent->GetBodyInstance().GetLinearVelocity();
		velocity.y	= 0.f;
		if ( audioComponent->GetStatus() != ASS_Playing && SMath::LengthVector( velocity ) > 0.f )
		{
			Vector		startRay	= GetActorLocation();
			Vector		endRay		= startRay + GetActorUpVector() * -1.f;
			SHitResult	hitResult;

			SCollisionQueryParams				queryParams;
			queryParams.bReturnPhysicalMaterial = true;
			bool		bResult = GWorld->LineTraceSingleByChannel( hitResult, startRay, endRay, CC_WorldStatic, queryParams );
			if ( bResult )
			{
				TAssetHandle<CAudioBank>		audioBank = walkAudioBanks[ hitResult.physMaterial->GetSurfaceType() ];
				if ( audioBank.IsAssetValid() )
				{
					audioComponent->SetAudioBank( audioBank );
					audioComponent->Play();
				}
			}
		}
	}
}

void AELPlayerCharacter::Landed()
{
	Super::Landed();

	SCollisionQueryParams		queryParams;
	queryParams.bReturnPhysicalMaterial = true;

	Vector			startRay	= GetActorLocation();
	Vector			endRay		= startRay + GetActorUpVector() * -1.f;
	SHitResult		hitResult;
	bool			bResult		= GWorld->LineTraceSingleByChannel( hitResult, startRay, endRay, CC_WorldStatic, queryParams );
	if ( bResult )
	{
		TAssetHandle<CAudioBank>	audioBank = jumpAudioBanks[ hitResult.physMaterial->GetSurfaceType() ];
		if ( audioBank.IsAssetValid() )
		{
			audioComponent->SetAudioBank( audioBank );
			audioComponent->Play();
		}
	}
}

void AELPlayerCharacter::Walk( const Vector& InWorldDirection, float InScale )
{
	Super::Walk( InWorldDirection, InScale );
}