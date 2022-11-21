/**
 * @file
 * @addtogroup Core Core
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef BOX_H
#define BOX_H

#include "Math/Math.h"

/**
 * @ingroup Core
 * @brief A rectangular minimum bounding volume
 */
class CBox
{
public:
	/**
	 * Constructor
	 */
	FORCEINLINE CBox()
		: bIsValid( false )
	{}

	/**
	 * Constructor
	 * 
	 * @param InMin Min position
	 * @param InMax Max position
	 */
	FORCEINLINE CBox( const Vector& InMin, const Vector& InMax )
		: minLocation( InMin ), maxLocation( InMax ), bIsValid( true )
	{}

	/**
	 * Utility function to build an AABB from InLocation and InSize
	 * 
	 * @param InLocation Location of AABB
	 * @param InSize Size of AABB
	 * @return Return created AABB
	 */
	static FORCEINLINE CBox BuildAABB( const Vector& InLocation, const Vector& InSize )
	{
		return CBox( InLocation - InSize, InLocation + InSize );
	}

	/**
	 * Get min
	 * @return Return min
	 */
	FORCEINLINE const Vector& GetMin() const
	{
		return minLocation;
	}

	/**
	 * Get max
	 * @return Return max
	 */
	FORCEINLINE const Vector& GetMax() const
	{
		return maxLocation;
	}

	/**
	 * Is valid AABB
	 * @return Return true if AABB is valid, else returning false
	 */
	FORCEINLINE bool IsValid() const
	{
		return bIsValid;
	}

private:
	Vector			minLocation;		/**< Min position */
	Vector			maxLocation;		/**< Max position */
	bool			bIsValid;			/**< Is valid box */
};

#endif // !BOX_H