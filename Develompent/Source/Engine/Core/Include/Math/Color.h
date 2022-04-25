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
class FColor
{
public:
	friend FArchive& operator<<( FArchive& InArchive, FColor& InValue );
	friend FArchive& operator<<( FArchive& InArchive, const FColor& InValue );

	/**
	 * @brief Default constructor
	 */
	FORCEINLINE					FColor() :
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
	FORCEINLINE					FColor( uint8 InRed, uint8 InGreen, uint8 InBlue, uint8 InAlpha = 255 )
	{
		Set( InRed, InGreen, InBlue, InAlpha );
	}

	/**
	 * @brief Constructor
	 * @param InColor		Color in uint32 type
	 */
	FORCEINLINE FColor( uint32 InColor )
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
		SetR( InRed );
		SetG( InGreen );
		SetB( InBlue );
		SetA( InAlpha );
	}
	
	/**
	 * @brief Set red component
	 * @param[in] InRed Red component of color
	 */
	FORCEINLINE void			SetR( uint8 InRed )
	{
		r = InRed;
	}

	/**
	 * @brief Set green component
	 * @param[in] InGreen Green component of color
	 */
	FORCEINLINE void			SetG( uint8 InGreen )
	{
		g = InGreen;
	}

	/**
	 * @brief Set blue component
	 * @param[in] InBlue Blue component of color
	 */
	FORCEINLINE void			SetB( uint8 InBlue )
	{
		b = InBlue;
	}

	/**
	 * @brief Set alpha component
	 * @param[in] InAlpha Alpha component of color
	 */
	FORCEINLINE void			SetA( uint8 InAlpha )
	{
		a = InAlpha;
	}

	/**
	 * @brief Get red component
	 * @return Red component
	 */
	FORCEINLINE uint8			GetR() const
	{
		return r;
	}

	/**
	 * @brief Get green component
	 * @return Green component
	 */
	FORCEINLINE uint8			GetG() const
	{
		return g;
	}

	/**
	 * @brief Get blue component
	 * @return blue component
	 */
	FORCEINLINE uint8			GetB() const
	{
		return b;
	}

	/**
	 * @brief Get alpha component
	 * @return Alpha component
	 */
	FORCEINLINE uint8			GetA() const
	{
		return a;
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
	 * @brief Convert FColor to FVector4D
	 * @return Return converted FColor to FVector4D
	 */
	FORCEINLINE FVector4D ToVector4D() const
	{
		return FVector4D( r, g, b, a );
	}

	/**
	 * @brief Convert FColor to normalized FVector4D (in range 0 -> 1)
	 * @return Return converted FColor to normalized FVector4D (in range 0 -> 1)
	 */
	FORCEINLINE FVector4D ToNormalizedVector4D() const
	{
		return FVector4D( r / 255.f, g / 255.f, b / 255.f, a / 255.f );
	}

	/**
	 * @brief Override operator ==
	 */
	FORCEINLINE bool operator==( const FColor& InRight ) const
	{
		return r == InRight.r && g == InRight.g && b == InRight.b && a == InRight.a;
	}

	static FColor			black;			/**< Black color */
	static FColor			white;			/**< White color */
	static FColor			red;			/**< Red color */

private:
	uint8			r;			/**< Red component of color */
	uint8			g;			/**< Green component of color */
	uint8			b;			/**< Blue component of color */
	uint8			a;			/**< Alpha component of color */
};

//
// Serialization
//

FORCEINLINE FArchive& operator<<( FArchive& InArchive, FColor& InValue )
{
	InArchive << InValue.r;
	InArchive << InValue.g;
	InArchive << InValue.b;
	InArchive << InValue.a;
	return InArchive;
}

FORCEINLINE FArchive& operator<<( FArchive& InArchive, const FColor& InValue )
{
	InArchive << InValue.r;
	InArchive << InValue.g;
	InArchive << InValue.b;
	InArchive << InValue.a;
	return InArchive;
}

#endif // !COLOR_H
