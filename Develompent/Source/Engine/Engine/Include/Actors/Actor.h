/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef ACTOR_H
#define ACTOR_H

#include <vector>

#include "Misc/Object.h"
#include "Misc/RefCounted.h"
#include "Misc/EngineTypes.h"

class AActor : public LObject, public FRefCounted
{
	DECLARE_CLASS( AActor, LObject )

public:
	/**
	 * Destructor
	 */
	virtual ~AActor();

	/**
	 * Overridable native event for when play begins for this actor
	 */
	virtual void BeginPlay();

	/**
	 * Function called every frame on this Actor. Override this function to implement custom logic to be executed every frame.
	 * 
	 * @param[in] InDeltaTime The time since the last tick.
	 */
	virtual void Tick( float InDeltaTime );

	/**
	 * Puts a component in to the OwnedComponents array of the Actor
	 * @param[in] InComponent Component
	 */
	void AddOwnedComponent( class LActorComponent* InComponent );

	/**
	 * Removes a component from the OwnedComponents array of the Actor
	 * @param[in] InComponent Component
	 */
	void RemoveOwnedComponent( class LActorComponent* InComponent );

	/**
	 * Clear array owned components
	 */
	void ResetOwnedComponents();

	/**
	 * Get array of owned components
	 * @return Return array owned components
	 */
	FORCEINLINE const std::vector< LActorComponentRef >& GetComponents() const
	{
		return ownedComponents;
	}

private:
	std::vector< LActorComponentRef >			ownedComponents;		/**< Owned components */
};

#endif // !ACTOR_H