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
#include "System/PhysicsShapeGeometry.h"

/**
 * @ingroup Physics
 * @brief Box shape used for collision
 */
struct PhysicsBoxGeometry : public PhysicsShapeGeometry
{
public:
	/**
	 * @brief Constructor
	 */
	FORCEINLINE PhysicsBoxGeometry()
		: PhysicsShapeGeometry( CS_Box )
		, location( Math::vectorZero )
		, rotation( Math::quaternionZero )
		, extent( Math::vectorZero )
	{}

	/**
	 * @brief Constructor
	 * @param InSize Size by X, Y and Z
	 */
	FORCEINLINE PhysicsBoxGeometry( float InSize )
		: PhysicsShapeGeometry( CS_Box )
		, location( Math::vectorZero )
		, rotation( Math::quaternionZero )
		, extent( InSize, InSize, InSize )
	{}

	/**
	 * @brief Constructor
	 * 
	 * @param InSizeX Size by X
	 * @param InSizeY Size by Y
	 * @param InSizeZ Size by Z
	 */
	FORCEINLINE PhysicsBoxGeometry( float InSizeX, float InSizeY, float InSizeZ )
		: PhysicsShapeGeometry( CS_Box )
		, location( Math::vectorZero )
		, rotation( Math::quaternionZero )
		, extent( InSizeX, InSizeY, InSizeZ )
	{}

	/**
	 * Serialize
	 *
	 * @param[in] InArchive Archive
	 */
	virtual void Serialize( class CArchive& InArchive ) override;

	/**
	 * Overload operator << for serialize
	 */
	FORCEINLINE friend CArchive& operator<<( CArchive& InAr, PhysicsBoxGeometry& InBoxGometry )
	{
		InBoxGometry.Serialize( InAr );
		return InAr;
	}

	/**
	 * Overload operator << for serialize
	 */
	FORCEINLINE friend CArchive& operator<<( CArchive& InAr, const PhysicsBoxGeometry& InBoxGometry )
	{
		Assert( InAr.IsSaving() );
		( ( PhysicsBoxGeometry& )InBoxGometry ).Serialize( InAr );
		return InAr;
	}

	/**
	 * Overload operator ==
	 */
	FORCEINLINE bool operator==( const PhysicsBoxGeometry& InBoxGometry ) const
	{
		return
			collisionShape == InBoxGometry.collisionShape &&
			material == InBoxGometry.material &&
			location == InBoxGometry.location &&
			rotation == InBoxGometry.rotation &&
			extent == InBoxGometry.extent;
	}

	Vector			location;	/**< Position of the box's origin */
	Quaternion		rotation;	/**< Rotation of the box */
	Vector			extent;		/**< Extent of the box */

protected:
	/**
	 * @brief Init physics shape
	 */
	virtual void InitPhysicsShape() const override;
};

#endif // !PHYSICSSBOXGEOMETRY_H