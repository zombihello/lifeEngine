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

#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "stdlib/math/math.h"
#include "core/debug.h"

/**
 * @ingroup stdlib
 * @brief Class for work with transform
 */
class CTransform
{
public:
	/**
	 * @brief Constructor with initialization to the identity transform
	 */
	FORCEINLINE CTransform() 
		: bDirtyMatrix( true )
		, translation( Math::vectorZero )
		, rotation( Math::quaternionZero )
		, scale( Math::vectorOne )
		, matrix( Math::matrixIdentity )
	{}

	/**
	 * @brief Constructor with an initial translation
	 * @param translation	The value to use for the translation component
	 */
	FORCEINLINE explicit CTransform( const Vector& translation )
		: bDirtyMatrix( true )
		, translation( translation )
		, rotation( Math::quaternionZero )
		, scale( Math::vectorOne )
		, matrix( Math::matrixIdentity )
	{}

	/**
	 * @brief Constructor with an initial rotation
	 * @param rotation		The value to use for rotation component
	 */
	FORCEINLINE explicit CTransform( const Quaternion& rotation )
		: bDirtyMatrix( true )
		, translation( Math::vectorZero )
		, rotation( rotation )
		, scale( Math::vectorOne )
		, matrix( Math::matrixIdentity )
	{}

	/**
	 * @brief Constructor with all components initialized
	 * 
	 * @param rotation		The value to use for rotation component
	 * @param translation	The value to use for the translation component
	 * @param scale			The value to use for the scale component
	 */
	FORCEINLINE CTransform( const Quaternion& rotation, const Vector& translation, const Vector& scale = Math::vectorOne ) 
		: bDirtyMatrix( true )
		, translation( translation )
		, rotation( rotation )
		, scale( scale )
		, matrix( Math::matrixIdentity )
	{}

	/**
	 * @brief Copy translation from another CTransform
	 * @param other		Another transform
	 */
	FORCEINLINE void CopyTranslation( const CTransform& other )
	{
		translation		= other.translation;
		bDirtyMatrix	= true;
	}

	/**
	 * @brief Adjusts the translation component of this transformation
	 * @param deltaTranslation		The translation to add in the following fashion: Translation += deltaTranslation
	 */
	FORCEINLINE void AddToTranslation( const Vector& deltaTranslation )
	{
		translation		+= deltaTranslation;
		bDirtyMatrix	= true;
	}

	/**
	 * @brief Subtract from the translation component of this transformation
	 * @param deltaTranslation		Delta translation
	 */
	FORCEINLINE void SubtractFromTranslation( const Vector& deltaTranslation )
	{
		translation		-= deltaTranslation;
		bDirtyMatrix	= true;
	}

	/**
	 * @brief Add another rotation to this transformation 
	 * @param deltaRotation		The rotation to add in the following fashion: Rotation = Rotation * deltaRotation
	 */
	FORCEINLINE void AddToRotation( const Quaternion& deltaRotation )
	{
		rotation		= deltaRotation * rotation;
		bDirtyMatrix	= true;
	}

	/**
	 * @brief Subtract another rotation from this transformation
	 * @param deltaRotation		Delta rotation
	 */
	FORCEINLINE void SubtractFromRotation( const Quaternion& deltaRotation )
	{
		rotation		= Math::InverseQuaternion( deltaRotation ) * rotation;
		bDirtyMatrix	= true;
	}

	/**
	 * @brief Adjusts the scale component of this transformation
	 * @param deltaScale	The scale to add in the following fashion: Scale += deltaScale
	 */
	FORCEINLINE void AddToScale( const Vector& deltaScale )
	{
		scale			+= deltaScale;
		bDirtyMatrix	= true;
	}

	/**
	 * @brief Subtract from the scale component of this transformation
	 * @param deltaScale	Delta scale
	 */
	FORCEINLINE void SubtractFromScale( const Vector& deltaScale )
	{
		scale			-= deltaScale;
		bDirtyMatrix	= true;
	}

	/**
	 * @brief Copy rotation from another CTransform
	 * @param other		Another transform
	 */
	FORCEINLINE void CopyRotation( const CTransform& other )
	{
		rotation		= other.rotation;
		bDirtyMatrix	= true;
	}

	/**
	 * @brief Copy scale from another CTransform
	 * @param other		Another transform
	 */
	FORCEINLINE void CopyScale( const CTransform& other )
	{
		scale			= other.scale;
		bDirtyMatrix	= true;
	}

	/**
	 * @brief Add transform to transform
	 * @param other		Another transform
	 */
	FORCEINLINE void Add( const CTransform& other )
	{
		translation	+= other.translation;
		rotation	= other.rotation * rotation;
		scale		+= other.scale;
		bDirtyMatrix = true;
	}

