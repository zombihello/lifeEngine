/**
 * @file
 * @addtogroup Core Core
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <glm.hpp>
#include <gtx/quaternion.hpp>

#include "Math/Math.h"
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
		rotation( FMath::quaternionZero ),
		scale( FMath::vectorOne )
	{}

	/**
	 * Constructor with an initial translation
	 * 
	 * @param[in] InTranslation The value to use for the translation component
	 */
	FORCEINLINE explicit FTransform( const glm::vec3& InTranslation ) :
		translation( InTranslation ),
		rotation( FMath::quaternionZero ),
		scale( FMath::vectorOne )
	{}

	/**
	 * Constructor with an initial rotation
	 * 
	 * @param[in] InRotation The value to use for rotation component
	 */
	FORCEINLINE explicit FTransform( const glm::quat& InRotation ) :
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
	FORCEINLINE FTransform( const glm::quat& InRotation, const glm::vec3& InTranslation, const glm::vec3& InScale = FMath::vectorOne ) :
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
	FORCEINLINE void AddToTranslation( const glm::vec3& InDeltaTranslation )
	{
		translation += InDeltaTranslation;
	}

	/**
	 * Concatenates another rotation to this transformation 
	 * 
	 * @param[in] InDeltaRotation The rotation to concatenate in the following fashion: Rotation = Rotation * DeltaRotation
	 */
	FORCEINLINE void ConcatenateRotation( const glm::quat& InDeltaRotation )
	{
		rotation *= InDeltaRotation;
	}

	/**
	 * Adjusts the scale component of this transformation
	 * 
	 * @param[in] InDeltaScale The scale to add in the following fashion: Scale += InDeltaScale
	 */
	FORCEINLINE void AddToScale( const glm::vec3& InDeltaScale )
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
	FORCEINLINE void SetLocation( const glm::vec3& InLocation )
	{
		translation = InLocation;
	}

	/**
	 * Set rotation
	 * 
	 * @param[in] InRotation New rotation
	 */
	FORCEINLINE void SetRotation( const glm::quat& InRotation )
	{
		rotation = InRotation;
	}

	/**
	 * Set scale
	 * 
	 * @param[in] InScale New scale
	 */
	FORCEINLINE void SetScale( const glm::vec3& InScale )
	{
		scale = InScale;
	}

	/**
	 * Set identity
	 */
	FORCEINLINE void SetIdentity()
	{
		translation = FMath::vectorZero;
		rotation = FMath::quaternionZero;
		scale = FMath::vectorOne;
	}

	/**
	 * Get location
	 * @return Return location of transform
	 */
	FORCEINLINE glm::vec3 GetLocation() const
	{
		return translation;
	}

	/**
	 * Get rotation
	 * @return Return rotation of transform
	 */
	FORCEINLINE glm::quat GetRotation() const
	{
		return rotation;
	}

	/**
	 * Get scale
	 * @return Return scale of transform
	 */
	FORCEINLINE glm::vec3 GetScale() const
	{
		return scale;
	}

protected:
	glm::vec3			translation;			/**< Translation of this transformation */
	glm::quat			rotation;				/**< Rotation of this transformation */
	glm::vec3			scale;					/**< 3D scale */
};

#endif // TRANSFORM_H