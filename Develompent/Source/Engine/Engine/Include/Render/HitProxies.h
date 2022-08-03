/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef HITPROXIES_H
#define HITPROXIES_H

#include "LEBuild.h"

#if ENABLE_HITPROXY
#include "Math/Color.h"

/**
 * @ingroup Engine
 * Encapsulates a hit proxy ID
 */
class CHitProxyId
{
public:
	/**
	 * Default constructor
	 */
	FORCEINLINE CHitProxyId()
		: index( INDEX_NONE )
	{}

	/**
	 * Initialization constructor
	 * @param	InIndex	Index
	 */
	FORCEINLINE CHitProxyId( uint32 InIndex )
		: index( InIndex )
	{}

	/**
	 * Color conversion constructor
	 * @param	InColor	Color
	 */
	FORCEINLINE CHitProxyId( const CColor& InColor )
	{
		SetIndex( InColor );
	}

	/**
	 * Get color
	 * @return Return color from index
	 */
	FORCEINLINE CColor GetColor() const
	{
		return CColor( ( index >> 0 ) & 0xFF, ( index >> 8 ) & 0xFF, ( index >> 16 ) & 0xFF, 0 );
	}

	/**
	 * Set index
	 * @param InNewIndex	New index
	 */
	FORCEINLINE void SetIndex( uint32 InNewIndex )
	{
		index = InNewIndex;
	}

	/**
	 * Set index from color
	 * @param InColor	New index in color format
	 */
	FORCEINLINE void SetIndex( const CColor& InColor )
	{
		index = ( ( int32 )InColor.r << 0 ) | ( ( int32 )InColor.g << 8 ) | ( ( int32 )InColor.b << 16 );
	}

	/**
	 * Get index
	 * @return Return index. If is not valid returning INDEX_NONE
	 */
	FORCEINLINE uint32 GetIndex() const
	{
		return index;
	}

	/**
	 * Is valid
	 * @return Return TRUE if hit proxy id is valid, else returning FALSE
	 */
	FORCEINLINE bool IsValid() const
	{
		return index != INDEX_NONE;
	}

private:
	uint32		index;		/**< A uniquely identifying index for the hit proxy */
};

#endif // ENABLE_HITPROXY
#endif // !HITPROXIES_H