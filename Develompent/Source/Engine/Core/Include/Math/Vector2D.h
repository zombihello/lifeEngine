/**
 * @file
 * @addtogroup Core Core
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef CORE_VECTOR2D_H
#define CORE_VECTOR2D_H

/**
 * @ingroup Core
 * Class for work with vector 2D
 */
struct FVector2D
{
	/**
	 * Constructor
	 * 
	 * @param[in] InX X cord
	 * @param[in] InY Y cord
	 */
	FVector2D( float InX = 0.f, float InY = 0.f ) :
		x( InX ),
		y( InY )
	{}

	float		x;		/**< X cord */
	float		y;		/**< Y cord */
};

#endif // !CORE_VECTOR2D_H