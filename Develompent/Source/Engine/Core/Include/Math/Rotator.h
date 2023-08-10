/**
 * @file
 * @addtogroup Core Core
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef ROTATOR_H
#define ROTATOR_H

#include "Math/Math.h"

 /**
  * @ingroup Core
  * @brief Implements a container for rotation information.
  * All rotation values are stored in degrees
  */
class CRotator
{
public:
	friend CArchive& operator<<( CArchive& InArchive, CRotator& InValue );
	friend CArchive& operator<<( CArchive& InArchive, const CRotator& InValue );

	/**
	 * @brief Constructor
	 */
	FORCEINLINE CRotator() :
		pitch( 0.f ),
		yaw( 0.f ),
		roll( 0.f )
	{}

	/**
	 * @brief Constructor
	 *
	 * @param InValue	Value to set all components to
	 */
	FORCEINLINE CRotator( float InValue ) :
		pitch( InValue ),
		yaw( InValue ),
		roll( InValue )
	{}

	/**
	 * @brief Constructor
	 *
	 * @param InPitch	Pitch in degrees
	 * @param InYaw		Yaw in degrees
	 * @param InRoll	Roll in degrees
	 */
	FORCEINLINE CRotator( float InPitch, float InYaw, float InRoll ) :
		pitch( InPitch ),
		yaw( InYaw ),
		roll( InRoll )
	{}

	/**
	 * @brief Constructor
	 *
	 * @param InEulerAngles		Euler angles (in degrees)
	 */
	FORCEINLINE CRotator( const Vector& InEulerAngles ) :
		pitch( InEulerAngles.x ),
		yaw( InEulerAngles.y ),
		roll( InEulerAngles.z )
	{}

	/**
	 * @brief Constructor
	 *
	 * @param InQuaternion	Quaternion
	 */
	FORCEINLINE CRotator( const Quaternion& InQuaternion )
	{
		Vector	eulerAngles = SMath::QuaternionToAngles( InQuaternion );
		pitch	= eulerAngles.x;
		yaw		= eulerAngles.y;
		roll	= eulerAngles.z;
	}

	/**
	 * @brief Constructor with leaving uninitialized memory
	 */
	FORCEINLINE explicit CRotator( ENoInit )
	{}

	/**
	 * @brief Checks whether rotator is nearly zero within specified tolerance, when treated as an orientation
	 * This means that CRotator(0, 0, 360) is "zero", because it is the same final orientation as the zero rotator
	 *
	 * @param InTolerance	Error tolerance
	 * @return Return TRUE if rotator is nearly zero, within specified tolerance, otherwise FALSE
	 */
	FORCEINLINE bool IsNearlyZero( float InTolerance = KINDA_SMALL_NUMBER ) const
	{
		return	SMath::Abs( NormalizeAxis( pitch ) ) <= InTolerance &&
				SMath::Abs( NormalizeAxis( yaw ) ) <= InTolerance &&
				SMath::Abs( NormalizeAxis( roll ) ) <= InTolerance;
	}

	/**
	 * @brief Checks whether this has exactly zero rotation, when treated as an orientation
	 * This means that CRotator(0, 0, 360) is "zero", because it is the same final orientation as the zero rotator
	 *
	 * @return Return TRUE if this has exactly zero rotation, otherwise FALSE
	 */
	FORCEINLINE bool IsZero() const
	{
		return ClampAxis( pitch ) == 0.f && ClampAxis( yaw ) == 0.f && ClampAxis( roll ) == 0.f;
	}

	/**
	 * @brief Checks whether two rotators are equal within specified tolerance, when treated as an orientation
	 * This means that CRotator(0, 0, 360).Equals(CRotator(0,0,0)) is true, because they represent the same final orientation
	 *
	 * @param InOther		The other rotator
	 * @param InTolerance	Error Tolerance
	 * @return Return TRUE if two rotators are equal, within specified tolerance, otherwise FALSE
	 */
	FORCEINLINE bool Equals( const CRotator& InOther, float InTolerance = KINDA_SMALL_NUMBER ) const
	{
		return	SMath::Abs( NormalizeAxis( pitch - InOther.pitch ) ) <= InTolerance &&
				SMath::Abs( NormalizeAxis( yaw - InOther.yaw ) ) <= InTolerance &&
				SMath::Abs( NormalizeAxis( roll - InOther.roll ) ) <= InTolerance;
	}

