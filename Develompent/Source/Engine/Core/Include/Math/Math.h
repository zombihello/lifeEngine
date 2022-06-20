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

#include "System/Archive.h"
#include "Core.h"

// Constants
#undef  PI

/**
 * @ingroup Core
 * Macro of PI value
 */
#define PI 					(3.1415926535897932)

/**
 * @ingroup Core
 * Typedef glm::vec2
 */
typedef glm::vec2		Vector2D;

/**
 * @ingroup Core
 * Typedef glm::vec3
 */
typedef glm::vec3		Vector;

/**
 * @ingroup Core
 * Typedef glm::vec4
 */
typedef glm::vec4		Vector4D;

/**
 * @ingroup Core
 * Typedef glm::mat4
 */
typedef glm::mat4		Matrix;

/**
 * @ingroup Core
 * Typedef glm::quat
 */
typedef glm::quat		Quaternion;

/**
 * @ingroup Core
 * Structure for all math helper functions
 */
struct SMath
{
	/**
	 * @ingroup Core
	 * Convert from euler angles to quaternion
	 * 
	 * @param[in] InEulerAngleX Euler angle by X
	 * @param[in] InEulerAngleY Euler angle by Y
	 * @param[in] InEulerAngleZ Euler angle by Z
	 */
	static FORCEINLINE Quaternion AnglesToQuaternion( float InEulerAngleX, float InEulerAngleY, float InEulerAngleZ )
	{
		return 
			glm::angleAxis( InEulerAngleX, Vector( 1.f, 0.f, 0.f ) ) *
			glm::angleAxis( InEulerAngleY, Vector( 0.f, 1.f, 0.f ) ) *
			glm::angleAxis( InEulerAngleZ, Vector( 0.f, 0.f, 1.f ) );
	}

	/**
	 * @ingroup Core
	 * Convert from euler angles to quaternion
	 * 
	 * @param[in] InEulerAngles Euler angles
	 */
	static FORCEINLINE Quaternion AnglesToQuaternion( const Vector& InEulerAngles )
	{
		return AnglesToQuaternion( InEulerAngles.x, InEulerAngles.y, InEulerAngles.z );
	}

	/**
	 * @ingroup Core
	 * Convert from quaternion to euler angles
	 * 
	 * @param[in] InQuaternion Quaternion
	 * @return Return euler angles in radians
	 */
	static FORCEINLINE Vector QuaternionToAngles( const Quaternion& InQuaternion )
	{
		return glm::eulerAngles( InQuaternion );
	}

    /**
     * @brief Convert from quaternion to matrix
     *
     * @param InQuaternion Quaternion
     * @return Return matrix of ratation
     */
    static FORCEINLINE Matrix QuaternionToMatrix( const Quaternion& InQuaternion )
    {
        return glm::mat4_cast( InQuaternion );
    }

	/**
	 * @ingroup Core
	 * Convert radians to degrees
	 * 
	 * @param[in] InRadians Radians
	 */
	static FORCEINLINE float RadiansToDegrees( float InRadians )
	{
		return glm::degrees( InRadians );
	}

	/**
	 * @ingroup Core
	 * Convert degrees to radians
	 * 
	 * @param[in] InDegrees Degrees
	 */
	static FORCEINLINE float DegreesToRadians( float InDegrees )
	{
		return glm::radians( InDegrees );
	}

	/**
	 * @brief Create identity matrix
	 * @param OutResult Output created identity matrix
	 */
	static FORCEINLINE void IdentityMatrix( Matrix& OutResult )
	{
		OutResult = glm::identity< Matrix >();
	}

	/**
	 * @brief Create identity matrix
	 * @return Return created identity matrix
	 */
	static FORCEINLINE Matrix IdentityMatrix()
	{
		return glm::identity< Matrix >();
	}

	/**
	 * @brief Create translate matrix
	 *
	 * @param InLocation Location
	 * @param OutResult Output created translate matrix
	 */
	static FORCEINLINE void TranslateMatrix( const Vector& InLocation, Matrix& OutResult )
	{
		OutResult[ 3 ].x = InLocation.x;
		OutResult[ 3 ].y = InLocation.y;
		OutResult[ 3 ].z = InLocation.z;
	}

	/**
	 * @brief Translate matrix
	 *
	 * @param InLocation Location
	 * @param InInitMatrix Initial matrix
	 * @param OutResult Output result translated matrix
	 */
	static FORCEINLINE void TranslateMatrix( const Vector& InLocation, const Matrix& InInitMatrix, Matrix& OutResult )
	{
		OutResult = glm::translate( InInitMatrix, InLocation );
	}

    /**
     * @brief Translate matrix
     *
     * @param InLocation Location
	 * @param InInitMatrix Initial matrix
     * @return Return translated matrix
     */
    static FORCEINLINE Matrix TranslateMatrix( const Vector& InLocation, const Matrix& InInitMatrix = Matrix( 1.f ) )
    {
		return glm::translate( InInitMatrix, InLocation );
    }