	/**
	 * @brief Subtract transform from transform
	 * @param other		Another transform
	 */
	FORCEINLINE void Subtract( const CTransform& other )
	{
		translation	-= other.translation;
		rotation	= Math::InverseQuaternion( other.rotation ) * rotation;
		scale		-= other.scale;
		bDirtyMatrix = true;
	}

	/**
	 * Rotate vector
	 * 
	 * @param vector 	Vector
	 * @return Return rotated vector
	 */
	FORCEINLINE Vector RotateVector( const Vector& vector ) const
	{
		return rotation * vector;
	}

	/**
	 * @brief Compare transform without scale
	 *
	 * @param otherTransform	Other transform
	 * @return Return true if equal with otherTransform, else returning false
	 */
	FORCEINLINE bool MatchesNoScale( const CTransform& otherTransform ) const
	{
		return translation == otherTransform.translation && rotation == otherTransform.rotation;
	}

	/**
	 * @brief Compare transform
	 *
	 * @param otherTransform	Other transform
	 * @return Return true if equal with otherTransform, else returning false
	 */
	FORCEINLINE bool Matches( const CTransform& otherTransform ) const
	{
		return MatchesNoScale( otherTransform ) && scale == otherTransform.scale;
	}

	/**
	 * Set location
	 * @param newLocation	A new location
	 */
	FORCEINLINE void SetLocation( const Vector& newLocation )
	{
		translation		= newLocation;
		bDirtyMatrix	= true;
	}

	/**
	 * @brief Set rotation
	 * @param newRotation	A new rotation
	 */
	FORCEINLINE void SetRotation( const Quaternion& newRotation )
	{
		rotation		= newRotation;
		bDirtyMatrix	= true;
	}

	/**
	 * @brief Set scale
	 * @param newScale	A new scale
	 */
	FORCEINLINE void SetScale( const Vector& newScale )
	{
		scale			= newScale;
		bDirtyMatrix	= true;
	}

	/**
	 * @brief Set identity
	 */
	FORCEINLINE void SetIdentity()
	{
		translation		= Math::vectorZero;
		rotation		= Math::quaternionZero;
		scale			= Math::vectorOne;
		bDirtyMatrix	= true;
	}

	/**
	 * @brief Get unit axis
	 * 
	 * @param axis	Axis
	 * @return Return unit axis
	 */
	FORCEINLINE Vector GetUnitAxis( EAxis axis ) const
	{
		switch ( axis )
		{
		case AXIS_X:	return RotateVector( Vector( 1.f, 0.f, 0.f ) );	break;
		case AXIS_Y:	return RotateVector( Vector( 0.f, 1.f, 0.f ) );	break;
		case AXIS_Z:	return RotateVector( Vector( 0.f, 0.f, 1.f ) );	break;
		default:		Assert( false ); break;
		}

		return Math::vectorZero;
	}

	/**
	 * @brief Get location
	 * @return Return location of transform
	 */
	FORCEINLINE Vector GetLocation() const
	{
		return translation;
	}

	/**
	 * @brief Get rotation
	 * @return Return rotation of transform
	 */
	FORCEINLINE Quaternion GetRotation() const
	{
		return rotation;
	}

	/**
	 * @brief Get scale
	 * @return Return scale of transform
	 */
	FORCEINLINE Vector GetScale() const
	{
		return scale;
	}

	/**
	 * @brief Convert transform to matrix
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
	 * @brief Convert transform to matrix
	 * @param destMatrix	Output matrix with location, scale and rotation
	 */
	FORCEINLINE void ToMatrix( Matrix& destMatrix ) const
	{
		if ( bDirtyMatrix )
		{
			matrix			= Math::TranslateMatrix( translation ) * Math::QuaternionToMatrix( rotation ) * Math::ScaleMatrix( scale );
			bDirtyMatrix	= false;
		}
		destMatrix			= matrix;
	}

	/**
	 * @brief Override operator +
	 */
	FORCEINLINE CTransform operator+( const CTransform& other ) const
	{
		return CTransform( other.rotation * rotation, translation + other.translation, scale + other.scale );
	}

	/**
	 * @brief Override operator -
	 */
	FORCEINLINE CTransform operator-( const CTransform& other ) const
	{
		return CTransform( Math::InverseQuaternion( other.rotation ) * rotation, translation - other.translation, scale - other.scale );
	}

protected:
	mutable bool	bDirtyMatrix;	/**< Is dirty matrix */
	Vector			translation;	/**< Translation of this transformation */
	Quaternion		rotation;		/**< Rotation of this transformation */
	Vector			scale;			/**< 3D scale */
	mutable Matrix	matrix;			/**< Transformation in matrix */
};

#endif // TRANSFORM_H
