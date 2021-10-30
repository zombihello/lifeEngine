/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef CAMERATYPES_H
#define CAMERATYPES_H

#include "Math/Math.h"

/**
 * @ingroup Engine
 * Enumerations of type projection mode in camera
 */
enum ECameraProjectionMode
{
	CPM_Perspective,		/**< Perspective mode */
	CPM_Orthographic		/**< Orthographic mode */
};

/**
 * @ingroup Engine
 * Struct of camera view info
 */
struct FCameraView
{
	FVector						location;		/**< Location */
	FQuaternion					rotation;		/**< Rotation */
	ECameraProjectionMode		projectionMode;	/**< Projection mode */
	float						fov;			/**< The horizontal field of view (in degrees) in perspective mode (ignored in orthographic mode) */
	float						orthoWidth;		/**< The desired width (in world units) of the orthographic view (ignored in Perspective mode) */
	float						orthoHeight;	/**< The desired height (in world units) of the orthographic view (ignored in Perspective mode) */
	float						nearClipPlane;	/**< The near plane distance */
	float						farClipPlane;	/**< The far plane distance */
	float						aspectRatio;	/**< Aspect Ratio (Width/Height) */
};

#endif // !CAMERATYPES_H