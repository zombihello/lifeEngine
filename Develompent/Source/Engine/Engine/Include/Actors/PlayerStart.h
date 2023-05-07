/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef PLAYERSTART_H
#define PLAYERSTART_H

#include "Actors/Actor.h"
#include "Components/SpriteComponent.h"
#include "Components/ArrowComponent.h"

/**
 * @ingroup Engine
 * Actor for spawn player controller in world
 */
class APlayerStart : public AActor
{
	DECLARE_CLASS( APlayerStart, AActor )

public:
	/**
	 * Constructor
	 */
	APlayerStart();

	/**
	 * Destructor
	 */
	virtual ~APlayerStart();

	/**
	 * Overridable native event for when play begins for this actor
	 */
	virtual void BeginPlay() override;

#if WITH_EDITOR
	/**
	 * @brief Get path to icon of actor for exploer level in WorldEd
	 * @return Return path to actor icon from Sys_BaseDir()
	 */
	virtual std::wstring GetActorIcon() const override;

private:
	TRefCountPtr<CSpriteComponent>		gizmoComponent;				/**< Gizmo component */
	TRefCountPtr<CArrowComponent>		arrowComponent;				/**< Arrow component */
#endif // WITH_EDITOR
};

#endif // !PLAYERSTART_H