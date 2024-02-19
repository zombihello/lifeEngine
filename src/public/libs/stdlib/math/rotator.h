/**
 * @file
 * @addtogroup stdlib stdlib
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

#ifndef ROTATOR_H
#define ROTATOR_H

#include "stdlib/math/math.h"

/**
 * @ingroup stdlib
 * @brief Implements a container for rotation information.
 * All rotation values are stored in degrees
 */
class CRotator
{
public:
	/**
	 * @brief Constructor
	 */
	FORCEINLINE CRotator() 
		: pitch( 0.f )
		, yaw( 0.f )
		, roll( 0.f )
	{}

	/**
	 * @brief Constructor
	 * @param value		Value to set all components to
	 */
	FORCEINLINE CRotator( float value ) :
		pitch( value ),
		yaw( value ),
		roll( value )
	{}

	/**
	 * @brief Constructor
	 *
	 * @param pitch	Pitch in degrees
	 * @param yaw	Yaw in degrees
	 * @param roll	Roll in degrees
	 */
	FORCEINLINE CRotator( float pitch, float yaw, float roll ) :
		pitch( pitch ),
		yaw( yaw ),
		roll( roll )
	{}

	/**
	 * @brief Constructor
	 * @param eulerAngles	Euler angles (in degrees)
	 */
	FORCEINLINE CRotator( const Vector& eulerAngles ) :
		pitch( eulerAngles.x ),
		yaw( eulerAngles.y ),
		roll( eulerAngles.z )
	{}

	/**
	 * @brief Constructor
	 * @param quaternion	Quaternion
	 */
	FORCEINLINE CRotator( const Quaternion& quaternion )
	{
		Vector	eulerAngles = Math::QuaternionToAngles( quaternion );
		pitch	= eulerAngles.x;
		yaw		= eulerAngles.y;
		roll	= eulerAngles.z;
	}

