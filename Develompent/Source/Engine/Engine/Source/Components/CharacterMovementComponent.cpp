#include "Misc/EngineGlobals.h"
#include "System/World.h"
#include "Actors/Actor.h"
#include "Actors/Character.h"
#include "Components/PrimitiveComponent.h"
#include "Components/CharacterMovementComponent.h"
#include "Logger/LoggerMacros.h"

IMPLEMENT_CLASS( CCharacterMovementComponent )

/*
==================
CCharacterMovementComponent::CCharacterMovementComponent
==================
*/
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

/*
==================
CCharacterMovementComponent::StaticInitializeClass
==================
*/
void CCharacterMovementComponent::StaticInitializeClass()
{
	new( staticClass, TEXT( "Walk Speed" ) )		CFloatProperty( CPP_PROPERTY( ThisClass, walkSpeed ), TEXT( "Movement" ), TEXT( "Walk speed" ), CPF_Edit );
	new( staticClass, TEXT( "Walk Speed In Fly" ) ) CFloatProperty( CPP_PROPERTY( ThisClass, walkSpeedInFly ), TEXT( "Movement" ), TEXT( "Walk speed in fly" ), CPF_Edit );
	new( staticClass, TEXT( "Jump Speed" ) )		CFloatProperty( CPP_PROPERTY( ThisClass, jumpSpeed ), TEXT( "Movement" ), TEXT( "Jump speed" ), CPF_Edit );
}

/*
==================
CCharacterMovementComponent::BeginPlay
==================
*/
void CCharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();
	ownerCharacter		= Cast<ACharacter>( GetOwner() );
	Assert( ownerCharacter );

	// TODO BS yehor.pohuliaka - Need add subscribe to event when body instance recreating or changed to other body (in case welding)
	CPrimitiveComponent*		collisionComponent = ownerCharacter->GetCollisionComponent();
	if ( collisionComponent )
	{
		bodyInstance = &collisionComponent->GetBodyInstance();
	}
}

/*
==================
CCharacterMovementComponent::TickComponent
==================
*/
void CCharacterMovementComponent::TickComponent( float InDeltaTime )
{
	Super::TickComponent( InDeltaTime );

	// Check if character on ground
	Vector		startRay	= GetOwner()->GetActorLocation();
	Vector		endRay		= startRay + GetOwner()->GetActorUpVector() * -1.f;
	HitResult	hitResult;
	bool		bResult		= g_World->LineTraceSingleByChannel( hitResult, startRay, endRay, CC_WorldStatic );
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

/*
==================
CCharacterMovementComponent::Serialize
==================
*/
void CCharacterMovementComponent::Serialize( class CArchive& InArchive )
{
	Super::Serialize( InArchive );
	InArchive << walkSpeed;
	InArchive << walkSpeedInFly;
	InArchive << jumpSpeed;
}

/*
==================
CCharacterMovementComponent::IsCanWalk
==================
*/
bool CCharacterMovementComponent::IsCanWalk( const Vector& InWorldDirection, float InScale ) const
{
	Vector		startRay = GetOwner()->GetActorLocation();
	Vector		endRay = startRay + InWorldDirection * InScale;
	HitResult	hitResult;
	return !g_World->LineTraceSingleByChannel( hitResult, startRay, endRay, CC_WorldStatic );
}

/*
==================
CCharacterMovementComponent::Walk
==================
*/
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

/*
==================
CCharacterMovementComponent::Jump
==================
*/
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

/*
==================
CCharacterMovementComponent::StopJump
==================
*/
void CCharacterMovementComponent::StopJump()
{
	bJump = false;
}