/**
 * @file
 * @addtogroup Physics Physics
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef PHYSICSSBOXGEOMETRY_H
#define PHYSICSSBOXGEOMETRY_H

#include "Math/Math.h"
#include "Math/Rotator.h"
#include "System/PhysicsShapeGeometry.h"

/**
 * @ingroup Physics
 * @brief Box shape used for collision
 */
struct FPhysicsBoxGeometry : public FPhysicsShapeGeometry
{
public:
	/**
	 * @brief Constructor
	 */
	FORCEINLINE FPhysicsBoxGeometry()
		: FPhysicsShapeGeometry( CS_Box )
		, center( FMath::vectorZero )
		, rotation( FMath::rotatorZero )
		, extent( FMath::vectorZero )
	{}

	/**
	 * @brief Constructor
	 * @param InSize Size by X, Y and Z
	 */
	FORCEINLINE FPhysicsBoxGeometry( float InSize )
		: FPhysicsShapeGeometry( CS_Box )
		, center( FMath::vectorZero )
		, rotation( FMath::rotatorZero )
		, extent( InSize, InSize, InSize )
	{}

	/**
	 * @brief Constructor
	 * 
	 * @param InSizeX Size by X
	 * @param InSizeY Size by Y
	 * @param InSizeZ Size by Z
	 */
	FORCEINLINE FPhysicsBoxGeometry( float InSizeX, float InSizeY, float InSizeZ )
		: FPhysicsShapeGeometry( CS_Box )
		, center( FMath::vectorZero )
		, rotation( FMath::rotatorZero )
		, extent( InSizeX, InSizeY, InSizeZ )
	{}

	/**
	 * Serialize
	 *
	 * @param[in] InArchive Archive
	 */
	virtual void Serialize( class FArchive& InArchive ) override;

	/**
	 * Overload operator << for serialize
	 */
	FORCEINLINE friend FArchive& operator<<( FArchive& InAr, FPhysicsBoxGeometry& InBoxGometry )
	{
		InBoxGometry.Serialize( InAr );
		return InAr;
	}

	/**
	 * Overload operator << for serialize
	 */
	FORCEINLINE friend FArchive& operator<<( FArchive& InAr, const FPhysicsBoxGeometry& InBoxGometry )
	{
		check( InAr.IsSaving() );
		( ( FPhysicsBoxGeometry& )InBoxGometry ).Serialize( InAr );
		return InAr;
	}

	/**
	 * Overload operator ==
	 */
	FORCEINLINE bool operator==( const FPhysicsBoxGeometry& InBoxGometry ) const
	{
		return
			collisionShape == InBoxGometry.collisionShape &&
			material == InBoxGometry.material &&
			center == InBoxGometry.center &&
			rotation == InBoxGometry.rotation &&
			extent == InBoxGometry.extent;
	}

	FVector			center;		/**< Position of the box's origin */
	FRotator		rotation;	/**< Rotation of the box */
	FVector			extent;		/**< Extent of the box */

protected:
	/**
	 * @brief Init physics shape
	 */
	virtual void InitPhysicsShape() const override;
};

#endif // !PHYSICSSBOXGEOMETRY_H