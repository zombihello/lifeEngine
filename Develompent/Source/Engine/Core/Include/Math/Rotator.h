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
class FRotator
{
public:
	friend FArchive& operator<<( FArchive& InArchive, FRotator& InValue );
	friend FArchive& operator<<( FArchive& InArchive, const FRotator& InValue );

	/**
	 * Constructor
	 */
	FORCEINLINE FRotator() :
		pitch( 0.f ),
		yaw( 0.f ),
		roll( 0.f )
	{}

	/**
	 * Constructor
	 * 
	 * @param[in] InValue Value to set all components to
	 */
	FORCEINLINE FRotator( float InValue ) :
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
	FORCEINLINE FRotator( float InPitch, float InYaw, float InRoll ) :
		pitch( InPitch ),
		yaw( InYaw ),
		roll( InRoll )
	{}

	/**
	 * Constructor with leaving uninitialized memory
	 */
	FORCEINLINE explicit FRotator( ENoInit )
	{}

	/**
	 * Add rotator to rotator
	 * 
	 * @param[in] InRotator Rotator
	 */
	FORCEINLINE void Add( const FRotator& InRotator )
	{
		Add( InRotator.pitch, InRotator.yaw, InRotator.roll );
	}

	/**
	 * Subtract rotator from rotator
	 * 
	 * @param[in] InRotator Rotator
	 */
	FORCEINLINE void Subtract( const FRotator& InRotator )
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
	FORCEINLINE FVector RotateVector( const FVector& InVector ) const
	{
		return InVector * ToQuaternion();
	}

	/**
	 * Convert to quaternion
	 * @return Return quaternion
	 */
	FORCEINLINE FQuaternion ToQuaternion() const
	{
		return FMath::AnglesToQuaternion( FMath::DegreesToRadians( pitch ), FMath::DegreesToRadians( yaw ), FMath::DegreesToRadians( roll ) );
	}

	/**
	 * Convert to Euler angles (in degrees)
	 * @return Return rotation as a Euler angle vector
	 */
	FORCEINLINE FVector ToEuler() const
	{
		return FVector( pitch, yaw, roll );
	}

	/**
	 * Convert to matrix
	 * @return Return rotation as a matrix
	 */
	FORCEINLINE FMatrix ToMatrix() const
	{
		return glm::mat4_cast( ToQuaternion() );
	}

	/**
	 * Get the result of adding a rotator to this
	 * 
	 * @param[in] InOther The other rotator
	 * @return The result of adding a rotator to this
	 */
	FORCEINLINE FRotator operator+( const FRotator& InOther ) const
	{
		return FRotator( pitch + InOther.pitch, yaw + InOther.yaw, roll + InOther.roll );
	}

	/**
	 * Get the result of subtracting a rotator from this
	 * 
	 * @param[in] InOther The other rotator
	 * @return The result of subtracting a rotator from this
	 */
	FORCEINLINE FRotator operator-( const FRotator& InOther ) const
	{
		return FRotator( pitch - InOther.pitch, yaw - InOther.yaw, roll - InOther.roll );
	}

	/**
	 * Adds another rotator to this
	 * 
	 * @param[in] InOther The other rotator
	 * @return Return self
	 */
	FORCEINLINE FRotator& operator+=( const FRotator& InOther )
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
	FORCEINLINE FRotator& operator-=( const FRotator& InOther )
	{
		Subtract( InOther );
		return *this;
	}

	float			pitch;		/**< Rotation around the right axis */
	float			yaw;		/**< Rotation around the up axis */
	float			roll;		/**< Rotation around the forward axis */
};

//
// Serialization
//

FORCEINLINE FArchive& operator<<( FArchive& InArchive, FRotator& InValue )
{
	InArchive << InValue.pitch;
	InArchive << InValue.yaw;
	InArchive << InValue.roll;
	return InArchive;
}

FORCEINLINE FArchive& operator<<( FArchive& InArchive, const FRotator& InValue )
{
	InArchive << InValue.pitch;
	InArchive << InValue.yaw;
	InArchive << InValue.roll;
	return InArchive;
}

#endif // !ROTATOR_H