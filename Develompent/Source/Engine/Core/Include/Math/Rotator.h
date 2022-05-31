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
 * Implements a container for rotation information.
 * All rotation values are stored in degrees
 */
class CRotator
{
public:
	friend CArchive& operator<<( CArchive& InArchive, CRotator& InValue );
	friend CArchive& operator<<( CArchive& InArchive, const CRotator& InValue );

	/**
	 * Constructor
	 */
	FORCEINLINE CRotator() :
		pitch( 0.f ),
		yaw( 0.f ),
		roll( 0.f )
	{}

	/**
	 * Constructor
	 * 
	 * @param[in] InValue Value to set all components to
	 */
	FORCEINLINE CRotator( float InValue ) :
		pitch( InValue ),
		yaw( InValue ),
		roll( InValue )
	{}

	/**
	 * Constructor
	 * 
	 * @param[in] InPitch Pitch in degrees
	 * @param[in] InYaw Yaw in degrees
	 * @param[in] InRoll Roll in degrees
	 */
	FORCEINLINE CRotator( float InPitch, float InYaw, float InRoll ) :
		pitch( InPitch ),
		yaw( InYaw ),
		roll( InRoll )
	{}

	/**
	 * Constructor
	 * 
	 * @param[in] InQuaternion Quaternion
	 */
	FORCEINLINE CRotator( const Quaternion& InQuaternion )
	{
		Vector		eulerAngles = SMath::QuaternionToAngles( InQuaternion );
		pitch	= eulerAngles.x;
		yaw		= eulerAngles.y;
		roll	= eulerAngles.z;
	}

	/**
	 * Constructor with leaving uninitialized memory
	 */
	FORCEINLINE explicit CRotator( ENoInit )
	{}

	/**
	 * Add rotator to rotator
	 * 
	 * @param[in] InRotator Rotator
	 */
	FORCEINLINE void Add( const CRotator& InRotator )
	{
		Add( InRotator.pitch, InRotator.yaw, InRotator.roll );
	}

	/**
	 * Subtract rotator from rotator
	 * 
	 * @param[in] InRotator Rotator
	 */
	FORCEINLINE void Subtract( const CRotator& InRotator )
	{
		Subtract( InRotator.pitch, InRotator.yaw, InRotator.roll );
	}

	/**
	 * Adds to each component of the rotator
	 * 
	 * @param[in] InDeltaPitch Change in pitch. (+/-)
	 * @param[in] InDeltaYaw Change in yaw. (+/-)
	 * @param[in] InDeltaRoll Change in roll. (+/-)
	 */
	FORCEINLINE void Add( float InDeltaPitch, float InDeltaYaw, float InDeltaRoll )
	{
		pitch += InDeltaPitch;
		yaw += InDeltaYaw;
		roll += InDeltaRoll;
	}

	/**
	 * Subtracts from each component of the rotator
	 * 
	 * @param[in] InDeltaPitch Change in pitch. (+/-)
	 * @param[in] InDeltaYaw Change in yaw. (+/-)
	 * @param[in] InDeltaRoll Change in roll. (+/-)
	 */
	FORCEINLINE void Subtract( float InDeltaPitch, float InDeltaYaw, float InDeltaRoll )
	{
		pitch -= InDeltaPitch;
		yaw -= InDeltaYaw;
		roll -= InDeltaRoll;
	}

	/**
	 * Rotate vector
	 * 
	 * @param[in] InVector Vector
	 * @return Return rotated vector
	 */
	FORCEINLINE Vector RotateVector( const Vector& InVector ) const
	{
		return InVector * ToQuaternion();
	}

	/**
	 * Convert to quaternion
	 * @return Return quaternion
	 */
	FORCEINLINE Quaternion ToQuaternion() const
	{
		return SMath::AnglesToQuaternion( SMath::DegreesToRadians( pitch ), SMath::DegreesToRadians( yaw ), SMath::DegreesToRadians( roll ) );
	}

	/**
	 * Convert to Euler angles (in degrees)
	 * @return Return rotation as a Euler angle vector
	 */
	FORCEINLINE Vector ToEuler() const
	{
		return Vector( pitch, yaw, roll );
	}

	/**
	 * Convert to matrix
	 * @return Return rotation as a matrix
	 */
	FORCEINLINE Matrix ToMatrix() const
	{
        return SMath::QuaternionToMatrix( ToQuaternion() );
	}

	/**
	 * Get the result of adding a rotator to this
	 * 
	 * @param[in] InOther The other rotator
	 * @return The result of adding a rotator to this
	 */
	FORCEINLINE CRotator operator+( const CRotator& InOther ) const
	{
		return CRotator( pitch + InOther.pitch, yaw + InOther.yaw, roll + InOther.roll );
	}

	/**
	 * Get the result of subtracting a rotator from this
	 * 
	 * @param[in] InOther The other rotator
	 * @return The result of subtracting a rotator from this
	 */
	FORCEINLINE CRotator operator-( const CRotator& InOther ) const
	{
		return CRotator( pitch - InOther.pitch, yaw - InOther.yaw, roll - InOther.roll );
	}

	/**
	 * Adds another rotator to this
	 * 
	 * @param[in] InOther The other rotator
	 * @return Return self
	 */
	FORCEINLINE CRotator& operator+=( const CRotator& InOther )
	{
		Add( InOther );
		return *this;
	}

	/**
	 * Subtracts another rotator from this
	 * 
	 * @param[in] InOther The other rotator
	 * @return Return self
	 */
	FORCEINLINE CRotator& operator-=( const CRotator& InOther )
	{
		Subtract( InOther );
		return *this;
	}

	/**
	 * Compare two CRotator
	 * 
	 * @param InRight Right rotator
	 * @return Return if two rotators is equal, else return false
	 */
	FORCEINLINE bool operator==( const CRotator& InRight ) const
	{
		return pitch == InRight.pitch &&
			yaw == InRight.yaw &&
			roll == InRight.roll;
	}

	float			pitch;		/**< Rotation around the right axis */
	float			yaw;		/**< Rotation around the up axis */
	float			roll;		/**< Rotation around the forward axis */
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
