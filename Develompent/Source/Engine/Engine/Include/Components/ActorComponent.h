/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef ACTORCOMPONENT_H
#define ACTORCOMPONENT_H

#include "Misc/Object.h"
#include "Misc/RefCounted.h"
#include "Misc/EngineTypes.h"

/**
 * @ingroup Engine
 * ActorComponent is the base class for components that define reusable behavior that can be added to different types of Actors.
 * ActorComponents that have a transform are known as SceneComponents and those that can be rendered are PrimitiveComponents.
 */
class LActorComponent : public LObject, public FRefCounted
{
	DECLARE_CLASS( LActorComponent, LObject )

public:
	/**
	 * Constructor
	 */
	LActorComponent();

	/**
	 * Destructor
	 */
	virtual ~LActorComponent();

	/**
	 * Begins Play for the component.
	 * Called when the owning Actor begins play or when the component is created if the Actor has already begun play.
	 */
	virtual void BeginPlay();

	/**
	 * Function called every frame on this ActorComponent. Override this function to implement custom logic to be executed every frame.
	 * 
	 * @param[in] InDeltaTime The time since the last tick.
	 */
	virtual void TickComponent( float InDeltaTime );

	/**
	 * Set owner
	 * @param[in] InOwner Actor owner
	 */
	FORCEINLINE void SetOwner( class AActor* InOwner )
	{
		owner = InOwner;
	}

	/**
	 * Get owner
	 * @return Return owner
	 */
	FORCEINLINE class AActor* GetOwner() const
	{
		return owner;
	}

private:
	class AActor*			owner;		/**< Actor owner */
};

#endif // !ACTORCOMPONENT_H