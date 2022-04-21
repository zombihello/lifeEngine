#include "Misc/EngineGlobals.h"
#include "System/World.h"
#include "Actors/Actor.h"
#include "Actors/Character.h"
#include "Components/PrimitiveComponent.h"
#include "Components/CharacterMovementComponent.h"
#include "Logger/LoggerMacros.h"

IMPLEMENT_CLASS( LCharacterMovementComponent )

LCharacterMovementComponent::LCharacterMovementComponent()
	: bOnGround( false )
	, bJump( false )
	, bWalk( false )
	, walkSpeed( 200.f )
	, walkSpeedInFly( 200.f )
	, jumpSpeed( 300.f )
	, bodyInstance( nullptr )
	, ownerCharacter( nullptr )
{}

void LCharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();
	ownerCharacter		= GetOwner()->Cast< ACharacter >();
	check( ownerCharacter );

	// TODO BS yehor.pohuliaka - Need add subscribe to event when body instance recreating or changed to other body (in case welding)
	LPrimitiveComponent*		collisionComponent = ownerCharacter->GetCollisionComponent();
	if ( collisionComponent )
	{
		bodyInstance = &collisionComponent->GetBodyInstance();
	}
}

void LCharacterMovementComponent::TickComponent( float InDeltaTime )
{
	Super::TickComponent( InDeltaTime );

	// Check if character on ground
	FVector		startRay	= GetOwner()->GetActorLocation();
	FVector		endRay		= startRay + GetOwner()->GetActorUpVector() * -1.f;
	FHitResult	hitResult;
	bool		bResult		= GWorld->LineTraceSingleByChannel( hitResult, startRay, endRay, CC_WorldStatic );
	if ( bResult && !bOnGround )
	{
		ownerCharacter->Landed();
	}
	bOnGround = bResult;

	// Stop moving character on ZX plane if we not walk
	if ( bodyInstance )
	{
		FVector		velocity = bodyInstance->GetLinearVelocity();
		bool		bMoving = velocity.x != 0.f || velocity.z != 0.f;
		if ( bMoving && !bWalk && bOnGround )
		{
			velocity.x = 0.f;
			velocity.y = 0.f;
			bodyInstance->SetLinearVelocity( velocity );
		}
	}

	bWalk = false;
}

void LCharacterMovementComponent::Serialize( class FArchive& InArchive )
{
	Super::Serialize( InArchive );
	InArchive << walkSpeed;
	InArchive << walkSpeedInFly;
	InArchive << jumpSpeed;
}

bool LCharacterMovementComponent::IsCanWalk( const FVector& InWorldDirection, float InScale ) const
{
	FVector		startRay = GetOwner()->GetActorLocation();
	FVector		endRay = startRay + InWorldDirection * InScale;
	FHitResult	hitResult;
	return !GWorld->LineTraceSingleByChannel( hitResult, startRay, endRay, CC_WorldStatic );
}

void LCharacterMovementComponent::Walk( const FVector& InWorldDirection, float InScale )
{
	if ( !bodyInstance || !IsCanWalk( InWorldDirection, InScale ) )
	{
		return;
	}

	float			maxVelocity = !IsFly() ? walkSpeed : walkSpeedInFly;
	FVector			velocity = bodyInstance->GetLinearVelocity();	
	velocity.x		= Clamp( velocity.x + InWorldDirection.x * InScale, -maxVelocity, maxVelocity );
	velocity.z		= Clamp( velocity.z + InWorldDirection.z * InScale, -maxVelocity, maxVelocity );
	
	if ( !bJump && bOnGround )
	{
		velocity.y	= Clamp( velocity.y + InWorldDirection.y * InScale, -maxVelocity, maxVelocity );
	}

	bodyInstance->SetLinearVelocity( velocity );
	bWalk = true;
}

void LCharacterMovementComponent::Jump()
{
	if ( bJump || !bodyInstance || !bOnGround )
	{
		return;
	}

	FVector			velocity = bodyInstance->GetLinearVelocity();
	velocity.y		+= jumpSpeed;
	bodyInstance->SetLinearVelocity( velocity );
	bJump = true;
}

void LCharacterMovementComponent::StopJump()
{
	bJump = false;
}