	/**
	 * @brief Checks whether rotator is nearly zero within specified tolerance, when treated as an orientation
	 * This means that CRotator(0, 0, 360) is "zero", because it is the same final orientation as the zero rotator
	 *
	 * @param tolerance		Error tolerance
	 * @return Return TRUE if rotator is nearly zero, within specified tolerance, otherwise FALSE
	 */
	FORCEINLINE bool IsNearlyZero( float tolerance = KINDA_SMALL_NUMBER ) const
	{
		return	Math::Abs( NormalizeAxis( pitch ) ) <= tolerance &&
				Math::Abs( NormalizeAxis( yaw ) ) <= tolerance &&
				Math::Abs( NormalizeAxis( roll ) ) <= tolerance;
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
	 * @param other		The other rotator
	 * @param tolerance	Error Tolerance
	 * @return Return TRUE if two rotators are equal, within specified tolerance, otherwise FALSE
	 */
	FORCEINLINE bool Equals( const CRotator& other, float tolerance = KINDA_SMALL_NUMBER ) const
	{
		return	Math::Abs( NormalizeAxis( pitch - other.pitch ) ) <= tolerance &&
				Math::Abs( NormalizeAxis( yaw - other.yaw ) ) <= tolerance &&
				Math::Abs( NormalizeAxis( roll - other.roll ) ) <= tolerance;
	}

	/**
	 * @brief Adds to each component of the rotator
	 * 
	 * @param deltaPitch	Change in pitch (+/-)
	 * @param deltaYaw		Change in yaw (+/-)
	 * @param deltaRoll		Change in roll (+/-)
	 * @return Return copy of rotator after addition
	 */
	FORCEINLINE CRotator Add( float deltaPitch, float deltaYaw, float deltaRoll )
	{
		pitch	+= deltaPitch;
		yaw		+= deltaYaw;
		roll	+= deltaRoll;
		return *this;
	}

	/**
	 * @brief Get the inverse of the rotator
	 * @return Return the inverse of the rotator
	 */
	FORCEINLINE CRotator GetInverse() const
	{
		return CRotator( Math::QuaternionToAngles( Math::InverseQuaternion( ToQuaternion() ) ) );
	}

	/**
	 * @brief Convert to quaternion
	 * @return Return quaternion
	 */
	FORCEINLINE Quaternion ToQuaternion() const
	{
		return Math::AnglesToQuaternion( pitch, yaw, roll );
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
	 * @param vector	Vector
	 * @return Return rotated vector
	 */
	FORCEINLINE Vector RotateVector( const Vector& vector ) const
	{
		return ToQuaternion() * vector;
	}

	/**
	 * @brief Gets the rotation values so they fall within the range [0,360]
	 * @return Return clamped version of rotator
	 */
	FORCEINLINE CRotator Clamp() const
	{
		return CRotator( ClampAxis( pitch ), ClampAxis( yaw ), ClampAxis( roll ) );
	}

	/**
	 * @brief Create a copy of this rotator and normalize, removes all winding and creates the "shortest route" rotation
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
	 * @param angle		The angle to clamp
	 * @return Return the clamped angle
	 */
	static FORCEINLINE float ClampAxis( float angle )
	{
		// Returns Angle in the range (-360,360)
		angle = Math::Fmod( angle, 360.f );
		if ( angle < 0.f )
		{
			// Shift to [0,360) range
			angle += 360.f;
		}

		return angle;
	}

	/**
	 * @brief Clamps an angle to the range of (-180, 180]
	 *
	 * @param angle		The Angle to clamp
	 * @return Return the clamped angle
	 */
	static FORCEINLINE float NormalizeAxis( float angle )
	{
		// Returns Angle in the range [0,360)
		angle = ClampAxis( angle );
		if ( angle > 180.f )
		{
			// Shift to (-180,180]
			angle -= 360.f;
		}

		return angle;
	}

	/**
	 * @brief Convert a vector of floating-point Euler angles (in degrees) into a rotator. Rotator now stored in degrees
	 *
	 * @param euler		Euler angle vector
	 * @return Return a rotator from a Euler angle
	 */
	static FORCEINLINE CRotator MakeFromEuler( const Vector& euler )
	{
		return CRotator( euler.x, euler.y, euler.z );
	}

	/**
	 * @brief Convert a vector of floating-point Euler angles (in degrees) into a rotator. Rotator now stored in degrees
	 *
	 * @param eulerX	Euler angle X
	 * @param eulerY	Euler angle Y
	 * @param eulerZ	Euler angle Z
	 * @return Return a rotator from a Euler angle
	 */
	static FORCEINLINE CRotator MakeFromEuler( float eulerX, float eulerY, float eulerZ )
	{
		return CRotator( eulerX, eulerY, eulerZ );
	}

	/**
	 * @brief Get the result of adding a rotator to this
	 *
	 * @param other		The other rotator
	 * @return Return the result of adding a rotator to this
	 */
	FORCEINLINE CRotator operator+( const CRotator& other ) const
	{
		return CRotator( pitch + other.pitch, yaw + other.yaw, roll + other.roll );
	}

	/**
	 * @brief Get the result of subtracting a rotator from this
	 *
	 * @param other		The other rotator
	 * @return Return the result of subtracting a rotator from this
	 */
	FORCEINLINE CRotator operator-( const CRotator& other ) const
	{
		return CRotator( pitch - other.pitch, yaw - other.yaw, roll - other.roll );
	}

	/**
	 * @brief Checks whether two rotators are identical. This checks each component for exact equality
	 *
	 * @param other		The other rotator
	 * @return true if two rotators are identical, otherwise false
	 */
	FORCEINLINE bool operator==( const CRotator& other ) const
	{
		return pitch == other.pitch && yaw == other.yaw && roll == other.roll;
	}

	/**
	 * @brief Checks whether two rotators are different
	 *
	 * @param other		The other rotator
	 * @return Return TRUE if two rotators are different, otherwise FALSE
	 */
	FORCEINLINE bool operator!=( const CRotator& other ) const
	{
		return pitch != other.pitch || yaw != other.yaw || roll != other.roll;
	}

	/**
	 * @brief Adds another rotator to this
	 *
	 * @param other		The other rotator
	 * @return Return copy of rotator after addition
	 */
	FORCEINLINE CRotator operator+=( const CRotator& other )
	{
		pitch	+= other.pitch;
		yaw		+= other.yaw;
		roll	+= other.roll;
		return *this;
	}

	/**
	 * @brief Subtracts another rotator from this
	 *
	 * @param other	The other rotator
	 * @return Return copy of rotator after subtraction
	 */
	FORCEINLINE CRotator operator-=( const CRotator& other )
	{
		pitch	-= other.pitch;
		yaw		-= other.yaw;
		roll	-= other.roll;
		return *this;
	}

	float	pitch;		/**< Rotation around the right axis */
	float	yaw;		/**< Rotation around the up axis */
	float	roll;		/**< Rotation around the forward axis */
};

/**
 * @ingroup stdlib
 * @brief Struct for caching Quat<->Rotator conversions
 */
struct RotationConversionCache
{
	/**
	 * @brief Constructor
	 */
	FORCEINLINE RotationConversionCache()
		: cachedQuat( Math::quaternionZero )
		, cachedRotator( Math::rotatorZero )
	{}

	/**
	 * @brief Convert a CRotator to Quaternion 
	 * Uses the cached conversion if possible, and updates it if there was no match
	 * 
	 * @param rotator		Rotator
	 * @return Return converted rotator to quaternion
	 */
	FORCEINLINE Quaternion RotatorToQuat( const CRotator& rotator ) const
	{
		if ( cachedRotator != rotator )
		{
			cachedRotator	= rotator.GetNormalized();
			cachedQuat		= cachedRotator.ToQuaternion();
		}
		return cachedQuat;
	}

	/**
	 * @brief Convert a CRotator to Quaternion
	 * Uses the cached conversion if possible, but does *NOT* update the cache if there was no match
	 * 
	 * @param rotator		Rotator
	 * @return Return converted rotator to quaternion
	 */
	FORCEINLINE Quaternion RotatorToQuat_ReadOnly( const CRotator& rotator ) const
	{
		if ( cachedRotator == rotator )
		{
			return cachedQuat;
		}
		return rotator.ToQuaternion();
	}

	/**
	 * @brief Convert a Quaternion to CRotator
	 * Uses the cached conversion if possible, and updates it if there was no match
	 *
	 * @param quat		Quaternion
	 * @return Return converted quaternion to rotator
	 */
	FORCEINLINE CRotator QuatToRotator( const Quaternion& quat ) const
	{
		if ( cachedQuat != quat )
		{
			cachedQuat		= Math::NormalizeQuaternion( quat );
			cachedRotator	= CRotator( Math::QuaternionToAngles( cachedQuat ) );
		}
		return cachedRotator;
	}

	/**
	 * @brief Convert a Quaternion to CRotator
	 * Uses the cached conversion if possible, but does *NOT* update the cache if there was no match
	 *
	 * @param quat		Quaternion
	 * @return Return converted quaternion to rotator
	 */
	FORCEINLINE CRotator QuatToRotator_ReadOnly( const Quaternion& quat ) const
	{
		if ( cachedQuat == quat )
		{
			return cachedRotator;
		}
		return CRotator( Math::QuaternionToAngles( Math::NormalizeQuaternion( quat ) ) );
	}

	/**
	 * @brief Version of QuatToRotator when the Quat is known to already be normalized
	 *
	 * @param normalizedQuat	Normalized quaternion
	 * @return Return converted quaternion to rotator
	 */
	FORCEINLINE CRotator NormalizedQuatToRotator( const Quaternion& normalizedQuat ) const
	{
		if ( cachedQuat != normalizedQuat )
		{
			cachedQuat		= normalizedQuat;
			cachedRotator	= CRotator( Math::QuaternionToAngles( normalizedQuat ) );
		}
		return cachedRotator;
	}

	/**
	 * @brief Version of QuatToRotator when the Quat is known to already be normalized
	 * Does *NOT* update the cache if there was no match
	 *
	 * @param normalizedQuat	Normalized quaternion
	 * @return Return converted quaternion to rotator
	 */
	FORCEINLINE CRotator NormalizedQuatToRotator_ReadOnly( const Quaternion& normalizedQuat ) const
	{
		if ( cachedQuat == normalizedQuat )
		{
			return cachedRotator;
		}
		return CRotator( Math::QuaternionToAngles( normalizedQuat ) );
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

#endif // !ROTATOR_H