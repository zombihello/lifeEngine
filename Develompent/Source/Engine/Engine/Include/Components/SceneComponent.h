/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef SCENECOMPONENT_H
#define SCENECOMPONENT_H

#include "Math/Transform.h"
#include "Math/Rotator.h"
#include "Components/ActorComponent.h"

 /**
  * @ingroup Engine
  * @brief A SceneComponent has a transform and supports attachment, but has no rendering or collision capabilities.
  */
class CSceneComponent : public CActorComponent
{
	DECLARE_CLASS( CSceneComponent, CActorComponent, 0, 0 )

public:
	/**
	 * @brief Constructor
	 */
	CSceneComponent();

	/**
	 * @brief Destructor
	 */
	virtual ~CSceneComponent();

	/**
	 * @brief Attaches a component to a given parent
	 * @warning Need use only in constructor or BeginPlay()
	 * 
	 * @param InParent		Parent component
	 */
	void SetupAttachment( CSceneComponent* InParent );

	/**
	 * @brief Called when this component is destroyed
	 */
	virtual void Destroyed() override;

	/**
	 * @brief Serialize component
	 * @param[in] InArchive Archive for serialize
	 */
	virtual void Serialize( class CArchive& InArchive ) override;

	/**
	 * @brief Detach this component from whatever it is attached to
	 * 
	 * @param InIsMaintainWorldPosition		Is need save world position
	 */
	void DetachFromParent( bool InIsMaintainWorldPosition = false );

#if WITH_EDITOR
	/**
	 * @brief Function called by the editor when property is changed
	 * @param InPropertyChangedEvenet    Property changed event
	 */
	virtual void PostEditChangeProperty( const PropertyChangedEvenet& InPropertyChangedEvenet ) override;

	/**
	 * @brief Function called every frame for drawing debug geometry. Override this function to implement custom logic of drawing debug geometry
	 */
	virtual void DrawDebugComponent();
#endif // WITH_EDITOR

	/**
	 * @brief Add to relative location component
	 * 
	 * @param InLocationDelta	Relative location delta
	 */
	FORCEINLINE void AddRelativeLocation( const Vector& InLocationDelta )
	{
		SetRelativeLocation( GetRelativeLocation() + InLocationDelta );
	}

	/**
	 * @brief Add to relative rotation component
	 * 
	 * @param InRotationDelta	Relative rotation delta
	 */
	FORCEINLINE void AddRelativeRotation( const Quaternion& InRotationDelta )
	{
		const Quaternion	currentRelativeRotation = relativeRotationCache.RotatorToQuat( GetRelativeRotation() );
		const Quaternion	newRelativeRotation		= InRotationDelta * currentRelativeRotation;
		SetRelativeRotation( newRelativeRotation );
	}

	/**
	 * @brief Add to relative rotation component
	 *
	 * @param InRotationDelta	Relative rotation delta
	 */
	FORCEINLINE void AddRelativeRotation( const CRotator& InRotationDelta )
	{
		SetRelativeRotation( GetRelativeRotation() + InRotationDelta );
	}

	/**
	 * @brief Add to relative scale component
	 * 
	 * @param InScaleDelta	Relative scale delta
	 */
	FORCEINLINE void AddRelativeScale( const Vector& InScaleDelta )
	{
		SetRelativeScale( GetRelativeScale() + InScaleDelta );
	}

	/**
	 * @brief Set relative location component
	 * 
	 * @param InLocation	New relative location
	 */
	FORCEINLINE void SetRelativeLocation( const Vector& InLocation )
	{
		if ( InLocation != GetRelativeLocation() )
		{
			relativeLocation		= InLocation;
			bDityComponentToWorld	= true;
		}
	}

	/**
	 * @brief Set relative rotation component
	 * 
	 * @param InRotation	New relative rotation of component
	 */
	FORCEINLINE void SetRelativeRotation( const Quaternion& InRotation )
	{
		const CRotator	newRelativeRotation = relativeRotationCache.QuatToRotator_ReadOnly( InRotation );
		if ( !newRelativeRotation.Equals( GetRelativeRotation() ) )
		{
			relativeRotation		= newRelativeRotation;
			bDityComponentToWorld	= true;
			relativeRotationCache.QuatToRotator( InRotation );
		}
	}

	/**
	 * @brief Set relative rotation component
	 *
	 * @param InRotation	New relative rotation of component
	 */
	FORCEINLINE void SetRelativeRotation( const CRotator& InRotation )
	{
		if ( !InRotation.Equals( GetRelativeRotation() ) )
		{
			relativeRotation		= InRotation;
			bDityComponentToWorld	= true;
			relativeRotationCache.RotatorToQuat( InRotation );
		}
	}