	/**
	 * @brief Adds to each component of the rotator
	 * 
	 * @param InDeltaPitch		Change in pitch (+/-)
	 * @param InDeltaYaw		Change in yaw (+/-)
	 * @param InDeltaRoll		Change in roll (+/-)
	 * @return Return copy of rotator after addition
	 */
	FORCEINLINE CRotator Add( float InDeltaPitch, float InDeltaYaw, float InDeltaRoll )
	{
		pitch	+= InDeltaPitch;
		yaw		+= InDeltaYaw;
		roll	+= InDeltaRoll;
		return *this;
	}

	/**
	 * @brief Get the inverse of the rotator
	 * @return Return the inverse of the rotator
	 */
	FORCEINLINE CRotator GetInverse() const
	{
		return CRotator( SMath::QuaternionToAngles( SMath::InverseQuaternion( ToQuaternion() ) ) );
	}

	/**
	 * @brief Convert to quaternion
	 * @return Return quaternion
	 */
	FORCEINLINE Quaternion ToQuaternion() const
	{
		return SMath::AnglesToQuaternion( pitch, yaw, roll );
	}

	/**
	 * @brief Convert to Euler angles (in degrees)
	 * @return Return rotation as a Euler angle vector
	 */
	FORCEINLINE Vector ToEuler() const
	{
		return Vector( pitch, yaw, roll );
	}

	/**
	 * @brief Rotate vector
	 *
	 * @param InVector	Vector
	 * @return Return rotated vector
	 */
	FORCEINLINE Vector RotateVector( const Vector& InVector ) const
	{
		return ToQuaternion() * InVector;
	}

	/**
	 * @brief Gets the rotation values so they fall within the range [0,360]
	 *
	 * @return Return clamped version of rotator
	 */
	FORCEINLINE CRotator Clamp() const
	{
		return CRotator( ClampAxis( pitch ), ClampAxis( yaw ), ClampAxis( roll ) );
	}

	/**
	 * @brief Create a copy of this rotator and normalize, removes all winding and creates the "shortest route" rotation
	 *
	 * @return Return normalized copy of this rotator
	 */
	FORCEINLINE CRotator GetNormalized() const
	{
		CRotator	rotator = *this;
		rotator.Normalize();
		return rotator;
	}

	/**
	 * @brief Create a copy of this rotator and denormalize, clamping each axis to 0 - 360
	 *
	 * @return Return denormalized copy of this rotator
	 */
	FORCEINLINE CRotator GetDenormalized() const
	{
		CRotator		rotator = *this;
		rotator.pitch	= ClampAxis( rotator.pitch );
		rotator.yaw		= ClampAxis( rotator.yaw );
		rotator.roll	= ClampAxis( rotator.roll );
		return rotator;
	}

	/**
	 * @brief In-place normalize, removes all winding and creates the "shortest route" rotation
	 */
	FORCEINLINE void Normalize()
	{
		pitch	= NormalizeAxis( pitch );
		yaw		= NormalizeAxis( yaw );
		roll	= NormalizeAxis( roll );
	}

	/**
	 * @brief Clamps an angle to the range of [0, 360)
	 *
	 * @param InAngle	The angle to clamp
	 * @return Return the clamped angle
	 */
	static FORCEINLINE float ClampAxis( float InAngle )
	{
		// Returns Angle in the range (-360,360)
		InAngle = SMath::Fmod( InAngle, 360.f );
		if ( InAngle < 0.f )
		{
			// Shift to [0,360) range
			InAngle += 360.f;
		}

		return InAngle;
	}

	/**
	 * @brief Clamps an angle to the range of (-180, 180]
	 *
	 * @param InAngle	The Angle to clamp
	 * @return Return the clamped angle
	 */
	static FORCEINLINE float NormalizeAxis( float InAngle )
	{
		// Returns Angle in the range [0,360)
		InAngle = ClampAxis( InAngle );
		if ( InAngle > 180.f )
		{
			// Shift to (-180,180]
			InAngle -= 360.f;
		}

		return InAngle;
	}

