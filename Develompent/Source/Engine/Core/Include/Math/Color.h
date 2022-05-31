/**
 * @file
 * @addtogroup Core Core
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef COLOR_H
#define COLOR_H

#include "Core.h"
#include "Misc/Types.h"
#include "System/Archive.h"
#include "Math/Math.h"

/**
 * @ingroup Core
 * @brief The class for work with color
 */
class ÑColor
{
public:
	friend CArchive& operator<<( CArchive& InArchive, ÑColor& InValue );
	friend CArchive& operator<<( CArchive& InArchive, const ÑColor& InValue );

	/**
	 * @brief Default constructor
	 */
	FORCEINLINE	ÑColor() :
		r( 0 ),
		g( 0 ),
		b( 0 ),
		a( 255 )
	{}

	/**
	 * @brief Constructor
	 * 
	 * @param[in] InRed Red component of color
	 * @param[in] InGreen Green component of color
	 * @param[in] InBlue Blue component of color
	 * @param[in] InAlpha Alpha component of color
	 */
	FORCEINLINE	ÑColor( uint8 InRed, uint8 InGreen, uint8 InBlue, uint8 InAlpha = 255 )
	{
		Set( InRed, InGreen, InBlue, InAlpha );
	}

	/**
	 * @brief Constructor
	 * @warning Values must be in range [0:1]
	 *
	 * @param InLinearColor	Linear color
	 */
	FORCEINLINE ÑColor( const Vector4D& InLinearColor )
	{
		Set( InLinearColor );
	}

	/**
	 * @brief Constructor
	 * @param InColor		Color in uint32 type
	 */
	FORCEINLINE ÑColor( uint32 InColor )
	{
		GetUInt32Color() = InColor;
	}

	/**
	 * @brief Set components of color
	 * 
	 * @param[in] InRed Red component of color
	 * @param[in] InGreen Green component of color
	 * @param[in] InBlue Blue component of color
	 * @param[in] InAlpha Alpha component of color
	 */
	FORCEINLINE void			Set( uint8 InRed, uint8 InGreen, uint8 InBlue, uint8 InAlpha = 255 )
	{
		r = InRed;
		g = InGreen;
		b = InBlue;
		a = InAlpha;
	}

	/**
	 * @brief Set components of color
	 * @warning Values must be in range [0:1]
	 *
	 * @param InRed			Red component of color
	 * @param InGreen		Green component of color
	 * @param InBlue		Blue component of color
	 * @param InAlpha		Alpha component of color
	 */
	FORCEINLINE void			Set( float InRed, float InGreen, float InBlue, float InAlpha = 255 )
	{
		r = InRed * 255.f;
		g = InGreen * 255.f;
		b = InBlue * 255.f;
		a = InAlpha * 255.f;
	}

	/**
	 * @brief Set components of color
	 * @warning Values must be in range [0:1]
	 *
	 * @param InLinearColor	Linear color
	 */
	FORCEINLINE void			Set( const Vector4D& InLinearColor )
	{
		Set( InLinearColor.x, InLinearColor.y, InLinearColor.z, InLinearColor.a );
	}

	/**
	 * @brief Color in uint32 type
	 * @return Return color in uint32 type
	 */
	FORCEINLINE uint32&	GetUInt32Color() 
	{ 
		return *( uint32* )this;
	}

	/**
	 * @brief Color in uint32 type
	 * @return Return color in uint32 type
	 */
	FORCEINLINE const uint32& GetUInt32Color( void ) const
	{ 
		return *( uint32* )this;
	}

	/**
	 * @brief Convert ÑColor to Vector4D
	 * @return Return converted ÑColor to Vector4D
	 */
	FORCEINLINE Vector4D ToVector4D() const
	{
		return Vector4D( r, g, b, a );
	}

	/**
	 * @brief Convert ÑColor to normalized Vector4D (in range 0 -> 1)
	 * @return Return converted ÑColor to normalized Vector4D (in range 0 -> 1)
	 */
	FORCEINLINE Vector4D ToNormalizedVector4D() const
	{
		return Vector4D( r / 255.f, g / 255.f, b / 255.f, a / 255.f );
	}

	/**
	 * @brief Override operator ==
	 */
	FORCEINLINE bool operator==( const ÑColor& InRight ) const
	{
		return r == InRight.r && g == InRight.g && b == InRight.b && a == InRight.a;
	}

	static ÑColor			black;			/**< Black color */
	static ÑColor			white;			/**< White color */
	static ÑColor			red;			/**< Red color */

	uint8			r;			/**< Red component of color */
	uint8			g;			/**< Green component of color */
	uint8			b;			/**< Blue component of color */
	uint8			a;			/**< Alpha component of color */
};

//
// Serialization
//

FORCEINLINE CArchive& operator<<( CArchive& InArchive, ÑColor& InValue )
{
	InArchive << InValue.r;
	InArchive << InValue.g;
	InArchive << InValue.b;
	InArchive << InValue.a;
	return InArchive;
}

FORCEINLINE CArchive& operator<<( CArchive& InArchive, const ÑColor& InValue )
{
	InArchive << InValue.r;
	InArchive << InValue.g;
	InArchive << InValue.b;
	InArchive << InValue.a;
	return InArchive;
}

#endif // !COLOR_H
