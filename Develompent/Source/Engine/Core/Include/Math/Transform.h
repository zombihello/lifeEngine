/**
 * @file
 * @addtogroup Core Core
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "Math/Math.h"
#include "Math/Rotator.h"
#include "Math/Axis.h"
#include "System/Archive.h"

/**
 * @ingroup Core
 * @brief Struct for work with transform
 */
struct FTransform
{
public:
	friend FArchive& operator<<( FArchive& InArchive, FTransform& InValue );
	friend FArchive& operator<<( FArchive& InArchive, const FTransform& InValue );

	/**
	 * Constructor with initialization to the identity transform
	 */
	FORCEINLINE FTransform() :
		translation( FMath::vectorZero ),
		rotation( FMath::rotatorZero ),
		scale( FMath::vectorOne )
	{}

	/**
	 * Constructor with an initial translation
	 * 
	 * @param[in] InTranslation The value to use for the translation component
	 */
	FORCEINLINE explicit FTransform( const FVector& InTranslation ) :
		translation( InTranslation ),
		rotation( FMath::rotatorZero ),
		scale( FMath::vectorOne )
	{}

	/**
	 * Constructor with an initial rotation
	 * 
	 * @param[in] InRotation The value to use for rotation component
	 */
	FORCEINLINE explicit FTransform( const FRotator& InRotation ) :
		translation( FMath::vectorZero ),
		rotation( InRotation ),
		scale( FMath::vectorOne )
	{}

	/**
	 * Constructor with all components initialized
	 * 
	 * @param[in] InRotation The value to use for rotation component
	 * @param[in] InTranslation The value to use for the translation component
	 * @param[in] InScale The value to use for the scale component
	 */
	FORCEINLINE FTransform( const FRotator& InRotation, const FVector& InTranslation, const FVector& InScale = FMath::vectorOne ) :
		translation( InTranslation ),
		rotation( InRotation ),
		scale( InScale )
	{}

	/**
	 * Constructor with leaving uninitialized memory
	 */
	FORCEINLINE explicit FTransform( ENoInit )
	{}

	/**
	 * Copy translation from another FTransform
	 * 
	 * @param[in] InOther Another transform
	 */
	FORCEINLINE void CopyTranslation( const FTransform& InOther )
	{
		translation = InOther.translation;
	}

	/**
	 * Adjusts the translation component of this transformation
	 * 
	 * @param[in] InDeltaTranslation The translation to add in the following fashion: Translation += InDeltaTranslation
	 */
	FORCEINLINE void AddToTranslation( const FVector& InDeltaTranslation )
	{
		translation += InDeltaTranslation;
	}

	/**
	 * Subtract from the translation component of this transformation
	 *
	 * @param[in] InDeltaTranslation Delta translation
	 */
	FORCEINLINE void SubtractFromTranslation( const FVector& InDeltaTranslation )
	{
		translation -= InDeltaTranslation;
	}

	/**
	 * Add another rotation to this transformation 
	 * 
	 * @param[in] InDeltaRotation The rotation to add in the following fashion: Rotation = Rotation * DeltaRotation
	 */
	FORCEINLINE void AddToRotation( const FRotator& InDeltaRotation )
	{
		rotation += InDeltaRotation;
	}

	/**
	 * Subtract another rotation from this transformation
	 *
	 * @param[in] InDeltaRotation Delta rotation
	 */
	FORCEINLINE void SubtractFromRotation( const FRotator& InDeltaRotation )
	{
		rotation -= InDeltaRotation;
	}

	/**
	 * Adjusts the scale component of this transformation
	 * 
	 * @param[in] InDeltaScale The scale to add in the following fashion: Scale += InDeltaScale
	 */
	FORCEINLINE void AddToScale( const FVector& InDeltaScale )
	{
		scale += InDeltaScale;
	}

	/**
	 * Subtract from the scale component of this transformation
	 *
	 * @param[in] InDeltaScale Delta scale
	 */
	FORCEINLINE void SubtractFromScale( const FVector& InDeltaScale )
	{
		scale -= InDeltaScale;
	}

	/**
	 * Copy rotation from another FTransform
	 * 
	 * @param[in] InOther Another transform
	 */
	FORCEINLINE void CopyRotation( const FTransform& InOther )
	{
		rotation = InOther.rotation;
	}

	/**
	 * Copy scale from another FTransform
	 * 
	 * @param[in] InOther Another transform
	 */
	FORCEINLINE void CopyScale( const FTransform& InOther )
	{
		scale = InOther.scale;
	}

	/**
	 * Add transform to transform
	 * 
	 * @param[in] InOther Another transform
	 */
	FORCEINLINE void Add( const FTransform& InOther )
	{
		translation += InOther.translation;
		rotation += InOther.rotation;
		scale += InOther.scale;
	}

	/**
	 * Subtract transform from transform
	 * 
	 * @param[in] InOther Another transform
	 */
	FORCEINLINE void Subtract( const FTransform& InOther )
	{
		translation -= InOther.translation;
		rotation -= InOther.rotation;
		scale -= InOther.scale;
	}

	/**
	 * Rotate vector
	 * 
	 * @param[in] InVector Vector
	 * @return Return rotated vector
	 */
	FORCEINLINE FVector RotateVector( const FVector& InVector ) const
	{
		return InVector * rotation.ToQuaternion();
	}

	/**
	 * Set location
	 * 
	 * @param[in] InLocation New location
	 */
	FORCEINLINE void SetLocation( const FVector& InLocation )
	{
		translation = InLocation;
	}

