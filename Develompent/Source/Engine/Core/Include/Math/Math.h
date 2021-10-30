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
 * Typedef glm::vec2
 */
typedef glm::vec2		FVector2D;

/**
 * @ingroup Core
 * Typedef glm::vec3
 */
typedef glm::vec3		FVector;

/**
 * @ingroup Core
 * Typedef glm::vec4
 */
typedef glm::vec4		FVector4D;

/**
 * @ingroup Core
 * Typedef glm::mat4
 */
typedef glm::mat4		FMatrix;

/**
 * @ingroup Core
 * Typedef glm::quat
 */
typedef glm::quat		FQuaternion;

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
	static FORCEINLINE FQuaternion AnglesToQuaternion( float InEulerAngleX, float InEulerAngleY, float InEulerAngleZ )
	{
		return 
			glm::angleAxis( InEulerAngleX, FVector( 1.f, 0.f, 0.f ) ) *
			glm::angleAxis( InEulerAngleY, FVector( 0.f, 1.f, 0.f ) ) *
			glm::angleAxis( InEulerAngleZ, FVector( 0.f, 0.f, 1.f ) );
	}

	/**
	 * @ingroup Core
	 * Convert from angles to quaternion
	 * 
	 * @param[in] InEulerAngles Euler angles
	 */
	static FORCEINLINE FQuaternion AnglesToQuaternion( const glm::vec3& InEulerAngles )
	{
		return AnglesToQuaternion( InEulerAngles.x, InEulerAngles.y, InEulerAngles.z );
	}

	static const FVector			vectorZero;			/**< Zero 3D vector */
	static const FVector			vectorOne;			/**< One 3D vector */
	static const FQuaternion		quaternionZero;		/**< Quaternion zero */
	static const FMatrix			matrixIdentity;		/**< Identity matrix */
};

#endif // !MATH_H