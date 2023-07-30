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
#include "Math/Axis.h"
#include "System/Archive.h"

/**
 * @ingroup Core
 * @brief Struct for work with transform
 */
struct CTransform
{
public:
	friend CArchive& operator<<( CArchive& InArchive, CTransform& InValue );
	friend CArchive& operator<<( CArchive& InArchive, const CTransform& InValue );

	/**
	 * Constructor with initialization to the identity transform
	 */
	FORCEINLINE CTransform() 
		: bDirtyMatrix( true )
		, translation( SMath::vectorZero )
		, rotation( SMath::quaternionZero )
		, scale( SMath::vectorOne )
		, matrix( SMath::matrixIdentity )
	{}

	/**
	 * Constructor with an initial translation
	 * 
	 * @param[in] InTranslation The value to use for the translation component
	 */
	FORCEINLINE explicit CTransform( const Vector& InTranslation ) 
		: bDirtyMatrix( true )
		, translation( InTranslation )
		, rotation( SMath::quaternionZero )
		, scale( SMath::vectorOne )
		, matrix( SMath::matrixIdentity )
	{}

	/**
	 * Constructor with an initial rotation
	 * 
	 * @param[in] InRotation The value to use for rotation component
	 */
	FORCEINLINE explicit CTransform( const Quaternion& InRotation ) 
		: bDirtyMatrix( true )
		, translation( SMath::vectorZero )
		, rotation( InRotation )
		, scale( SMath::vectorOne )
		, matrix( SMath::matrixIdentity )
	{}

	/**
	 * Constructor with all components initialized
	 * 
	 * @param[in] InRotation The value to use for rotation component
	 * @param[in] InTranslation The value to use for the translation component
	 * @param[in] InScale The value to use for the scale component
	 */
	FORCEINLINE CTransform( const Quaternion& InRotation, const Vector& InTranslation, const Vector& InScale = SMath::vectorOne ) 
		: bDirtyMatrix( true )
		, translation( InTranslation )
		, rotation( InRotation )
		, scale( InScale )
		, matrix( SMath::matrixIdentity )
	{}

	/**
	 * Constructor with leaving uninitialized memory
	 */
	FORCEINLINE explicit CTransform( ENoInit )
	{}

	/**
	 * Copy translation from another CTransform
	 * 
	 * @param[in] InOther Another transform
	 */
	FORCEINLINE void CopyTranslation( const CTransform& InOther )
	{
		translation		= InOther.translation;
		bDirtyMatrix	= true;
	}

	/**
	 * Adjusts the translation component of this transformation
	 * 
	 * @param[in] InDeltaTranslation The translation to add in the following fashion: Translation += InDeltaTranslation
	 */
	FORCEINLINE void AddToTranslation( const Vector& InDeltaTranslation )
	{
		translation		+= InDeltaTranslation;
		bDirtyMatrix	= true;
	}

	/**
	 * Subtract from the translation component of this transformation
	 *
	 * @param[in] InDeltaTranslation Delta translation
	 */
	FORCEINLINE void SubtractFromTranslation( const Vector& InDeltaTranslation )
	{
		translation		-= InDeltaTranslation;
		bDirtyMatrix	= true;
	}

	/**
	 * Add another rotation to this transformation 
	 * 
	 * @param[in] InDeltaRotation The rotation to add in the following fashion: Rotation = Rotation * DeltaRotation
	 */
	FORCEINLINE void AddToRotation( const Quaternion& InDeltaRotation )
	{
		rotation		= InDeltaRotation * rotation;
		bDirtyMatrix	= true;
	}

	/**
	 * Subtract another rotation from this transformation
	 *
	 * @param[in] InDeltaRotation Delta rotation
	 */
	FORCEINLINE void SubtractFromRotation( const Quaternion& InDeltaRotation )
	{
		rotation		= SMath::InverseQuaternion( InDeltaRotation ) * rotation;
		bDirtyMatrix	= true;
	}

	/**
	 * Adjusts the scale component of this transformation
	 * 
	 * @param[in] InDeltaScale The scale to add in the following fashion: Scale += InDeltaScale
	 */
	FORCEINLINE void AddToScale( const Vector& InDeltaScale )
	{
		scale			+= InDeltaScale;
		bDirtyMatrix	= true;
	}

	/**
	 * Subtract from the scale component of this transformation
	 *
	 * @param[in] InDeltaScale Delta scale
	 */
	FORCEINLINE void SubtractFromScale( const Vector& InDeltaScale )
	{
		scale			-= InDeltaScale;
		bDirtyMatrix	= true;
	}

	/**
	 * Copy rotation from another CTransform
	 * 
	 * @param[in] InOther Another transform
	 */
	FORCEINLINE void CopyRotation( const CTransform& InOther )
	{
		rotation		= InOther.rotation;
		bDirtyMatrix	= true;
	}

	/**
	 * Copy scale from another CTransform
	 * 
	 * @param[in] InOther Another transform
	 */
	FORCEINLINE void CopyScale( const CTransform& InOther )
	{
		scale			= InOther.scale;
		bDirtyMatrix	= true;
	}

	/**
	 * Add transform to transform
	 * 
	 * @param[in] InOther Another transform
	 */
	FORCEINLINE void Add( const CTransform& InOther )
	{
		translation	+= InOther.translation;
		rotation	=  InOther.rotation * rotation;
		scale		+= InOther.scale;
		bDirtyMatrix = true;
	}

	/**
	 * Subtract transform from transform
	 * 
	 * @param[in] InOther Another transform
	 */
	FORCEINLINE void Subtract( const CTransform& InOther )
	{
		translation	-= InOther.translation;
		rotation	= SMath::InverseQuaternion( InOther.rotation ) * rotation;
		scale		-= InOther.scale;
		bDirtyMatrix = true;
	}