	/**
	 * @brief Create scale matrix
	 *
	 * @param InScale Scale
	 * @param OutResult Output created scale matrix
	 */
	static FORCEINLINE void ScaleMatrix( const Vector& InScale, Matrix& OutResult )
	{
		OutResult[ 0 ].x = InScale.x;
		OutResult[ 1 ].y = InScale.y;
		OutResult[ 2 ].z = InScale.z;
	}

	/**
	 * @brief Scale matrix
	 *
	 * @param InScale Scale
	 * @param InInitMatrix Initial matrix
	 * @param OutResult Output result scaled matrix
	 */
	static FORCEINLINE void ScaleMatrix( const Vector& InScale, const Matrix& InInitMatrix, Matrix& OutResult )
	{
		OutResult = glm::scale( InInitMatrix, InScale );
	}

    /**
     * @brief Scale matrix
     *
     * @param InScale Scale
	 * @param InInitMatrix Initial matrix
     * @return Return scaled matrix
     */
    static FORCEINLINE Matrix ScaleMatrix( const Vector& InScale, const Matrix& InInitMatrix = Matrix( 1.f ) )
    {
        return glm::scale( InInitMatrix, InScale );
    }

	/**
	 * @brief Inverse matrix
	 * 
	 * @param InMatrix		Input matrix
	 * @param OutMatrix		Output matrix
	 */
	static FORCEINLINE void InverseMatrix( const Matrix& InMatrix, Matrix& OutMatrix )
	{
		OutMatrix = glm::inverse( InMatrix );
	}

	/**
	 * @brief Inverse matrix
	 *
	 * @param InMatrix		Input matrix
	 * @return Return inverted matrix
	 */
	static FORCEINLINE Matrix InverseMatrix( const Matrix& InMatrix )
	{
		Matrix		result;
		InverseMatrix( InMatrix, result );
		return result;
	}

	/**
	 * @brief Get origin from matrix
	 * 
	 * @param InMatric		Input matrix
	 * @param OutOrigin		Output origin
	 */
	static FORCEINLINE void GetOriginMatrix( const Matrix& InMatrix, Vector& OutOrigin )
	{
		OutOrigin.x = InMatrix[ 3 ].x;
		OutOrigin.y = InMatrix[ 3 ].y;
		OutOrigin.z = InMatrix[ 3 ].z;
	}

	/**
	 * @brief Get origin from matrix
	 *
	 * @param InMatric		Input matrix
	 * @return Return origin from matrix
	 */
	static FORCEINLINE Vector GetOriginMatrix( const Matrix& InMatrix )
	{
		Vector		result;
		GetOriginMatrix( InMatrix, result );
		return result;
	}

	/**
	 * @brief Normalize vector
	 * 
	 * @param InVector Vector
	 * @return Return normalized vector
	 */
	static FORCEINLINE Vector2D NormalizeVector( const Vector2D& InVector )
	{
		return glm::normalize( InVector );
	}

	/**
	 * @brief Normalize vector
	 *
	 * @param InVector Vector
	 * @return Return normalized vector
	 */
	static FORCEINLINE Vector NormalizeVector( const Vector& InVector )
	{
		return glm::normalize( InVector );
	}

	/**
	 * @brief Normalize vector
	 *
	 * @param InVector Vector
	 * @return Return normalized vector
	 */
	static FORCEINLINE Vector4D NormalizeVector( const Vector4D& InVector )
	{
		return glm::normalize( InVector );
	}

	/**
	 * @brie Length vector
	 * 
	 * @param InVector Vector
	 * @return Return length vector
	 */
	static FORCEINLINE float LengthVector( const Vector2D& InVector )
	{
		return glm::length( InVector );
	}

	/**
	 * @brie Length vector
	 *
	 * @param InVector Vector
	 * @return Return length vector
	 */
	static FORCEINLINE float LengthVector( const Vector& InVector )
	{
		return glm::length( InVector );
	}

	/**
	 * @brie Length vector
	 *
	 * @param InVector Vector
	 * @return Return length vector
	 */
	static FORCEINLINE float LengthVector( const Vector4D& InVector )
	{
		return glm::length( InVector );
	}

	/**
	 * @brief Cross vector
	 *
	 * @param InVectorA		Vector A
	 * @param InVectorB		Vector B
	 * @param OutVector		Output vector
	 */
	static FORCEINLINE void CrossVector( const Vector& InVectorA, const Vector& InVectorB, Vector& OutVector )
	{
		OutVector = glm::cross( InVectorA, InVectorB );
	}

	/**
	 * @brief Cross vector
	 *
	 * @param InVectorA		Vector A
	 * @param InVectorB		Vector B
	 * @return Return cross vector
	 */
	static FORCEINLINE Vector CrossVector( const Vector& InVectorA, const Vector& InVectorB )
	{
		Vector		result;
		CrossVector( InVectorA, InVectorB, result );
		return result;
	}

	/**
	 * @brief Pow
	 * 
	 * @param InA Value A
	 * @param InB Value B
	 * @return Return A to the extent  B
	 */
	static FORCEINLINE float Pow( float InA, float InB )
	{
		return powf( InA, InB );
	}