	/**
	 * @brief Set relative scale component
	 * 
	 * @param InScale	New relative scale
	 */
	FORCEINLINE void SetRelativeScale( const Vector& InScale )
	{
		if ( InScale != GetRelativeScale() )
		{
			relativeScale			= InScale;
			bDityComponentToWorld	= true;
		}
	}

	/**
	 * @brief Get the relative current transform for this component
	 * @return Return relative current transform for this component
	 */
	FORCEINLINE CTransform GetRelativeTransform() const
	{
		const CTransform	relativeTransform( relativeRotationCache.RotatorToQuat( GetRelativeRotation() ), GetRelativeLocation(), GetRelativeScale() );
		return relativeTransform;
	}

	/**
	 * @brief Get the current transform for this component in world space
	 * @return Return current transform in world space for this component
	 */
	const CTransform& GetComponentTransform() const;

	/**
	 * @brief Get relative location
	 * @return Return relative location
	 */
	FORCEINLINE Vector GetRelativeLocation() const
	{
		return relativeLocation;
	}

	/**
	 * @brief Get relative rotation
	 * @return Return relative rotation
	 */
	FORCEINLINE CRotator GetRelativeRotation() const
	{
		return relativeRotation;
	}

	/**
	 * @brief Get relative scale
	 * @return Return relative scale
	 */
	FORCEINLINE Vector GetRelativeScale() const
	{
		return relativeScale;
	}

	/**
	 * @brief Get location of the component in world space
	 * @return Return location of the component in world space
	 */
	FORCEINLINE Vector GetComponentLocation() const
	{
		return GetComponentTransform().GetLocation();
	}

	/**
	 * @brief Get rotation of the component in world space
	 * @return Return rotation of the component in world space
	 */
	FORCEINLINE CRotator GetComponentRotation() const
	{
		return worldRotationCache.NormalizedQuatToRotator( GetComponentTransform().GetRotation() );
	}

	/**
	 * @brief Get rotation quaternion of the component in world space
	 * @return Return rotation quaternion of the component in world space
	 */
	FORCEINLINE Quaternion GetComponentQuat() const
	{
		return GetComponentTransform().GetRotation();
	}

	/**
	 * @brief Get scale of the component in world space
	 * @return Return scale of the component in world space
	 */
	FORCEINLINE Vector GetComponentScale() const
	{
		return GetComponentTransform().GetScale();
	}

	/**
	 * @brief Walks up the attachment chain to see if this component is attached to the supplied component
	 * 
	 * @param[in] InTestComp Test component
	 * @return Return true if we attached to test component, else return false. If InTestComp == this, returns false
	 */
	bool IsAttachedTo( CSceneComponent* InTestComp ) const;

	/**
	 * @brief Get the SceneComponent we are attached to
	 * @return Return SceneComponent we are attached to, if parent not exist - return nullptr
	 */
	FORCEINLINE TRefCountPtr<CSceneComponent> GetAttachParent() const
	{
		return attachParent;
	}

	/**
	 * @brief Get the SceneComponents that are attached to this component
	 * @return Return the SceneComponents that are attached to this component
	 */
	FORCEINLINE const std::vector<TRefCountPtr<CSceneComponent>>&GetAttachChildren() const
	{
		return attachChildren;
	}

private:
	/**
	 * @brief Update component to world
	 */
	void UpdateComponentToWorld();

	bool											bDityComponentToWorld;	/**< Is need update ComponentToWorld */
	TRefCountPtr<CSceneComponent>					attachParent;			/**< What we are currently attached to. If valid, transform are used relative to this object */
	Vector											relativeLocation;		/**< Location of the component relative to its parent */
	CRotator										relativeRotation;		/**< Rotation of the component relative to its parent */
	Vector											relativeScale;			/**< Non-uniform scaling of the component relative to its parent */
	RotationConversionCache						worldRotationCache;		/**< Cache that avoids Quat<->Rotator conversions if possible. Only to be used with GetComponentTransform().GetRotation() */	
	RotationConversionCache						relativeRotationCache;	/**< Cache that avoids Quat<->Rotator conversions if possible. Only to be used with 'relativeRotation' */
	CTransform										componentToWorld;		/**< Current transform of the component, relative to the world */
	std::vector<TRefCountPtr<CSceneComponent>>		attachChildren;			/**< List of child SceneComponents that are attached to us */
};

#endif // !SCENECOMPONENT_H