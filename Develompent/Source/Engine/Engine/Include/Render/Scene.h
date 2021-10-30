/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef SCENE_H
#define SCENE_H

#include "Math/Math.h"
#include "Render/CameraTypes.h"
#include "Components/CameraComponent.h"

/**
 * @ingroup Engine
 * A projection from scene space into a 2D screen region
 */
class FSceneView
{
public:
	/**
	 * Constructor
	 */
	FSceneView();

	/**
	 * Set camera view
	 * 
	 * @param[in] InCameraView View info of camera
	 */
	void SetCameraView( const FCameraView& InCameraView );

	/**
	 * Set camera view by camera component
	 * 
	 * @param[in] InCameraComponent Camera component
	 */
	FORCEINLINE void SetCameraView( class LCameraComponent* InCameraComponent )
	{
		check( InCameraComponent );

		FCameraView		cameraView;
		InCameraComponent->GetCameraView( cameraView );
		SetCameraView( cameraView );
	}

	/**
	 * Get camera view
	 * @return Return camera view info
	 */
	FORCEINLINE const FCameraView& GetCameraView() const
	{
		return cameraView;
	}

	/**
	 * Get view matrix
	 * @return Return view matrix
	 */
	FORCEINLINE const FMatrix& GetViewMatrix() const
	{
		return viewMatrix;
	}

	/**
	 * Get projection matrix
	 * @return Return projection matrix
	 */
	FORCEINLINE const FMatrix& GetProjectionMatrix() const
	{
		return projectionMatrix;
	}

	/**
	 * Get View * Projection matrix
	 * @return Return View * Projection matrix
	 */
	FORCEINLINE const FMatrix& GetViewProjectionMatrix() const
	{
		return viewProjectionMatrix;
	}

private:
	FMatrix			viewMatrix;				/**< View matrix */
	FMatrix			projectionMatrix;		/**< Projection matrix */
	FMatrix			viewProjectionMatrix;	/**< View * Projection matrix */
	FCameraView		cameraView;				/**< View info of camera */
};

#endif // !SCENE_H