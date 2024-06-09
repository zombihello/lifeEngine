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

#include "Math/Rect.h"
#include "Math/Color.h"
#include "System/Delegate.h"
#include "Reflection/ObjectMacros.h"
#include "Reflection/Property.h"
#include "Render/Material.h"
#include "Render/HitProxies.h"
#include "Components/SceneComponent.h"
#include "Components/PrimitiveComponent.h"

/**
 * @ingroup Engine
 * @brief Delegate for called event when actor is destroyed
 */
DECLARE_MULTICAST_DELEGATE( COnActorDestroyed, class AActor* );

/**
 * @ingroup Engine
 * @brief Base class of all actors in world
 */
class AActor : public CObject
{
	DECLARE_CLASS( AActor, CObject, CLASS_HasComponents, 0, TEXT( "Engine" ) )

public:
	friend struct MarkActorIsBeingDestroyed;

	/**
	 * @brief Constructor
	 */
	AActor();

	/**
	 * @brief Called before destroying the object
	 * This is called immediately upon deciding to destroy the object, to allow the object to begin an
	 * asynchronous cleanup process
	 */
	virtual void BeginDestroy() override;

	/**
	 * @brief Overridable native event for when play begins for this actor
	 */
	virtual void BeginPlay();

	/**
	 * @brief Overridable native event for when end play for this actor
	 */
	virtual void EndPlay();

	/**
	 * @brief Function called every frame on this Actor. Override this function to implement custom logic to be executed every frame.
	 * 
	 * @param InDeltaTime	The time since the last tick.
	 */
	virtual void Tick( float InDeltaTime );
	
	/**
	 * @brief Called when this actor is spawned
	 */
	virtual void Spawned();

	/**
	 * @brief Called when this actor is destroyed
	 */
	virtual void Destroyed();

	/**
	 * @brief Destroy this actor. Returns TRUE the actor is destroyed or already marked for destruction, FALSE if indestructible
	 * Destruction is latent. It occurs at the end of the tick
	 * @returns	Return TRUE if destroyed or already marked for destruction, FALSE if indestructible
	 */
	bool Destroy();

	/**
	 * @brief Init physics body
	 */
	void InitPhysics();

	/**
	 * @brief Terminate physics body
	 */
	void TermPhysics();

	/**
	 * @brief Sync actor to physics body
	 */
	void SyncPhysics();

#if WITH_EDITOR
	/**
	 * @brief Get path to icon of actor for exploer level in WorldEd
	 * @return Return path to actor icon from Sys_BaseDir()
	 */
	virtual std::wstring GetActorIcon() const;
#endif // WITH_EDITOR

	/**
	 * @brief Get array of owned components
	 * @return Return array owned components
	 */
	FORCEINLINE const std::vector<CActorComponent*>& GetComponents() const
	{
		return ownedComponents;
	}

	/**
	 * @brief Is the actor pending kill
	 * This is set to TRUE in CWorld::DestroyActor
	 * 
	 * @return Return TRUE if this actor has begun the destruction process, otherwise returns FALSE
	 */
	FORCEINLINE bool IsPendingKillPending() const
	{
		return bActorIsBeingDestroyed || IsPendingKill();
	}

	/**
	 * @brief Adds a delta to the location of this actor in world space
	 * 
	 * @param InDeltaLocation	Delta location
	 */
	FORCEINLINE void AddActorLocation( const Vector& InDeltaLocation )
	{
		if ( !rootComponent )	return;
		rootComponent->AddRelativeLocation( InDeltaLocation );
	}

	/**
	 * @brief Adds a delta to the rotation of this actor in world space
	 * 
	 * @param InDeltaRotation	Delta rotation
	 */
	FORCEINLINE void AddActorRotation( const Quaternion& InDeltaRotation )
	{
		if ( !rootComponent )	return;
		rootComponent->AddRelativeRotation( InDeltaRotation );
	}

	/**
	 * @brief Adds a delta to the rotation of this actor in world space
	 *
	 * @param InDeltaRotation	Delta rotation
	 */
	FORCEINLINE void AddActorRotation( const CRotator& InDeltaRotation )
	{
		if ( !rootComponent )	return;
		rootComponent->AddRelativeRotation( InDeltaRotation );
	}

	/**
	 * @brief Adds a delta to the scale of this actor in world space
	 * 
	 * @param InDeltaScale	Delta scale
	 */
	FORCEINLINE void AddActorScale( const Vector& InDeltaScale )
	{
		if ( !rootComponent )	return;
		rootComponent->AddRelativeScale( InDeltaScale );
	}

	/**
	 * @brief Set actor location in world space
	 * 
	 * @param InNewLocation		New actor location
	 */
	FORCEINLINE void SetActorLocation( const Vector& InNewLocation )
	{
		if ( !rootComponent )	return;
		rootComponent->SetRelativeLocation( InNewLocation );
	}

