/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef CAMERACOMPONENT_H
#define CAMERACOMPONENT_H

#include "Render/CameraTypes.h"
#include "Components/SceneComponent.h"

 /**
  * @ingroup Engine
  * Represents a camera viewpoint and settings, such as projection type, field of view, and post-process overrides
  */
class LCameraComponent : public LSceneComponent
{
	DECLARE_CLASS( LCameraComponent, LSceneComponent )

public:
	/**
	 * Constructor
	 */
	LCameraComponent();

	/**
	 * Rotate camera by mouse
	 * 
	 * @param[in] InConstrainYaw Is constrain Yaw 
	 */
	void RotateComponentByMouse( bool InConstrainYaw = true );

	/**
	 * @brief Serialize component
	 * @param[in] InArchive Archive for serialize
	 */
	virtual void Serialize( class FArchive& InArchive ) override;

	/**
	 * Set projection mode
	 * 
	 * @param[in] InProjectionMode Projection mode
	 */
	FORCEINLINE void SetProjectionMode( ECameraProjectionMode InProjectionMode )
	{
		projectionMode = InProjectionMode;
	}

	/**
	 * Set field of view (in degrees) in perspective mode (ignored in Orthographic mode)
	 * 
	 * @param[in] InFieldOfView Field of view (in degrees) in perspective mode
	 */
	FORCEINLINE void SetFieldOfView( float InFieldOfView )
	{
		fieldOfView = InFieldOfView;
	}

	/**
	 * Set width of orthographic projection
	 * 
	 * @param[in] InOrthoWidth Width of orthographic projection
	 */
	FORCEINLINE void SetOrthoWidth( float InOrthoWidth )
	{
		orthoWidth = InOrthoWidth;
	}

	/**
	 * Set height of orthographic projection
	 * 
	 * @param[in] InOrthoHeight Height of orthographic projection
	 */
	FORCEINLINE void SetOrthoHeight( float InOrthoHeight )
	{
		orthoHeight = InOrthoHeight;
	}

	/**
	 * Set near clip plane
	 * 
	 * @param[in] InNearClipPlane The near plane distance (in world units)
	 */
	FORCEINLINE void SetNearClipPlane( float InNearClipPlane )
	{
		nearClipPlane = InNearClipPlane;
	}

	/**
	 * Set far clip plane
	 * 
	 * @param[in] InFarClipPlane The far plane distance (in world units)
	 */
	FORCEINLINE void SetFarClipPlane( float InFarClipPlane )
	{
		farClipPlane = InFarClipPlane;
	}

	/**
	 * Set aspect ratio
	 * 
	 * @param[in] InAspectRatio Aspect Ratio (Width/Height)
	 */
	FORCEINLINE void SetAspectRatio( float InAspectRatio )
	{
		aspectRatio = InAspectRatio;
	}

	/**
	 * Get projection mode
	 * @return Return projection mode
	 */
	FORCEINLINE ECameraProjectionMode GetProjectionMode() const
	{
		return projectionMode;
	}

	/**
	 * Get field of view
	 * @return Return field of view
	 */
	FORCEINLINE float GetFieldOfView() const
	{
		return fieldOfView;
	}

	/**
	 * Get width of orthographic projection
	 * @return Return Width of orthographic projection
	 */
	FORCEINLINE float GetOrthoWidth() const
	{
		return orthoWidth;
	}

	/**
	 * Get height of orthographic projection
	 * @return Return height of orthographic projection
	 */
	FORCEINLINE float GetOrthoHeight() const
	{
		return orthoHeight;
	}

	/**
	 * Get near plane distance (in world units)
	 * @return Return near plane distance (in world units)
	 */
	FORCEINLINE float GetNearClipPlane() const
	{
		return nearClipPlane;
	}

	/**
	 * Get far plane distance (in world units)
	 * @return Return far plane distance (in world units)
	 */
	FORCEINLINE float GetFarClipPlane() const
	{
		return farClipPlane;
	}

	/**
	 * Get aspect ratio (Width/Height)
	 * @return Return aspect ratio (Width/Height)
	 */
	FORCEINLINE float GetAspectRatio() const
	{
		return aspectRatio;
	}

	/**
	 * Get camera view
	 * @param[out] OutDesiredView Desired view info
	 */
	void GetCameraView( FCameraView& OutDesiredView );

private:
	ECameraProjectionMode			projectionMode;		/**< Projection mode */
	float							fieldOfView;		/**< The horizontal field of view (in degrees) in perspective mode (ignored in Orthographic mode) */
	float							orthoWidth;			/**< The desired width (in world units) of the orthographic view (ignored in Perspective mode) */
	float							orthoHeight;		/**< The desired height (in world units) of the orthographic view (ignored in Perspective mode) */
	float							nearClipPlane;		/**< The near plane distance (in world units) */
	float							farClipPlane;		/**< The far plane distance (in world units) */
	float							aspectRatio;		/**< Aspect Ratio (Width/Height) (ignored in Orthographic mode) */
};

//
// Serialization
//

FORCEINLINE FArchive operator<<( FArchive& InArchive, ECameraProjectionMode& InValue )
{
	InArchive.Serialize( &InValue, sizeof( InValue ) );
	return InArchive;
}

FORCEINLINE FArchive operator<<( FArchive& InArchive, const ECameraProjectionMode& InValue )
{
	check( InArchive.IsSaving() );
	InArchive.Serialize( ( void* ) &InValue, sizeof( InValue ) );
	return InArchive;
}

#endif // !CAMERACOMPONENT_H