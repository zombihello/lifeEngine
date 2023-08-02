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
#include <gtx/compatibility.hpp>

#include "Math/Axis.h"
#include "System/Archive.h"
#include "Core.h"

// Constants
#undef PI
#undef SMALL_NUMBER
#undef KINDA_SMALL_NUMBER
#undef BIG_NUMBER
#undef EULERS_NUMBER

/**
 * @ingroup Core
 * Macro of PI value
 */
#define PI 					(3.1415926535897932)

/**
 * @ingroup Core
 * Macro of small number
 */
#define SMALL_NUMBER		(1.e-8)

/**
 * @ingroup Core
 * Macro of kinda small number
 */
#define KINDA_SMALL_NUMBER	(1.e-4)

/**
 * @ingroup Core
 * Macro of big number
 */
#define BIG_NUMBER			(3.4e+38f)

/**
 * @ingroup Core
 * Macro of Euler's number
 */
#define EULERS_NUMBER       (2.71828182845904523536)

/**
 * @ingroup Core
 * Typedef glm::vec2
 */
typedef glm::vec2		Vector2D;

/**
 * @ingroup Core
 * Typedef glm::ivec2
 */
typedef glm::ivec2		Vector2DInt;

/**
 * @ingroup Core
 * Typedef glm::vec3
 */
typedef glm::vec3		Vector;

/**
 * @ingroup Core
 * Typedef glm::ivec3
 */
typedef glm::ivec3		VectorInt;

/**
 * @ingroup Core
 * Typedef glm::vec4
 */
typedef glm::vec4		Vector4D;

/**
 * @ingroup Core
 * Typedef glm::ivec4
 */
