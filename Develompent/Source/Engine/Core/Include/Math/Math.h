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
	 * Convert from euler angles to quaternion
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
	 * Convert from euler angles to quaternion
	 * 
	 * @param[in] InEulerAngles Euler angles
	 */
	static FORCEINLINE FQuaternion AnglesToQuaternion( const FVector& InEulerAngles )
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
	static FORCEINLINE FVector QuaternionToAngles( const FQuaternion& InQuaternion )
	{
		return glm::eulerAngles( InQuaternion );
	}

    /**
     * @brief Convert from quaternion to matrix
     *
     * @param InQuaternion Quaternion
     * @return Return matrix of ratation
     */
    static FORCEINLINE FMatrix QuaternionToMatrix( const FQuaternion& InQuaternion )
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
	static FORCEINLINE void IdentityMatrix( FMatrix& OutResult )
	{
		OutResult = glm::identity< FMatrix >();
	}

	/**
	 * @brief Create identity matrix
	 * @return Return created identity matrix
	 */
	static FORCEINLINE FMatrix IdentityMatrix()
	{
		return glm::identity< FMatrix >();
	}

	/**
	 * @brief Create translate matrix
	 *
	 * @param InLocation Location
	 * @param OutResult Output created translate matrix
	 */
	static FORCEINLINE void TranslateMatrix( const FVector& InLocation, FMatrix& OutResult )
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
	static FORCEINLINE void TranslateMatrix( const FVector& InLocation, const FMatrix& InInitMatrix, FMatrix& OutResult )
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
    static FORCEINLINE FMatrix TranslateMatrix( const FVector& InLocation, const FMatrix& InInitMatrix = FMatrix( 1.f ) )
    {
		return glm::translate( InInitMatrix, InLocation );
    }

	/**
	 * @brief Create scale matrix
	 *
	 * @param InScale Scale
	 * @param OutResult Output created scale matrix
	 */
	static FORCEINLINE void ScaleMatrix( const FVector& InScale, FMatrix& OutResult )
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
	static FORCEINLINE void ScaleMatrix( const FVector& InScale, const FMatrix& InInitMatrix, FMatrix& OutResult )
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
    static FORCEINLINE FMatrix ScaleMatrix( const FVector& InScale, const FMatrix& InInitMatrix = FMatrix( 1.f ) )
    {
        return glm::scale( InInitMatrix, InScale );
    }

	/**
	 * @brief Inverse matrix
	 * 
	 * @param InMatrix		Input matrix
	 * @param OutMatrix		Output matrix
	 */
	static FORCEINLINE void InverseMatrix( const FMatrix& InMatrix, FMatrix& OutMatrix )
	{
		OutMatrix = glm::inverse( InMatrix );
	}

	/**
	 * @brief Inverse matrix
	 *
	 * @param InMatrix		Input matrix
	 * @return Return inverted matrix
	 */
	static FORCEINLINE FMatrix InverseMatrix( const FMatrix& InMatrix )
	{
		FMatrix		result;
		InverseMatrix( InMatrix, result );
		return result;
	}

	/**
	 * @brief Get origin from matrix
	 * 
	 * @param InMatric		Input matrix
	 * @param OutOrigin		Output origin
	 */
	static FORCEINLINE void GetOriginMatrix( const FMatrix& InMatrix, FVector& OutOrigin )
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
	static FORCEINLINE FVector GetOriginMatrix( const FMatrix& InMatrix )
	{
		FVector		result;
		GetOriginMatrix( InMatrix, result );
		return result;
	}

	/**
	 * @brief Normalize vector
	 * 
	 * @param InVector Vector
	 * @return Return normalized vector
	 */
	static FORCEINLINE FVector2D NormalizeVector( const FVector2D& InVector )
	{
		return glm::normalize( InVector );
	}

	/**
	 * @brief Normalize vector
	 *
	 * @param InVector Vector
	 * @return Return normalized vector
	 */
	static FORCEINLINE FVector NormalizeVector( const FVector& InVector )
	{
		return glm::normalize( InVector );
	}

	/**
	 * @brief Normalize vector
	 *
	 * @param InVector Vector
	 * @return Return normalized vector
	 */
	static FORCEINLINE FVector4D NormalizeVector( const FVector4D& InVector )
	{
		return glm::normalize( InVector );
	}

	/**
	 * @brie Length vector
	 * 
	 * @param InVector Vector
	 * @return Return length vector
	 */
	static FORCEINLINE float LengthVector( const FVector2D& InVector )
	{
		return glm::length( InVector );
	}

	/**
	 * @brie Length vector
	 *
	 * @param InVector Vector
	 * @return Return length vector
	 */
	static FORCEINLINE float LengthVector( const FVector& InVector )
	{
		return glm::length( InVector );
	}

	/**
	 * @brie Length vector
	 *
	 * @param InVector Vector
	 * @return Return length vector
	 */
	static FORCEINLINE float LengthVector( const FVector4D& InVector )
	{
		return glm::length( InVector );
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

	static const FVector				vectorZero;			/**< Zero 3D vector */
	static const FVector				vectorOne;			/**< One 3D vector */
	static const FQuaternion			quaternionZero;		/**< Quaternion zero */
	static const FMatrix				matrixIdentity;		/**< Identity matrix */
	static const class FRotator			rotatorZero;		/**< A rotator of zero degrees on each axis */
	static const struct FTransform		transformZero;		/**< Transform zero */
	static const FVector				vectorForward;		/**< Forward vector */
	static const FVector				vectorRight;		/**< Right vector */
	static const FVector				vectorUp;			/**< Up vector */
};