	/**
	 * @brief Convert a vector of floating-point Euler angles (in degrees) into a rotator. Rotator now stored in degrees
	 *
	 * @param InEuler	Euler angle vector
	 * @return Return a rotator from a Euler angle
	 */
	static FORCEINLINE CRotator MakeFromEuler( const Vector& InEuler )
	{
		return CRotator( InEuler.x, InEuler.y, InEuler.z );
	}

	/**
	 * @brief Convert a vector of floating-point Euler angles (in degrees) into a rotator. Rotator now stored in degrees
	 *
	 * @param InEulerX	Euler angle X
	 * @param InEulerY	Euler angle Y
	 * @param InEulerZ	Euler angle Z
	 * @return Return a rotator from a Euler angle
	 */
	static FORCEINLINE CRotator MakeFromEuler( float InEulerX, float InEulerY, float InEulerZ )
	{
		return CRotator( InEulerX, InEulerY, InEulerZ );
	}

	/**
	 * @brief Get the result of adding a rotator to this
	 *
	 * @param InOther	The other rotator
	 * @return Return the result of adding a rotator to this
	 */
	FORCEINLINE CRotator operator+( const CRotator& InOther ) const
	{
		return CRotator( pitch + InOther.pitch, yaw + InOther.yaw, roll + InOther.roll );
	}

	/**
	 * @brief Get the result of subtracting a rotator from this
	 *
	 * @param InOther	The other rotator
	 * @return Return the result of subtracting a rotator from this
	 */
	FORCEINLINE CRotator operator-( const CRotator& InOther ) const
	{
		return CRotator( pitch - InOther.pitch, yaw - InOther.yaw, roll - InOther.roll );
	}

	/**
	 * @brief Checks whether two rotators are identical. This checks each component for exact equality
	 *
	 * @param InOther	The other rotator
	 * @return true if two rotators are identical, otherwise false
	 */
	FORCEINLINE bool operator==( const CRotator& InOther ) const
	{
		return pitch == InOther.pitch && yaw == InOther.yaw && roll == InOther.roll;
	}

	/**
	 * @brief Checks whether two rotators are different
	 *
	 * @param InOther	The other rotator
	 * @return Return TRUE if two rotators are different, otherwise FALSE
	 */
	FORCEINLINE bool operator!=( const CRotator& InOther ) const
	{
		return pitch != InOther.pitch || yaw != InOther.yaw || roll != InOther.roll;
	}

	/**
	 * @brief Adds another rotator to this
	 *
	 * @param InOther	The other rotator
	 * @return Return copy of rotator after addition
	 */
	FORCEINLINE CRotator operator+=( const CRotator& InOther )
	{
		pitch	+= InOther.pitch;
		yaw		+= InOther.yaw;
		roll	+= InOther.roll;
		return *this;
	}

	/**
	 * @brief Subtracts another rotator from this
	 *
	 * @param InOther	The other rotator
	 * @return Return copy of rotator after subtraction
	 */
	FORCEINLINE CRotator operator-=( const CRotator& InOther )
	{
		pitch	-= InOther.pitch;
		yaw		-= InOther.yaw;
		roll	-= InOther.roll;
		return *this;
	}

	float	pitch;		/**< Rotation around the right axis */
	float	yaw;		/**< Rotation around the up axis */
	float	roll;		/**< Rotation around the forward axis */
};

/**
  * @ingroup Core
  * @brief Struct for caching Quat<->Rotator conversions
 */
struct SRotationConversionCache
{
	/**
	 * @brief Constructor
	 */
	FORCEINLINE SRotationConversionCache()
		: cachedQuat( SMath::quaternionZero )
		, cachedRotator( SMath::rotatorZero )
	{}

	/**
	 * @brief Convert a CRotator to Quaternion 
	 * Uses the cached conversion if possible, and updates it if there was no match
	 * 
	 * @param InRotator		Rotator
	 * @return Return converted rotator to quaternion
	 */
	FORCEINLINE Quaternion RotatorToQuat( const CRotator& InRotator ) const
	{
		if ( cachedRotator != InRotator )
		{
			cachedRotator	= InRotator.GetNormalized();
			cachedQuat		= cachedRotator.ToQuaternion();
		}
		return cachedQuat;
	}

