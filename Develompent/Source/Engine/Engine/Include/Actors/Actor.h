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
#include "Components/SceneComponent.h"

/**
 * @ingroup Engine
 * Base class of all actors in world
 */
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
	 * Get array of owned components
	 * @return Return array owned components
	 */
	FORCEINLINE const std::vector< LActorComponentRef >& GetComponents() const
	{
		return ownedComponents;
	}

	/**
	 * Adds a delta to the location of this actor in world space
	 * 
	 * @param[in] InDeltaLocation Delta location
	 */
	FORCEINLINE void AddActorLocation( const FVector& InDeltaLocation )
	{
		if ( !rootComponent )	return;
		rootComponent->AddRelativeLocation( InDeltaLocation );
	}

	/**
	 * Adds a delta to the rotation of this actor in world space
	 * 
	 * @param[in] InDeltaRotation Delta rotation
	 */
	FORCEINLINE void AddActorRotation( const FRotator& InDeltaRotation )
	{
		if ( !rootComponent )	return;
		rootComponent->AddRelativeRotate( InDeltaRotation );
	}

	/**
	 * Adds a delta to the scale of this actor in world space
	 * 
	 * @param[in] InDeltaScale Delta scale
	 */
	FORCEINLINE void AddActorScale( const FVector& InDeltaScale )
	{
		if ( !rootComponent )	return;
		rootComponent->AddRelativeScale( InDeltaScale );
	}

	/**
	 * Set actor location in world space
	 * 
	 * @param[in] InNewLocation New actor location
	 */
	FORCEINLINE void SetActorLocation( const FVector& InNewLocation )
	{
		if ( !rootComponent )	return;
		rootComponent->SetRelativeLocation( InNewLocation );
	}

	/**
	 * Set actor rotation in world space
	 * 
	 * @param[in] InNewRotation New actor rotation
	 */
	FORCEINLINE void SetActorRotation( const FRotator& InNewRotation )
	{
		if ( !rootComponent )	return;
		rootComponent->SetRelativeRotation( InNewRotation );
	}

	/**
	 * Set actor scale in world space
	 * 
	 * @param[in] InNewScale New actor scale
	 */
	FORCEINLINE void SetActorScale( const FVector& InNewScale )
	{
		if ( !rootComponent )	return;
		rootComponent->SetRelativeScale( InNewScale );
	}

	/**
	 * Get actor location in world space
	 * @return Return actor location, if root component is not valid return zero vector
	 */
	FORCEINLINE FVector GetActorLocation() const
	{
		return rootComponent ? rootComponent->GetComponentLocation() : FMath::vectorZero;
	}

	/**
	 * Get actor rotation in world space
	 * @return Return actor rotation, if root component is not valid return zero rotator
	 */
	FORCEINLINE FRotator GetActorRotation() const
	{
		return rootComponent ? rootComponent->GetComponentRotation() : FMath::rotatorZero;
	}

	/**
	 * Get actor scale in world space
	 * @return Return actor scale, if root component is not valid return one vector
	 */
	FORCEINLINE FVector GetActorScale() const
	{
		return rootComponent ? rootComponent->GetComponentScale() : FMath::vectorOne;
	}

	/**
	 * Get actor transform in world space
	 * @return Return actor transform, if root component is not valid return zero transform
	 */
	FORCEINLINE FTransform GetActorTransform() const
	{
		return rootComponent ? rootComponent->GetComponentTransform() : FMath::transformZero;
	}

	/**
	 * Get actor forward vector
	 * @return Return actor forward vector
	 */
	FORCEINLINE FVector GetActorForwardVector() const
	{
		return rootComponent ? rootComponent->GetForwardVector() : FMath::vectorForward;
	}

	/**
	 * Get actor right vector
	 * @return Return actor right vector
	 */
	FORCEINLINE FVector GetActorRightVector() const
	{
		return rootComponent ? rootComponent->GetRightVector() : FMath::vectorRight;
	}

	/**
	 * Get actor up vector
	 * @return Return actor up vector
	 */
	FORCEINLINE FVector GetActorUpVector() const
	{
		return rootComponent ? rootComponent->GetUpVector() : FMath::vectorUp;
	}

protected:
	/**
	 * Create component and add to array of owned components
	 * If creating LSceneComponent, if RootComponent is nullptr, automatic new component setted to root
	 *
	 * @param[in] InClass Class component
	 * @param[in] InName Name component
	 * @return Return pointer to component
	 */
	LActorComponentRef CreateComponent( LClass* InClass, const tchar* InName );

	/**
	 * Create component and add to array of owned components
	 *
	 * @param[in] InName Name component
	 * @return Return pointer to component
	 */
	template< typename TClass >
	FORCEINLINE TRefCountPtr< TClass > CreateComponent( const tchar* InName )
	{
		LActorComponent*		newComponent = CreateComponent( TClass::StaticClass(), InName );
		return ( TClass* )newComponent;
	}

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

	TRefCountPtr< LSceneComponent >				rootComponent;			/**< Root component, default is null */

private:
	std::vector< LActorComponentRef >			ownedComponents;		/**< Owned components */
};

#endif // !ACTOR_H