	/**
	 * @brief Set actor rotation in world space
	 * 
	 * @param InNewRotation		New actor rotation
	 */
	FORCEINLINE void SetActorRotation( const Quaternion& InNewRotation )
	{
		if ( !rootComponent )	return;
		rootComponent->SetRelativeRotation( InNewRotation );
	}

	/**
	 * @brief Set actor rotation in world space
	 *
	 * @param InNewRotation		New actor rotation
	 */
	FORCEINLINE void SetActorRotation( const CRotator& InNewRotation )
	{
		if ( !rootComponent )	return;
		rootComponent->SetRelativeRotation( InNewRotation );
	}

	/**
	 * @brief Set actor scale in world space
	 * 
	 * @param InNewScale	New actor scale
	 */
	FORCEINLINE void SetActorScale( const Vector& InNewScale )
	{
		if ( !rootComponent )	return;
		rootComponent->SetRelativeScale( InNewScale );
	}

	/**
	 * @brief Set static actor
	 * @param InIsStatic Is actor static
	 */
	FORCEINLINE void SetStatic( bool InIsStatic )
	{
		bNeedReinitCollision = InIsStatic != bIsStatic ? true : bNeedReinitCollision;
		bIsStatic = InIsStatic;
	}

#if ENABLE_HITPROXY
	/**
	 * @brief Set hit proxy id
	 * @param InIndex	Index
	 */
	FORCEINLINE void SetHitProxyId( uint32 InIndex )
	{
		hitProxyId.SetIndex( InIndex );
	}

	/**
	 * @brief Get hit proxy id
	 * @return Return hit proxy id
	 */
	FORCEINLINE const CHitProxyId& GetHitProxyId() const
	{
		return hitProxyId;
	}
#endif // ENABLE_HITPROXY

#if WITH_EDITOR
	/**
	 * @brief Set selected actor
	 * @param InIsSelected	Is selected this actor
	 */
	FORCEINLINE void SetSelected( bool InIsSelected )
	{
		bSelected = InIsSelected;
	}

	/**
	 * @brief Is selected actor
	 * @return Return TRUE if actor is selected, else returning FALSE
	 */
	FORCEINLINE bool IsSelected() const
	{
		return bSelected;
	}
#endif // WITH_EDITOR

	/**
	 * @brief Set visibility
	 *
	 * @param InNewVisibility New visibility
	 */
	FORCEINLINE void SetVisibility( bool InNewVisibility )
	{
		bVisibility = InNewVisibility;
	}

	/**
	 * @brief Get visibility
	 * @return Return visibility of actor
	 */
	FORCEINLINE bool IsVisibility() const
	{
		return bVisibility;
	}

	/**
	 * @brief Get actor location in world space
	 * @return Return actor location, if root component is not valid return zero vector
	 */
	FORCEINLINE Vector GetActorLocation() const
	{
		return rootComponent ? rootComponent->GetComponentLocation() : Math::vectorZero;
	}

	/**
	 * @brief Get actor rotation in world space
	 * @return Return actor rotation, if root component is not valid return zero rotator
	 */
	FORCEINLINE CRotator GetActorRotation() const
	{
		return rootComponent ? rootComponent->GetComponentRotation() : Math::quaternionZero;
	}

	/**
	 * @brief Get actor's quaternion rotation in world space
	 * @return Return actor's quaternion rotation, if root component is not valid return zero quaternion
	 */
	FORCEINLINE Quaternion GetActorQuat() const
	{
		return rootComponent ? rootComponent->GetComponentQuat() : Math::quaternionZero;
	}

	/**
	 * @brief Get actor scale in world space
	 * @return Return actor scale, if root component is not valid return one vector
	 */
	FORCEINLINE Vector GetActorScale() const
	{
		return rootComponent ? rootComponent->GetComponentScale() : Math::vectorOne;
	}

	/**
	 * @brief Get actor transform in world space
	 * @return Return actor transform, if root component is not valid return zero transform
	 */
	FORCEINLINE CTransform GetActorTransform() const
	{
		return rootComponent ? rootComponent->GetComponentTransform() : Math::transformZero;
	}

	/**
	 * @brief Get actor forward vector
	 * @return Return actor forward vector
	 */
	FORCEINLINE Vector GetActorForwardVector() const
	{
		if ( !rootComponent )
		{
			return Math::vectorForward;
		}
		else
		{
			const CTransform&	componentTransform = rootComponent->GetComponentTransform();
			return componentTransform.GetUnitAxis( A_Forward );
		}
	}

