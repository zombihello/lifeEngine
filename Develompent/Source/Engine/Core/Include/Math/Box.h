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
class FBox
{
public:
	/**
	 * Constructor
	 */
	FORCEINLINE FBox()
		: bIsValid( false )
	{}

	/**
	 * Constructor
	 * 
	 * @param InMin Min position
	 * @param InMax Max position
	 */
	FORCEINLINE FBox( const FVector& InMin, const FVector& InMax )
		: minLocation( InMin ), maxLocation( InMax ), bIsValid( true )
	{}

	/**
	 * Utility function to build an AABB from InLocation and InSize
	 * 
	 * @param InLocation Location of AABB
	 * @param InSize Size of AABB
	 * @return Return created AABB
	 */
	static FORCEINLINE FBox BuildAABB( const FVector& InLocation, const FVector& InSize )
	{
		return FBox( InLocation - InSize, InLocation + InSize );
	}

	/**
	 * Get min
	 * @return Return min
	 */
	FORCEINLINE const FVector& GetMin() const
	{
		return minLocation;
	}

	/**
	 * Get max
	 * @return Return max
	 */
	FORCEINLINE const FVector& GetMax() const
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
	FVector			minLocation;		/**< Min position */
	FVector			maxLocation;		/**< Max position */
	bool			bIsValid;			/**< Is valid box */
};

#endif // !BOX_H