typedef glm::ivec4		Vector4DInt;

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
	 * Convert from euler angles to quaternion XYZ
	 * 
	 * @param[in] InEulerAngleX Euler angle by X
	 * @param[in] InEulerAngleY Euler angle by Y
	 * @param[in] InEulerAngleZ Euler angle by Z
	 */
	static FORCEINLINE Quaternion AnglesToQuaternionXYZ( float InEulerAngleX, float InEulerAngleY, float InEulerAngleZ )
	{
		return 
			glm::angleAxis( DegreesToRadians( InEulerAngleX ), Vector( 1.f, 0.f, 0.f ) )*
			glm::angleAxis( DegreesToRadians( InEulerAngleY ), Vector( 0.f, 1.f, 0.f ) )*
			glm::angleAxis( DegreesToRadians( InEulerAngleZ ), Vector( 0.f, 0.f, 1.f ) );
	}

	/**
	 * @ingroup Core
	 * Convert from euler angles to quaternion ZXY
	 *
	 * @param[in] InEulerAngleX Euler angle by X
	 * @param[in] InEulerAngleY Euler angle by Y
	 * @param[in] InEulerAngleZ Euler angle by Z
	 */
	static FORCEINLINE Quaternion AnglesToQuaternionZXY( float InEulerAngleX, float InEulerAngleY, float InEulerAngleZ )
	{
		return
			glm::angleAxis( DegreesToRadians( InEulerAngleZ ), Vector( 0.f, 0.f, 1.f ) ) *
			glm::angleAxis( DegreesToRadians( InEulerAngleX ), Vector( 1.f, 0.f, 0.f ) ) *
			glm::angleAxis( DegreesToRadians( InEulerAngleY ), Vector( 0.f, 1.f, 0.f ) );
	}

	/**
	 * @ingroup Core
	 * Convert from euler angles to quaternion ZYX
	 *
	 * @param[in] InEulerAngleX Euler angle by X
	 * @param[in] InEulerAngleY Euler angle by Y
	 * @param[in] InEulerAngleZ Euler angle by Z
	 */
	static FORCEINLINE Quaternion AnglesToQuaternionZYX( float InEulerAngleX, float InEulerAngleY, float InEulerAngleZ )
	{
		return
			glm::angleAxis( DegreesToRadians( InEulerAngleZ ), Vector( 0.f, 0.f, 1.f ) ) *
			glm::angleAxis( DegreesToRadians( InEulerAngleY ), Vector( 0.f, 1.f, 0.f ) ) *
			glm::angleAxis( DegreesToRadians( InEulerAngleX ), Vector( 1.f, 0.f, 0.f ) );
	}

	/**
	 * @ingroup Core
	 * Convert from euler angles to quaternion XYZ
	 * 
	 * @param[in] InEulerAngles Euler angles
	 */
	static FORCEINLINE Quaternion AnglesToQuaternionXYZ( const Vector& InEulerAngles )
	{
		return AnglesToQuaternionXYZ( InEulerAngles.x, InEulerAngles.y, InEulerAngles.z );
	}

	/**
	 * @ingroup Core
	 * Convert from euler angles to quaternion ZXY
	 *
	 * @param[in] InEulerAngles Euler angles
	 */
	static FORCEINLINE Quaternion AnglesToQuaternionZXY( const Vector& InEulerAngles )
	{
		return AnglesToQuaternionZXY( InEulerAngles.x, InEulerAngles.y, InEulerAngles.z );
	}

	/**
	 * @ingroup Core
	 * Convert from euler angles to quaternion ZYX
	 *
	 * @param[in] InEulerAngles Euler angles
	 */
	static FORCEINLINE Quaternion AnglesToQuaternionZYX( const Vector& InEulerAngles )
	{
		return AnglesToQuaternionZYX( InEulerAngles.x, InEulerAngles.y, InEulerAngles.z );
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
		Vector		result = glm::eulerAngles( InQuaternion );
		result.x = RadiansToDegrees( result.x );
		result.y = RadiansToDegrees( result.y );
		result.z = RadiansToDegrees( result.z );
		return result;
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
	 * @brief Inverse quaternion
	 *
	 * @param InQuaternion		Input quaternion
	 * @param OutQuaternion		Output quaternion
	 */
	static FORCEINLINE void InverseQuaternion( const Quaternion& InQuaternion, Quaternion& OutQuaternion )
	{
		OutQuaternion = glm::inverse( InQuaternion );
	}

	/**
	 * @brief Inverse quaternion
	 *
	 * @param InQuaternion		Input quaternion
	 * @return Return inverted quaternion
	 */
	static FORCEINLINE Quaternion InverseQuaternion( const Quaternion& InQuaternion )
	{
		Quaternion		result;
		InverseQuaternion( InQuaternion, result );
		return result;
	}

	/**
	 * @brief Dot product
	 * 
	 * @param InX	Vector X
	 * @param InY	Vector Y
	 * @return Returns the dot product of InX and InY.
	 */
	static FORCEINLINE float DotProduct( const Vector& InX, const Vector& InY )
	{
		return glm::dot( InX, InY );
	}

	/**
	 * @brief LookAt quaternion
	 * 
	 * @param InLookFrom	Look from position
	 * @param InLookTo		Look to position
	 * @param InUp			Up vector
	 * @param InGlobalUp	Global up vector
	 */
	static FORCEINLINE Quaternion LookAtQuatenrion( const Vector& InLookFrom, const Vector& InLookTo, const Vector& InUp, const Vector& InGlobalUp )
	{
		Vector		direction		= InLookTo - InLookFrom;
		float      directionLength	= SMath::LengthVector( direction );
		direction = SMath::NormalizeVector( direction );

		// Check if the direction is valid; Also deals with NaN
		if ( directionLength <= 0.0001f )
		{
			return Quaternion( 1.f, 0.f, 0.f, 0.f );	// Just return identity
		}

		// Is the normal up (nearly) parallel to direction?
		if ( SMath::Abs( SMath::DotProduct( direction, InUp ) ) > 0.9999f )
		{
			// Use alternative up
			return glm::quatLookAt( direction, InGlobalUp ) * glm::angleAxis( SMath::DegreesToRadians( 90.f ), Vector( 1.f, 0.f, 0.f ) );
		}
		else
		{
			return glm::quatLookAt( direction, InUp ) * glm::angleAxis( SMath::DegreesToRadians( 90.f ), Vector( 1.f, 0.f, 0.f ) );
		}
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
	 * @brief Log2
	 *
	 * @param InA	Value whose logarithm is calculated
	 * @return Return the binary logarithm of InA
	 */
	static FORCEINLINE float Log2( float InA )
	{
		return log2( InA );
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

	/**
	 * @brief Computes the absolute value of an number
	 * @param InX	Value
	 * @return Return The absolute value of n (i.e. |n|), if it is representable.
	 */
	static FORCEINLINE float Abs( float InX )
	{
		return abs( InX );
	}

	/**
	 * @brief Distance between vectors
	 * 
	 * @param InVectorA	Vector A
	 * @param InVeftorB	Vector B
	 * @return Return distance between vectors
	 */
	static FORCEINLINE float DistanceVector( const Vector& InVectorA, const Vector& InVectorB )
	{
		return glm::distance( InVectorA, InVectorB );
	}

	/**
	 * @brief Lerp
	 * 
	 * @param InX	Value X
	 * @param InY	Value Y
	 * @param InA	Alpha
	 * @return Return the linear blend of InX and InY using the floating-point value InA
	 */
	static FORCEINLINE float Lerp( float InX, float InY, float InA )
	{
		return glm::lerp( InX, InY, InA );
	}

	/**
	 * @brief Snaps a value to the nearest grid multiple
	 * 
	 * @param InValue	Value
	 * @param InGrid	Grid
	 * @return Return snapped value to the nearest grid multiple
	 */
	static FORCEINLINE float GridSnap( float InValue, float InGrid )
	{
		if ( InGrid == 0.f )
		{
			return InValue;
		}
		else
		{
			return Floor( ( InValue + 0.5f * InGrid ) / InGrid ) * InGrid;
		}
	}

	/**
	 * @brief Snaps a Vector2D to the nearest grid multiple
	 * 
	 * @param InValue	Value
	 * @param InGrid	Grid
	 * @return Return snapped Vector2D to the nearest grid multiple
	 */
	static FORCEINLINE Vector2D GridSnap( const Vector2D& InValue, float InGrid )
	{
		Vector2D	retVec;
		GridSnap( InValue, InGrid, retVec );
		return retVec;
	}

	/**
	 * @brief Snaps a Vector2D to the nearest grid multiple
	 *
	 * @param InValue	Value
	 * @param InGrid	Grid
	 * @param OutValue	Return snapped Vector2D to the nearest grid multiple
	 */
	static FORCEINLINE void GridSnap( const Vector2D& InValue, float InGrid, Vector2D& OutValue )
	{
		OutValue = Vector2D( GridSnap( InValue.x, InGrid ), GridSnap( InValue.y, InGrid ) );
	}

	/**
	 * @brief Snaps a Vector to the nearest grid multiple
	 *
	 * @param InValue	Value
	 * @param InGrid	Grid
	 * @return Return snapped Vector to the nearest grid multiple
	 */
	static FORCEINLINE Vector GridSnap( const Vector& InValue, float InGrid )
	{
		Vector	retVec;
		GridSnap( InValue, InGrid, retVec );
		return retVec;
	}

	/**
	 * @brief Snaps a Vector to the nearest grid multiple
	 *
	 * @param InValue	Value
	 * @param InGrid	Grid
	 * @param OutValue	Return snapped Vector to the nearest grid multiple
	 */
	static FORCEINLINE void GridSnap( const Vector& InValue, float InGrid, Vector& OutValue )
	{
		OutValue = Vector( GridSnap( InValue.x, InGrid ), GridSnap( InValue.y, InGrid ), GridSnap( InValue.z, InGrid ) );
	}

	/**
	 * @brief Snaps a Vector4D to the nearest grid multiple
	 *
	 * @param InValue	Value
	 * @param InGrid	Grid
	 * @return Return snapped Vector4D to the nearest grid multiple
	 */
	static FORCEINLINE Vector4D GridSnap( const Vector4D& InValue, float InGrid )
	{
		Vector4D	retVec;
		GridSnap( InValue, InGrid, retVec );
		return retVec;
	}

	/**
	 * @brief Snaps a Vector4D to the nearest grid multiple
	 *
	 * @param InValue	Value
	 * @param InGrid	Grid
	 * @param OutValue	Return snapped Vector4D to the nearest grid multiple
	 */
	static FORCEINLINE void GridSnap( const Vector4D& InValue, float InGrid, Vector4D& OutValue )
	{
		OutValue = Vector4D( GridSnap( InValue.x, InGrid ), GridSnap( InValue.y, InGrid ), GridSnap( InValue.z, InGrid ), GridSnap( InValue.w, InGrid ) );
	}

	/**
	 * @brief Checks if a floating point number is nearly zero
	 *	
	 * @param InValue			Number to compare
	 * @param InErrorTolerance	Maximum allowed difference for considering InValue as 'nearly zero'
	 * @return Return TRUE if InValue is nearly zero
	 **/
	static FORCEINLINE bool IsNearlyZero( float InValue, float InErrorTolerance = SMALL_NUMBER )
	{
		return Abs( InValue ) < InErrorTolerance;
	}

	/**
	 * @brief Checks if a Vector2D number is nearly zero
	 *
	 * @param InValue			Vector2D to compare
	 * @param InErrorTolerance	Maximum allowed difference for considering InValue as 'nearly zero'
	 * @return Return TRUE if InValue is nearly zero
	 **/
	static FORCEINLINE bool IsNearlyZero( const Vector2D& InValue, float InErrorTolerance = SMALL_NUMBER )
	{
		return	Abs( InValue.x ) < InErrorTolerance &&
				Abs( InValue.y ) < InErrorTolerance;
	}

	/**
	 * @brief Checks if a Vector number is nearly zero
	 *
	 * @param InValue			Vector to compare
	 * @param InErrorTolerance	Maximum allowed difference for considering InValue as 'nearly zero'
	 * @return Return TRUE if InValue is nearly zero
	 **/
	static FORCEINLINE bool IsNearlyZero( const Vector& InValue, float InErrorTolerance = SMALL_NUMBER )
	{
		return	Abs( InValue.x ) < InErrorTolerance &&
				Abs( InValue.y ) < InErrorTolerance &&
				Abs( InValue.z ) < InErrorTolerance;
	}

	/**
	 * @brief Checks if a Vector4D number is nearly zero
	 *
	 * @param InValue			Vector4D to compare
	 * @param InErrorTolerance	Maximum allowed difference for considering InValue as 'nearly zero'
	 * @return Return TRUE if InValue is nearly zero
	 **/
	static FORCEINLINE bool IsNearlyZero( const Vector4D& InValue, float InErrorTolerance = SMALL_NUMBER )
	{
		return	Abs( InValue.x ) < InErrorTolerance &&
				Abs( InValue.y ) < InErrorTolerance &&
				Abs( InValue.z ) < InErrorTolerance &&
				Abs( InValue.w ) < InErrorTolerance;
	}

	static const Vector				vectorZero;			/**< Zero 3D vector */
	static const Vector				vectorOne;			/**< One 3D vector */
	static const Quaternion			quaternionZero;		/**< Quaternion zero */
	static const Matrix				matrixIdentity;		/**< Identity matrix */
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
	Assert( InArchive.IsSaving() );
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
	Assert( InArchive.IsSaving() );
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
	Assert( InArchive.IsSaving() );
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
	Assert( InArchive.IsSaving() );
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
	Assert( InArchive.IsSaving() );
	InArchive.Serialize( ( void* ) &InValue, sizeof( InValue ) );
	return InArchive;
}

#endif // !MATH_H