	/**
	 * Rotate vector
	 * 
	 * @param[in] InVector Vector
	 * @return Return rotated vector
	 */
	FORCEINLINE Vector RotateVector( const Vector& InVector ) const
	{
		return rotation * InVector;
	}

	/**
	 * @brief Compare transform without scale
	 *
	 * @param InOtherTransform Other transform
	 * @return Return true if equal with InOtherTransform, else returning false
	 */
	FORCEINLINE bool MatchesNoScale( const CTransform& InOtherTransform ) const
	{
		return translation == InOtherTransform.translation && rotation == InOtherTransform.rotation;
	}

	/**
	 * @brief Compare transform
	 *
	 * @param InOtherTransform Other transform
	 * @return Return true if equal with InOtherTransform, else returning false
	 */
	FORCEINLINE bool Matches( const CTransform& InOtherTransform ) const
	{
		return MatchesNoScale( InOtherTransform ) && scale == InOtherTransform.scale;
	}

	/**
	 * Set location
	 * 
	 * @param[in] InLocation New location
	 */
	FORCEINLINE void SetLocation( const Vector& InLocation )
	{
		translation		= InLocation;
		bDirtyMatrix	= true;
	}

	/**
	 * Set rotation
	 * 
	 * @param[in] InRotation New rotation
	 */
	FORCEINLINE void SetRotation( const Quaternion& InRotation )
	{
		rotation		= InRotation;
		bDirtyMatrix	= true;
	}

	/**
	 * Set scale
	 * 
	 * @param[in] InScale New scale
	 */
	FORCEINLINE void SetScale( const Vector& InScale )
	{
		scale			= InScale;
		bDirtyMatrix	= true;
	}

	/**
	 * Set identity
	 */
	FORCEINLINE void SetIdentity()
	{
		translation		= SMath::vectorZero;
		rotation		= SMath::quaternionZero;
		scale			= SMath::vectorOne;
		bDirtyMatrix	= true;
	}

	/**
	 * Get unit axis
	 * 
	 * @param[in] InAxis Axis
	 * @return Return unit axis, if InAxis = A_None return SMath::vectorZero
	 */
	FORCEINLINE Vector GetUnitAxis( EAxis InAxis ) const
	{
		switch ( InAxis )
		{
		case A_X:		return RotateVector( Vector( 1.f, 0.f, 0.f ) );	break;
		case A_Y:		return RotateVector( Vector( 0.f, 1.f, 0.f ) );	break;
		case A_Z:		return RotateVector( Vector( 0.f, 0.f, 1.f ) );	break;
		}

		return SMath::vectorZero;
	}

	/**
	 * Get location
	 * @return Return location of transform
	 */
	FORCEINLINE Vector GetLocation() const
	{
		return translation;
	}

	/**
	 * Get rotation
	 * @return Return rotation of transform
	 */
	FORCEINLINE Quaternion GetRotation() const
	{
		return rotation;
	}

	/**
	 * Get scale
	 * @return Return scale of transform
	 */
	FORCEINLINE Vector GetScale() const
	{
		return scale;
	}

	/**
	 * Convert transform to matrix
	 * @return Return matrix with location, scale and rotation
	 */
	FORCEINLINE const Matrix& ToMatrix() const
	{
		if ( bDirtyMatrix )
		{
			ToMatrix( matrix );
			bDirtyMatrix	= false;
		}
		return matrix;
	}

	/**
	 * Convert transform to matrix
	 * @param OutMatrix Output matrix with location, scale and rotation
	 */
	FORCEINLINE void ToMatrix( Matrix& OutMatrix ) const
	{
		if ( bDirtyMatrix )
		{
			matrix			= SMath::TranslateMatrix( translation ) * SMath::QuaternionToMatrix( rotation ) * SMath::ScaleMatrix( scale );
			bDirtyMatrix	= false;
		}
		OutMatrix			= matrix;
	}

	/**
	 * Override operator +
	 */
	FORCEINLINE CTransform operator+( const CTransform& InOther ) const
	{
		return CTransform( InOther.rotation * rotation, translation + InOther.translation, scale + InOther.scale );
	}

	/**
	 * Override operator -
	 */
	FORCEINLINE CTransform operator-( const CTransform& InOther ) const
	{
		return CTransform( SMath::InverseQuaternion( InOther.rotation ) * rotation, translation - InOther.translation, scale - InOther.scale );
	}

protected:
	mutable bool	bDirtyMatrix;	/**< Is dirty matrix */
	Vector			translation;	/**< Translation of this transformation */
	Quaternion		rotation;		/**< Rotation of this transformation */
	Vector			scale;			/**< 3D scale */
	mutable Matrix	matrix;			/**< Transformation in matrix */
};

//
// Serialization
//

/**
 * Overload operator << for serialize CTransform
 */
FORCEINLINE CArchive& operator<<( CArchive& InArchive, CTransform& InValue )
{
	InArchive << InValue.translation;
	InArchive << InValue.rotation;
	InArchive << InValue.scale;

	if ( InArchive.IsLoading() )
	{
		InValue.bDirtyMatrix = true;
	}
	return InArchive;
}

/**
 * Overload operator << for serialize CTransform
 */
FORCEINLINE CArchive& operator<<( CArchive& InArchive, const CTransform& InValue )
{
	Assert( InArchive.IsSaving() );
	InArchive << InValue.translation;
	InArchive << InValue.rotation;
	InArchive << InValue.scale;
	return InArchive;
}

#endif // TRANSFORM_H
