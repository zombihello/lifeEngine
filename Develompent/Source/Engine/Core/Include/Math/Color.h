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

/**
 * @ingroup Core
 * @brief The class for work with color
 */
class FColor
{
public:
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

	static FColor			black;			/**< Black color */
	static FColor			white;			/**< White color */

private:
	uint8			r;			/**< Red component of color */
	uint8			g;			/**< Green component of color */
	uint8			b;			/**< Blue component of color */
	uint8			a;			/**< Alpha component of color */
};

#endif // !COLOR_H
