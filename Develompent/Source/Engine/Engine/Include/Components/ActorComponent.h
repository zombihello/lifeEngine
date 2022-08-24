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
class CActorComponent : public CObject, public CRefCounted
{
	DECLARE_CLASS( CActorComponent, CObject )

public:
	/**
	 * Constructor
	 */
	CActorComponent();

	/**
	 * Destructor
	 */
	virtual ~CActorComponent();

	/**
	 * Begins Play for the component.
	 * Called when the owning Actor begins play or when the component is created if the Actor has already begun play.
	 */
	virtual void BeginPlay();

	/**
	 * End play for the component.
	 * Called when owning actor eng play
	 */
	virtual void EndPlay();

	/**
	 * Function called every frame on this ActorComponent. Override this function to implement custom logic to be executed every frame.
	 * 
	 * @param[in] InDeltaTime The time since the last tick.
	 */
	virtual void TickComponent( float InDeltaTime );

	/**
	 * @brief Called when the owning Actor is spawned
	 */
	virtual void Spawned();

	/**
	 * @brief Called when this component is destroyed
	 */
	virtual void Destroyed();

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

#if WITH_EDITOR
	/**
	 * Set editor only
	 * @param InEditorOnly	Is component only for editor
	 */
	FORCEINLINE void SetEditorOnly( bool InEditorOnly )
	{
		bEditorOnly = InEditorOnly;
	}

	/**
	 * Is editor only
	 * @return Return TRUE if component only for editor
	 */
	FORCEINLINE bool IsEditorOnly() const
	{
		return bEditorOnly;
	}
#endif // WITH_EDITOR

private:
#if WITH_EDITOR
	bool					bEditorOnly;	/**< Is component only for editor */
#endif // WITH_EDITOR

	class AActor*			owner;			/**< Actor owner */
};

#endif // !ACTORCOMPONENT_H