//
// Serialization
//

FORCEINLINE FArchive& operator<<( FArchive& InArchive, FVector2D& InValue )
{
	InArchive.Serialize( &InValue, sizeof( InValue ) );
	return InArchive;
}

FORCEINLINE FArchive& operator<<( FArchive& InArchive, const FVector2D& InValue )
{
	check( InArchive.IsSaving() );
	InArchive.Serialize( ( void* ) &InValue, sizeof( InValue ) );
	return InArchive;
}

FORCEINLINE FArchive& operator<<( FArchive& InArchive, FVector& InValue )
{
	InArchive.Serialize( &InValue, sizeof( InValue ) );
	return InArchive;
}

FORCEINLINE FArchive& operator<<( FArchive& InArchive, const FVector& InValue )
{
	check( InArchive.IsSaving() );
	InArchive.Serialize( ( void* ) &InValue, sizeof( InValue ) );
	return InArchive;
}

FORCEINLINE FArchive& operator<<( FArchive& InArchive, FVector4D& InValue )
{
	InArchive.Serialize( &InValue, sizeof( InValue ) );
	return InArchive;
}

FORCEINLINE FArchive& operator<<( FArchive& InArchive, const FVector4D& InValue )
{
	check( InArchive.IsSaving() );
	InArchive.Serialize( ( void* ) &InValue, sizeof( InValue ) );
	return InArchive;
}

FORCEINLINE FArchive& operator<<( FArchive& InArchive, FMatrix& InValue )
{
	InArchive.Serialize( &InValue, sizeof( InValue ) );
	return InArchive;
}

FORCEINLINE FArchive& operator<<( FArchive& InArchive, const FMatrix& InValue )
{
	check( InArchive.IsSaving() );
	InArchive.Serialize( ( void* ) &InValue, sizeof( InValue ) );
	return InArchive;
}

FORCEINLINE FArchive& operator<<( FArchive& InArchive, FQuaternion& InValue )
{
	InArchive.Serialize( &InValue, sizeof( InValue ) );
	return InArchive;
}

FORCEINLINE FArchive& operator<<( FArchive& InArchive, const FQuaternion& InValue )
{
	check( InArchive.IsSaving() );
	InArchive.Serialize( ( void* ) &InValue, sizeof( InValue ) );
	return InArchive;
}

#endif // !MATH_H
