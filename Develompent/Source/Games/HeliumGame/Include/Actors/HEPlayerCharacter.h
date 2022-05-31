/**
 * @file
 * @addtogroup HeliumGame HeliumGame
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef HEPLAYERCHARACTER_H
#define HEPLAYERCHARACTER_H

#include "Actors/Character.h"
#include "Components/CameraComponent.h"
#include "Components/SpriteComponent.h"
#include "Components/AudioComponent.h"

 /**
  * @ingroup HeliumGame
  * Class of player character
  */
class AHEPlayerCharacter : public ACharacter
{
	DECLARE_CLASS( AHEPlayerCharacter, ACharacter )

public:
	/**
	 * @brief Constructor
	 */
	AHEPlayerCharacter();

	/**
	 * @brief Destructor
	 */
	~AHEPlayerCharacter();

	/**
	 * Overridable native event for when play begins for this actor
	 */
	virtual void BeginPlay() override;

	/**
	 * Function called every frame on this Actor. Override this function to implement custom logic to be executed every frame.
	 *
	 * @param[in] InDeltaTime The time since the last tick.
	 */
	virtual void Tick( float InDeltaTime ) override;

	/**
	 * @brief Walk
	 *
	 * @param InWorldDirection Direction of move in world space
	 * @param InScale Scale of move
	 */
	virtual void Walk( const Vector& InWorldDirection, float InScale ) override;

	/**
	 * @brief Landed
	 */
	virtual void Landed();

private:
	TAssetHandle<CAudioBank>					walkAudioBanks[ ST_Max ];		/**< Audio banks of walking on each surface type */
	TAssetHandle<CAudioBank>					jumpAudioBanks[ ST_Max ];		/**< Audio banks of jump on each surface type */
	TRefCountPtr< CCameraComponent >			cameraComponent;				/**< Camera component */
	TRefCountPtr< CSpriteComponent >			spriteComponent;				/**< Sprite component */
	TRefCountPtr< CAudioComponent >				audioComponent;					/**< Audio component */
};

#endif // !HEPLAYERCHARACTER_H