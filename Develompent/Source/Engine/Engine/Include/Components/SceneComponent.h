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
#include "Components/ActorComponent.h"

 /**
  * @ingroup Engine
  * A SceneComponent has a transform and supports attachment, but has no rendering or collision capabilities.
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
	 * Attaches a component to a given parent
	 * @warning Need use only in constructor or BeginPlay()
	 * 
	 * @param[in] InParent Parent component
	 */
	void SetupAttachment( CSceneComponent* InParent );

	/**
	 * @brief Serialize component
	 * @param[in] InArchive Archive for serialize
	 */
	virtual void Serialize( class CArchive& InArchive ) override;

	/**
	 * Add to relative location component
	 * 
	 * @param[in] InLocationDelta Relative location delta
	 */
	FORCEINLINE void AddRelativeLocation( const Vector& InLocationDelta )
	{
		transform.AddToTranslation( InLocationDelta );
	}

	/**
	 * Add to relative rotate component
	 * 
	 * @param[in] InRotationDelta Relative rotation delta
	 */
	FORCEINLINE void AddRelativeRotate( const Quaternion& InRotationDelta )
	{
		transform.AddToRotation( InRotationDelta );
	}

	/**
	 * Add to relative scale component
	 * 
	 * @param[in] InScaleDelta Relative scale delta
	 */
	FORCEINLINE void AddRelativeScale( const Vector& InScaleDelta )
	{
		transform.AddToScale( InScaleDelta );
	}

	/**
	 * Set relative location component
	 * 
	 * @param[in] InLocation New relative location
	 */
	FORCEINLINE void SetRelativeLocation( const Vector& InLocation )
	{
		transform.SetLocation( InLocation );
	}

	/**
	 * Set relative rotation component
	 * 
	 * @param[in] InRotation New relative rotation of component
	 */
	FORCEINLINE void SetRelativeRotation( const Quaternion& InRotation )
	{
		transform.SetRotation( InRotation );
	}

	/**
	 * Set relative scale component
	 * 
	 * @param[in] InScale New relative scale
	 */
	FORCEINLINE void SetRelativeScale( const Vector& InScale )
	{
		transform.SetScale( InScale );
	}

	/**
	 * Get relative forward vector
	 * @return Return relative forward vector
	 */
	FORCEINLINE Vector GetRelativeForwardVector() const
	{
		return transform.GetUnitAxis( A_Z );
	}

	/**
	 * Get relative right vector
	 * @return Return relative right vector
	 */
	FORCEINLINE Vector GetRelativeRightVector() const
	{
		return transform.GetUnitAxis( A_X );
	}

	/**
	 * Get relative up vector
	 * @return Return relative up vector
	 */
	FORCEINLINE Vector GetRelativeUpVector() const
	{
		return transform.GetUnitAxis( A_Y );
	}

	/**
	 * Get the relative current transform for this component
	 * @return Return relative current transform for this component
	 */
	FORCEINLINE CTransform GetRelativeTransform() const
	{
		return transform;
	}

	/**
	 * Get the current transform for this component in world space
	 * @return Return current transform in world space for this component
	 */
	FORCEINLINE CTransform GetComponentTransform() const
	{
		return attachParent ? attachParent->GetComponentTransform() + GetRelativeTransform() : GetRelativeTransform();
	}

	/**
	 * Get forward vector in world space
	 * @return Return forward vector in world space
	 */
	FORCEINLINE Vector GetComponentForwardVector() const
	{
		return GetComponentTransform().GetUnitAxis( A_Z );
	}

	/**
	 * Get right vector in world space
	 * @return Return right vector in world space
	 */
	FORCEINLINE Vector GetComponentRightVector() const
	{
		return GetComponentTransform().GetUnitAxis( A_X );
	}

	/**
	 * Get up vector in world space
	 * @return Return up vector in world space
	 */
	FORCEINLINE Vector GetComponentUpVector() const
	{
		return GetComponentTransform().GetUnitAxis( A_Y );
	}

	/**
	 * Get relative location
	 * @return Return relative location
	 */
	FORCEINLINE Vector GetRelativeLocation() const
	{
		return transform.GetLocation();
	}

	/**
	 * Get relative rotation
	 * @return Return relative rotation
	 */
	FORCEINLINE Quaternion GetRelativeRotation() const
	{
		return transform.GetRotation();
	}

	/**
	 * Get relative scale
	 * @return Return relative scale
	 */
	FORCEINLINE Vector GetRelativeScale() const
	{
		return transform.GetScale();
	}

	/**
	 * Get location of the component in world space
	 * @return Return location of the component in world space
	 */
	FORCEINLINE Vector GetComponentLocation() const
	{
		return attachParent ? attachParent->GetComponentLocation() + GetRelativeLocation() : GetRelativeLocation();
	}

	/**
	 * Get rotation of the component in world space
	 * @return Return rotation of the component in world space
	 */
	FORCEINLINE Quaternion GetComponentRotation() const
	{
		return attachParent ? attachParent->GetComponentRotation() + GetRelativeRotation() : GetRelativeRotation();
	}

	/**
	 * Get scale of the component in world space
	 * @return Return scale of the component in world space
	 */
	FORCEINLINE Vector GetComponentScale() const
	{
		return attachParent ? attachParent->GetComponentScale() + GetRelativeScale() : GetRelativeScale();
	}

	/**
	 * Walks up the attachment chain to see if this component is attached to the supplied component
	 * 
	 * @param[in] InTestComp Test component
	 * @return Return true if we attached to test component, else return false. If InTestComp == this, returns false
	 */
	bool IsAttachedTo( CSceneComponent* InTestComp ) const;

	/**
	 * Get the SceneComponent we are attached to
	 * @return Return SceneComponent we are attached to, if parent not exist - return nullptr
	 */
	FORCEINLINE TRefCountPtr< CSceneComponent > GetAttachParent() const
	{
		return attachParent;
	}

private:
	// TODO BS yehor.pohuliaka - Need add array of child components
	TRefCountPtr< CSceneComponent >		attachParent;	/**< What we are currently attached to. If valid, transform are used relative to this object */
	CTransform							transform;		/**< Transform of component */
};

#endif // !SCENECOMPONENT_H