	/**
	 * @brief Converts number with truncation towards zero
	 * 
	 * @param InA		Value
	 * @return Rounds InA toward zero, returning the nearest integral value that is not larger in magnitude than InA
	 */
	static FORCEINLINE float Trunc( float InA )
	{
		return truncf( InA );
	}

	/**
	 * @brief Round to nearest a floating point number
	 * 
	 * @param InA		Value
	 * @return Returns the integral value that is nearest to InA, with halfway cases rounded away from zero
	 */
	static FORCEINLINE float Round( float InA )
	{
		return roundf( InA );
	}

	/**
	 * @brief Sin
	 * @param InA	Value representing an angle expressed in radians
	 * @return Returns the sine of an angle of InA radians
	 */
	static FORCEINLINE float Sin( float InA )
	{
		return sin( InA );
	}

	/**
	 * @brief Cos
	 * @param InA	Value representing an angle expressed in radians
	 * @return Returns the cosine of an angle of x radians
	 */
	static FORCEINLINE float Cos( float InA )
	{
		return cos( InA );
	}

	/**
	 * @brief ATan
	 * @param InA		Value whose arc tangent is computed
	 * @return Principal arc tangent of InA, in the interval [-pi/2,+pi/2] radians
	 */
	static FORCEINLINE float ATan( float InA )
	{
		return atanf( InA );
	}

	/**
	 * @brief Sqrt
	 * @param InA		Value whose square root is computed
	 * @return Square root of InA
	 */
	static FORCEINLINE float Sqrt( float InA )
	{
		return sqrtf( InA );
	}

	/**
	 * @brief Floor
	 * 
	 * @param InA	Value to round down
	 * @return Return the value of InA rounded downward (as a floating-point value)
	 */
	static FORCEINLINE float Floor( float InA )
	{
		return floorf( InA );
	}

	/**
	 * @brief Clamp value in range InMin and InMax
	 *
	 * @param InX Value
	 * @param InMin Min range
	 * @param InMax Max range
	 * @return Return clamped value
	 */
	static FORCEINLINE float Clamp( float InX, float InMin, float InMax )
	{
		return ::Clamp( InX, InMin, InMax );
	}

	static const Vector				vectorZero;			/**< Zero 3D vector */
	static const Vector				vectorOne;			/**< One 3D vector */
	static const Quaternion			quaternionZero;		/**< Quaternion zero */
	static const Matrix				matrixIdentity;		/**< Identity matrix */
	static const class CRotator		rotatorZero;		/**< A rotator of zero degrees on each axis */
	static const struct CTransform	transformZero;		/**< Transform zero */
	static const Vector				vectorForward;		/**< Forward vector */
	static const Vector				vectorRight;		/**< Right vector */
	static const Vector				vectorUp;			/**< Up vector */
};

//
// Serialization
//

FORCEINLINE CArchive& operator<<( CArchive& InArchive, Vector2D& InValue )
{
	InArchive.Serialize( &InValue, sizeof( InValue ) );
	return InArchive;
}

FORCEINLINE CArchive& operator<<( CArchive& InArchive, const Vector2D& InValue )
{
	check( InArchive.IsSaving() );
	InArchive.Serialize( ( void* ) &InValue, sizeof( InValue ) );
	return InArchive;
}

FORCEINLINE CArchive& operator<<( CArchive& InArchive, Vector& InValue )
{
	InArchive.Serialize( &InValue, sizeof( InValue ) );
	return InArchive;
}

FORCEINLINE CArchive& operator<<( CArchive& InArchive, const Vector& InValue )
{
	check( InArchive.IsSaving() );
	InArchive.Serialize( ( void* ) &InValue, sizeof( InValue ) );
	return InArchive;
}

FORCEINLINE CArchive& operator<<( CArchive& InArchive, Vector4D& InValue )
{
	InArchive.Serialize( &InValue, sizeof( InValue ) );
	return InArchive;
}

FORCEINLINE CArchive& operator<<( CArchive& InArchive, const Vector4D& InValue )
{
	check( InArchive.IsSaving() );
	InArchive.Serialize( ( void* ) &InValue, sizeof( InValue ) );
	return InArchive;
}

FORCEINLINE CArchive& operator<<( CArchive& InArchive, Matrix& InValue )
{
	InArchive.Serialize( &InValue, sizeof( InValue ) );
	return InArchive;
}

FORCEINLINE CArchive& operator<<( CArchive& InArchive, const Matrix& InValue )
{
	check( InArchive.IsSaving() );
	InArchive.Serialize( ( void* ) &InValue, sizeof( InValue ) );
	return InArchive;
}

FORCEINLINE CArchive& operator<<( CArchive& InArchive, Quaternion& InValue )
{
	InArchive.Serialize( &InValue, sizeof( InValue ) );
	return InArchive;
}

FORCEINLINE CArchive& operator<<( CArchive& InArchive, const Quaternion& InValue )
{
	check( InArchive.IsSaving() );
	InArchive.Serialize( ( void* ) &InValue, sizeof( InValue ) );
	return InArchive;
}

#endif // !MATH_H
