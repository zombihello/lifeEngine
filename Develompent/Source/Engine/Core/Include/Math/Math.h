/**
 * @file
 * @addtogroup Core Core
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef MATH_H
#define MATH_H

#include <glm.hpp>
#include <gtx/quaternion.hpp>
#include <gtc/type_ptr.hpp>
#include <gtx/transform.hpp>

#include "Core.h"

/**
 * @ingroup Core
 * Structure for all math helper functions
 */
struct FMath
{
	/**
	 * @ingroup Core
	 * Convert from angles to quaternion
	 * 
	 * @param[in] InEulerAngleX Euler angle by X
	 * @param[in] InEulerAngleY Euler angle by Y
	 * @param[in] InEulerAngleZ Euler angle by Z
	 */
	static FORCEINLINE glm::quat AnglesToQuaternion( float InEulerAngleX, float InEulerAngleY, float InEulerAngleZ )
	{
		return 
			glm::angleAxis( InEulerAngleX, glm::vec3( 1.f, 0.f, 0.f ) ) * 
			glm::angleAxis( InEulerAngleY, glm::vec3( 0.f, 1.f, 0.f ) ) * 
			glm::angleAxis( InEulerAngleZ, glm::vec3( 0.f, 0.f, 1.f ) );
	}

	/**
	 * @ingroup Core
	 * Convert from angles to quaternion
	 * 
	 * @param[in] InEulerAngles Euler angles
	 */
	static FORCEINLINE glm::quat AnglesToQuaternion( const glm::vec3& InEulerAngles )
	{
		return AnglesToQuaternion( InEulerAngles.x, InEulerAngles.y, InEulerAngles.z );
	}

	static const glm::vec3			vectorZero;			/**< Zero 3D vector */
	static const glm::vec3			vectorOne;			/**< One 3D vector */
	static const glm::quat			quaternionZero;		/**< Quaternion zero */
};

#endif // !MATH_H