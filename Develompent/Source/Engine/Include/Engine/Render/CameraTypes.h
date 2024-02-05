/**
 * @file
 * @addtogroup Engine Engine
 *
 * ************************************************************
 *                  This file is part of:
 *                      LIFEENGINE
 *          https://github.com/zombihello/lifeEngine
 * ************************************************************
 * Copyright (C) 2024 Yehor Pohuliaka.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef CAMERATYPES_H
#define CAMERATYPES_H

#include "Core/Math/Math.h"
#include "Core/Misc/Enum.h"

/**
 * @ingroup Engine
 * Enumerations of type projection mode in camera
 */
enum ECameraProjectionMode
{
	CPM_Perspective,		/**< Perspective mode */
	CPM_Orthographic		/**< Orthographic mode */
};

DECLARE_ENUM( ECameraProjectionMode )
#define FOREACH_ENUM_CAMERAPROJECTMODE( X ) \
	X( CPM_Perspective ) \
	X( CPM_Orthographic )

/**
 * @ingroup Engine
 * Struct of camera view info
 */
struct CameraView
{
	Vector						location;		/**< Location */
	Quaternion					rotation;		/**< Rotation */
	ECameraProjectionMode		projectionMode;	/**< Projection mode */
	float						fov;			/**< The horizontal field of view (in degrees) in perspective mode (ignored in orthographic mode) */
	float						orthoWidth;		/**< The desired width (in world units) of the orthographic view (ignored in Perspective mode) */
	float						orthoHeight;	/**< The desired height (in world units) of the orthographic view (ignored in Perspective mode) */
	float						nearClipPlane;	/**< The near plane distance */
	float						farClipPlane;	/**< The far plane distance */
	float						aspectRatio;	/**< Aspect Ratio (Width/Height) */
};

#endif // !CAMERATYPES_H