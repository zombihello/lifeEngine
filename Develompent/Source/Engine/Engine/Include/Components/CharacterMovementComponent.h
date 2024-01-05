/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef CHARACTERMOVEMENTCOMPONENT_H
#define CHARACTERMOVEMENTCOMPONENT_H

#include "Math/Math.h"
#include "System/PhysicsBodyInstance.h"
#include "Components/ActorComponent.h"

/**
 * @ingroup Engine
 * @brief Component for moving character
 */
class CCharacterMovementComponent : public CActorComponent
{
	DECLARE_CLASS( CCharacterMovementComponent, CActorComponent, 0, 0, TEXT( "Engine" ) )

public:
	/**
	 * @brief Constructor
	 */
	CCharacterMovementComponent();

	/**
	 * Begins Play for the component.
	 * Called when the owning Actor begins play or when the component is created if the Actor has already begun play.
	 */
	virtual void BeginPlay() override;

	/**
	 * Function called every frame on this ActorComponent. Override this function to implement custom logic to be executed every frame.
	 *
	 * @param[in] InDeltaTime The time since the last tick.
	 */
	virtual void TickComponent( float InDeltaTime );

	/**
	 * @brief Serialize object
	 * @param[in] InArchive Archive for serialize
	 */
	virtual void Serialize( class CArchive& InArchive ) override;

	/**
	 * @brief Walk
	 * 
	 * @param InWorldDirection Direction of move in world space
	 * @param InScale Scale of move
	 */
	void Walk( const Vector& InWorldDirection, float InScale );

	/**
	 * @brief Jump
	 */
	void Jump();

	/**
	 * @brief Stop jump
	 */
	void StopJump();

	/**
	 * @brief Set walk speed
	 * @param InWalkSpeed New walk speed
	 */
	FORCEINLINE void SetWalkSpeed( float InWalkSpeed )
	{
		walkSpeed = InWalkSpeed;
	}

	/**
	 * @brief Set walk speed in fly
	 * @param InWalkSpeedInFly New walk speed in fly
	 */
	FORCEINLINE void SetWalkSpeedInFly( float InWalkSpeedInFly )
	{
		walkSpeedInFly = InWalkSpeedInFly;
	}

	/**
	 * @brief Set jump speed
	 * @param InJumpSpeed New jump speed
	 */
	FORCEINLINE void SetJumpSpeed( float InJumpSpeed )
	{
		jumpSpeed = InJumpSpeed;
	}

	/**
	 * @brief Is fly
	 * @return Return TRUE if character in fly, else return FALSE
	 */
	FORCEINLINE bool IsFly() const
	{
		return !bOnGround;
	}

	/**
	 * @brief Is jump
	 * @return Return TRUE if character is jump, else return FALSE
	 */
	FORCEINLINE bool IsJump() const
	{
		return bJump;
	}

	/**
	 * @brief Is walking
	 * @return Return TRUE if character is walking, else return FALSE
	 */
	FORCEINLINE bool IsWalk() const
	{
		return bWalk;
	}

	/**
	 * @brief Get walk speed
	 * @return Return walk speed
	 */
	FORCEINLINE float GetWalkSpeed() const
	{
		return walkSpeed;
	}

	/**
	 * @brief Get walk speed in fly
	 * @return Return walk speed in fly
	 */
	FORCEINLINE float GetWalkSpeedInFly() const
	{
		return walkSpeedInFly;
	}

	/**
	  * @brief Get jump speed
	  * @return Return jump speed
	  */
	FORCEINLINE float GetJumpSpeed() const
	{
		return jumpSpeed;
	}

private:
	/**
	 * @brief Is can walk
	 * 
	 * @param InWorldDirection Direction of move in world space
	 * @param InScale Scale of move
	 * @return Return TRUE if character can walk, else return FALSE
	 */
	bool IsCanWalk( const Vector& InWorldDirection, float InScale ) const;

	bool							bOnGround;			/**< Is character on ground */
	bool							bJump;				/**< Is character is jump */
	bool							bWalk;				/**< Is cahracter is walking */
	float							walkSpeed;			/**< Walk speed */
	float							walkSpeedInFly;		/**< Walk speed in fly */
	float							jumpSpeed;			/**< Jump speed */
	CPhysicsBodyInstance*			bodyInstance;		/**< Pointer to body instance */
	class ACharacter*				ownerCharacter;		/**< Owner character */
};

#endif // !CHARACTERMOVEMENTCOMPONENT_H