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
#include "System/BaseArchive.h"

/**
 * @ingroup Core
 * @brief Struct for work with transform
 */
struct FTransform
{
public:
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
	 * Concatenates another rotation to this transformation 
	 * 
	 * @param[in] InDeltaRotation The rotation to concatenate in the following fashion: Rotation = Rotation * DeltaRotation
	 */
	FORCEINLINE void ConcatenateRotation( const FRotator& InDeltaRotation )
	{
		rotation += InDeltaRotation;
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

protected:
	FVector			translation;			/**< Translation of this transformation */
	FRotator		rotation;				/**< Rotation of this transformation */
	FVector			scale;					/**< 3D scale */
};

#endif // TRANSFORM_H