	/**
	 * Set rotation
	 * 
	 * @param[in] InRotation New rotation
	 */
	FORCEINLINE void SetRotation( const FRotator& InRotation )
	{
		rotation = InRotation;
	}

	/**
	 * Set scale
	 * 
	 * @param[in] InScale New scale
	 */
	FORCEINLINE void SetScale( const FVector& InScale )
	{
		scale = InScale;
	}

	/**
	 * Set identity
	 */
	FORCEINLINE void SetIdentity()
	{
		translation = FMath::vectorZero;
		rotation = FMath::rotatorZero;
		scale = FMath::vectorOne;
	}

	/**
	 * Get unit axis
	 * 
	 * @param[in] InAxis Axis
	 * @return Return unit axis, if InAxis = A_None return FMath::vectorZero
	 */
	FORCEINLINE FVector GetUnitAxis( EAxis InAxis ) const
	{
		switch ( InAxis )
		{
		case A_X:		return RotateVector( FVector( 1.f, 0.f, 0.f ) );	break;
		case A_Y:		return RotateVector( FVector( 0.f, 1.f, 0.f ) );	break;
		case A_Z:		return RotateVector( FVector( 0.f, 0.f, 1.f ) );	break;
		}

		return FMath::vectorZero;
	}

	/**
	 * Get location
	 * @return Return location of transform
	 */
	FORCEINLINE FVector GetLocation() const
	{
		return translation;
	}

	/**
	 * Get rotation
	 * @return Return rotation of transform
	 */
	FORCEINLINE FRotator GetRotation() const
	{
		return rotation;
	}

	/**
	 * Get scale
	 * @return Return scale of transform
	 */
	FORCEINLINE FVector GetScale() const
	{
		return scale;
	}

	/**
	 * Convert transform to matrix
	 * @return Return matrix with location, scale and rotation
	 */
	FORCEINLINE FMatrix ToMatrix() const
	{
		FMatrix		result;
		ToMatrix( result );
		return result;
	}

	/**
	 * Convert transform to matrix
	 * @param OutMatrix Output matrix with location, scale and rotation
	 */
	FORCEINLINE void ToMatrix( FMatrix& OutMatrix ) const
	{
		const FQuaternion		quaternion = rotation.ToQuaternion();
		OutMatrix[ 3 ].x = translation.x;
		OutMatrix[ 3 ].y = translation.y;
		OutMatrix[ 3 ].z = translation.z;

		const float				x2 = quaternion.x + quaternion.x;
		const float				y2 = quaternion.y + quaternion.y;
		const float				z2 = quaternion.z + quaternion.z;
		{
			const float			xx2 = quaternion.x * x2;
			const float			yy2 = quaternion.y * y2;
			const float			zz2 = quaternion.z * z2;

			OutMatrix[ 0 ].x = ( 1.0f - ( yy2 + zz2 ) ) * scale.x;
			OutMatrix[ 1 ].y = ( 1.0f - ( xx2 + zz2 ) ) * scale.y;
			OutMatrix[ 2 ].z = ( 1.0f - ( xx2 + yy2 ) ) * scale.z;
		}
		{
			const float			yz2 = quaternion.y * z2;
			const float			wx2 = quaternion.w * x2;

			OutMatrix[ 2 ].y = ( yz2 - wx2 ) * scale.z;
			OutMatrix[ 1 ].z = ( yz2 + wx2 ) * scale.y;
		}
		{
			const float			xy2 = quaternion.x * y2;
			const float			wz2 = quaternion.w * z2;

			OutMatrix[ 1 ].x = ( xy2 - wz2 ) * scale.y;
			OutMatrix[ 0 ].y = ( xy2 + wz2 ) * scale.x;
		}
		{
			const float			xz2 = quaternion.x * z2;
			const float			wy2 = quaternion.w * y2;

			OutMatrix[ 2 ].x = ( xz2 + wy2 ) * scale.z;
			OutMatrix[ 0 ].z = ( xz2 - wy2 ) * scale.x;
		}

		OutMatrix[ 0 ].w = 0.0f;
		OutMatrix[ 1 ].w = 0.0f;
		OutMatrix[ 2 ].w = 0.0f;
		OutMatrix[ 3 ].w = 1.0f;
	}

	/**
	 * Override operator +
	 */
	FORCEINLINE FTransform operator+( const FTransform& InOther ) const
	{
		return FTransform( rotation + InOther.rotation, translation + InOther.translation, scale + InOther.scale );
	}

	/**
	 * Override operator -
	 */
	FORCEINLINE FTransform operator-( const FTransform& InOther ) const
	{
		return FTransform( rotation - InOther.rotation, translation - InOther.translation, scale - InOther.scale );
	}

protected:
	FVector			translation;			/**< Translation of this transformation */
	FRotator		rotation;				/**< Rotation of this transformation */
	FVector			scale;					/**< 3D scale */
};

//
// Serialization
//

/**
 * Overload operator << for serialize FTransform
 */
FORCEINLINE FArchive& operator<<( FArchive& InArchive, FTransform& InValue )
{
	InArchive << InValue.translation;
	InArchive << InValue.rotation;
	InArchive << InValue.scale;
	return InArchive;
}

/**
 * Overload operator << for serialize FTransform
 */
FORCEINLINE FArchive& operator<<( FArchive& InArchive, const FTransform& InValue )
{
	InArchive << InValue.translation;
	InArchive << InValue.rotation;
	InArchive << InValue.scale;
	return InArchive;
}

#endif // TRANSFORM_H
