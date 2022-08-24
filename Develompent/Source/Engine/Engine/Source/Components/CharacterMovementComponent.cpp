#include "Misc/EngineGlobals.h"
#include "System/World.h"
#include "Actors/Actor.h"
#include "Actors/Character.h"
#include "Components/PrimitiveComponent.h"
#include "Components/CharacterMovementComponent.h"
#include "Logger/LoggerMacros.h"

IMPLEMENT_CLASS( CCharacterMovementComponent )

CCharacterMovementComponent::CCharacterMovementComponent()
	: bOnGround( false )
	, bJump( false )
	, bWalk( false )
	, walkSpeed( 200.f )
	, walkSpeedInFly( 200.f )
	, jumpSpeed( 300.f )
	, bodyInstance( nullptr )
	, ownerCharacter( nullptr )
{}

void CCharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();
	ownerCharacter		= GetOwner()->Cast< ACharacter >();
	check( ownerCharacter );

	// TODO BS yehor.pohuliaka - Need add subscribe to event when body instance recreating or changed to other body (in case welding)
	CPrimitiveComponent*		collisionComponent = ownerCharacter->GetCollisionComponent();
	if ( collisionComponent )
	{
		bodyInstance = &collisionComponent->GetBodyInstance();
	}
}

void CCharacterMovementComponent::TickComponent( float InDeltaTime )
{
	Super::TickComponent( InDeltaTime );

	// Check if character on ground
	Vector		startRay	= GetOwner()->GetActorLocation();
	Vector		endRay		= startRay + GetOwner()->GetActorUpVector() * -1.f;
	SHitResult	hitResult;
	bool		bResult		= GWorld->LineTraceSingleByChannel( hitResult, startRay, endRay, CC_WorldStatic );
	if ( bResult && !bOnGround )
	{
		ownerCharacter->Landed();
	}
	bOnGround = bResult;

	// Stop moving character on ZX plane if we not walk
	if ( bodyInstance )
	{
		Vector		velocity = bodyInstance->GetLinearVelocity();
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

void CCharacterMovementComponent::Serialize( class CArchive& InArchive )
{
	Super::Serialize( InArchive );
	InArchive << walkSpeed;
	InArchive << walkSpeedInFly;
	InArchive << jumpSpeed;
}

bool CCharacterMovementComponent::IsCanWalk( const Vector& InWorldDirection, float InScale ) const
{
	Vector		startRay = GetOwner()->GetActorLocation();
	Vector		endRay = startRay + InWorldDirection * InScale;
	SHitResult	hitResult;
	return !GWorld->LineTraceSingleByChannel( hitResult, startRay, endRay, CC_WorldStatic );
}

void CCharacterMovementComponent::Walk( const Vector& InWorldDirection, float InScale )
{
	if ( !bodyInstance )
	{
		AActor*		owner = GetOwner();
		if ( owner )
		{
			owner->AddActorLocation( InWorldDirection * InScale );
		}
	}
	else if ( IsCanWalk( InWorldDirection, InScale ) )
	{
		float			maxVelocity = !IsFly() ? walkSpeed : walkSpeedInFly;
		Vector			velocity = bodyInstance->GetLinearVelocity();	
		velocity.x		= Clamp( velocity.x + InWorldDirection.x * InScale, -maxVelocity, maxVelocity );
		velocity.z		= Clamp( velocity.z + InWorldDirection.z * InScale, -maxVelocity, maxVelocity );
	
		if ( !bJump && bOnGround )
		{
			velocity.y	= Clamp( velocity.y + InWorldDirection.y * InScale, -maxVelocity, maxVelocity );
		}

		bodyInstance->SetLinearVelocity( velocity );	
	}
	else
	{
		return;
	}

	bWalk = true;
}

void CCharacterMovementComponent::Jump()
{
	if ( bJump || !bodyInstance || !bOnGround )
	{
		return;
	}

	Vector			velocity = bodyInstance->GetLinearVelocity();
	velocity.y		+= jumpSpeed;
	bodyInstance->SetLinearVelocity( velocity );
	bJump = true;
}

void CCharacterMovementComponent::StopJump()
{
	bJump = false;
}