	/**
	 * @brief Get actor right vector
	 * @return Return actor right vector
	 */
	FORCEINLINE Vector GetActorRightVector() const
	{
		if ( !rootComponent )
		{
			return Math::vectorForward;
		}
		else
		{
			const CTransform&	componentTransform = rootComponent->GetComponentTransform();
			return componentTransform.GetUnitAxis( A_Right );
		}
	}

	/**
	 * @brief Get actor up vector
	 * @return Return actor up vector
	 */
	FORCEINLINE Vector GetActorUpVector() const
	{
		if ( !rootComponent )
		{
			return Math::vectorForward;
		}
		else
		{
			const CTransform&	componentTransform = rootComponent->GetComponentTransform();
			return componentTransform.GetUnitAxis( A_Up );
		}
	}

	/**
	 * @brief Get collision component
	 * @return Return collision component. If not exist return nullptr
	 */
	FORCEINLINE CPrimitiveComponent* GetCollisionComponent() const
	{
		return collisionComponent;
	}

	/**
	 * @brief Is static actor
	 * @return Return true if actor is static, else return false
	 */
	FORCEINLINE bool IsStatic() const
	{
		return bIsStatic;
	}

	/**
	 * @brief Is actor playing
	 * @return Return TRUE if actor is playing, else returning FALSE
	 */
	FORCEINLINE bool IsPlaying() const
	{
		return bBeginPlay;
	}

protected:
	/**
	 * @brief Create component and add to array of owned components
	 * If creating CSceneComponent, if RootComponent is nullptr, automatic new component setted to root
	 *
	 * @param InClass		Class component
	 * @param InName		Name component
	 * @param InEditorOnly	Is editor only component
	 * @return Return pointer to component
	 */
	CActorComponent* CreateComponent( CClass* InClass, const CName& InName, bool InEditorOnly = false );

	/**
	 * @brief Create component and add to array of owned components
	 *
	 * @param InName	Name component
	 * @return Return pointer to component
	 */
	template<typename TClass>
	FORCEINLINE TClass* CreateComponent( const CName& InName, bool InEditorOnly = false )
	{
		CActorComponent*	newComponent = CreateComponent( TClass::StaticClass(), InName, InEditorOnly );
		return ( TClass* )newComponent;
	}

	/**
	 * @brief Puts a component in to the OwnedComponents array of the Actor
	 * @param InComponent	Component
	 */
	void AddOwnedComponent( class CActorComponent* InComponent );

	/**
	 * @brief Removes a component from the OwnedComponents array of the Actor
	 * @param InComponent	Component
	 */
	void RemoveOwnedComponent( class CActorComponent* InComponent );

	/**
	 * @brief Clear array owned components
	 */
	void ResetOwnedComponents();

	/**
	 * @brief Get event when actor is destroyed
	 * @return Return event when actor is destroyed
	 */
	FORCEINLINE COnActorDestroyed& OnActorDestroyed() const
	{
		return onActorDestroyed;
	}

	/**
	 * @brief Get world where this actor is
	 * @return Return a pointer to world where this actor is
	 */
	FORCEINLINE CWorld* GetWorld() const
	{
		return worldPrivate ? worldPrivate : GetWorld_Uncached();
	}

	CSceneComponent*							rootComponent;			/**< Root component, default is null */
	CPrimitiveComponent*						collisionComponent;		/**< Collision component */

private:
	/**
	 * @brief Get world where this actor is
	 * This function use for cache pointer to world
	 * 
	 * @return Return a pointer to world where this actor is
	 */
	CWorld* GetWorld_Uncached() const;

	bool										bIsStatic;				/**< Is static actor */
	bool										bNeedReinitCollision;	/**< Is need reinit collision component */
	bool										bActorIsBeingDestroyed;	/**< Actor is being destroyed */
	bool										bBeginPlay;				/**< Is begin play for this actor */
	bool										bVisibility;			/**< Is actor visibility */

#if WITH_EDITOR
	bool										bSelected;				/**< Is selected this actor */
#endif // WITH_EDITOR

	class CWorld*								worldPrivate;			/**< Pointer to world where this actor is */
	std::vector<CActorComponent*>				ownedComponents;		/**< Owned components */
	mutable COnActorDestroyed					onActorDestroyed;		/**< Called event when actor is destroyed */

#if ENABLE_HITPROXY
	CHitProxyId									hitProxyId;				/**< Hit proxy id */
#endif // ENABLE_HITPROXY
};

/**
 * @ingroup Engine
 * @brief Internal struct used by level code to mark actors as destroyed
 */
struct MarkActorIsBeingDestroyed
{
private:
	friend class CWorld;

	/**
	 * @brief Constructor
	 * @param InActor	Actor
	 */
	MarkActorIsBeingDestroyed( AActor* InActor )
	{
		InActor->bActorIsBeingDestroyed = true;
	}
};

#endif // !ACTOR_H