	/**
	 * @brief Convert a CRotator to Quaternion
	 * Uses the cached conversion if possible, but does *NOT* update the cache if there was no match
	 * 
	 * @param InRotator		Rotator
	 * @return Return converted rotator to quaternion
	 */
	FORCEINLINE Quaternion RotatorToQuat_ReadOnly( const CRotator& InRotator ) const
	{
		if ( cachedRotator == InRotator )
		{
			return cachedQuat;
		}
		return InRotator.ToQuaternion();
	}

	/**
	 * @brief Convert a Quaternion to CRotator
	 * Uses the cached conversion if possible, and updates it if there was no match
	 *
	 * @param InQuat		Quaternion
	 * @return Return converted quaternion to rotator
	 */
	FORCEINLINE CRotator QuatToRotator( const Quaternion& InQuat ) const
	{
		if ( cachedQuat != InQuat )
		{
			cachedQuat		= SMath::NormalizeQuaternion( InQuat );
			cachedRotator	= CRotator( SMath::QuaternionToAngles( cachedQuat ) );
		}
		return cachedRotator;
	}

	/**
	 * @brief Convert a Quaternion to CRotator
	 * Uses the cached conversion if possible, but does *NOT* update the cache if there was no match
	 *
	 * @param InQuat		Quaternion
	 * @return Return converted quaternion to rotator
	 */
	FORCEINLINE CRotator QuatToRotator_ReadOnly( const Quaternion& InQuat ) const
	{
		if ( cachedQuat == InQuat )
		{
			return cachedRotator;
		}
		return CRotator( SMath::QuaternionToAngles( SMath::NormalizeQuaternion( InQuat ) ) );
	}

	/**
	 * @brief Version of QuatToRotator when the Quat is known to already be normalized
	 *
	 * @param InNormalizedQuat		Normalized quaternion
	 * @return Return converted quaternion to rotator
	 */
	FORCEINLINE CRotator NormalizedQuatToRotator( const Quaternion& InNormalizedQuat ) const
	{
		if ( cachedQuat != InNormalizedQuat )
		{
			cachedQuat		= InNormalizedQuat;
			cachedRotator	= CRotator( SMath::QuaternionToAngles( InNormalizedQuat ) );
		}
		return cachedRotator;
	}

	/**
	 * @brief Version of QuatToRotator when the Quat is known to already be normalized
	 * Does *NOT* update the cache if there was no match
	 *
	 * @param InNormalizedQuat		Normalized quaternion
	 * @return Return converted quaternion to rotator
	 */
	FORCEINLINE CRotator NormalizedQuatToRotator_ReadOnly( const Quaternion& InNormalizedQuat ) const
	{
		if ( cachedQuat == InNormalizedQuat )
		{
			return cachedRotator;
		}
		return CRotator( SMath::QuaternionToAngles( InNormalizedQuat ) );
	}

	/**
	 * @brief Get cached quaternion
	 * @return Return cached quaternion
	 */
	FORCEINLINE Quaternion GetCachedQuat() const
	{
		return cachedQuat;
	}

	/**
	 * @brief Get cached rotator
	 * @return Return cached rotator
	 */
	FORCEINLINE CRotator GetCachedRotator() const
	{
		return cachedRotator;
	}

private:
	mutable Quaternion	cachedQuat;		/**< Quaternion matching */
	mutable CRotator	cachedRotator;	/**< CRotator matching */
};

//
// Serialization
//

FORCEINLINE CArchive& operator<<( CArchive& InArchive, CRotator& InValue )
{
	InArchive << InValue.pitch;
	InArchive << InValue.yaw;
	InArchive << InValue.roll;
	return InArchive;
}

FORCEINLINE CArchive& operator<<( CArchive& InArchive, const CRotator& InValue )
{
	InArchive << InValue.pitch;
	InArchive << InValue.yaw;
	InArchive << InValue.roll;
	return InArchive;
}

#endif // !ROTATOR_H