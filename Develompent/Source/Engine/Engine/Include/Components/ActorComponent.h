/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef ACTORCOMPONENT_H
#define ACTORCOMPONENT_H

#include "Misc/RefCounted.h"
#include "Reflection/ReflectionEnvironment.h"
#include "Reflection/Object.h"
#include "Reflection/Property.h"

/**
 * @ingroup Engine
 * ActorComponent is the base class for components that define reusable behavior that can be added to different types of Actors.
 * ActorComponents that have a transform are known as SceneComponents and those that can be rendered are PrimitiveComponents.
 */
class CActorComponent : public CObject 
{
	DECLARE_CLASS( CActorComponent, CObject, 0, 0, TEXT( "Engine" ) )

public:
	/**
	 * Constructor
	 */
	CActorComponent();

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
	 * @brief Get owner
	 * @return Return owner
	 */
	class AActor* GetOwner() const;

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

	/**
	 * @brief Get world where this component is
	 * @return Return a pointer to world where this component is
	 */
	FORCEINLINE class CWorld* GetWorld() const
	{
		return worldPrivate ? worldPrivate : GetWorld_Uncached();
	}

private:
	/**
	 * @brief Get world where this component is
	 * This function use for cache pointer to world
	 *
	 * @return Return a pointer to world where this component is
	 */
	class CWorld* GetWorld_Uncached() const;

#if WITH_EDITOR
	bool					bEditorOnly;	/**< Is component only for editor */
#endif // WITH_EDITOR

	class CWorld*			worldPrivate;	/**< Pointer to world where this component is */
};

#endif // !ACTORCOMPONENT_H