/**
 * @file
 * @addtogroup Physics Physics
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef PHYSICSTYPES_H
#define PHYSICSTYPES_H

#include <string>

#include "Math/Math.h"
#include "System/Package.h"
#include "System/Archive.h"

/**
 * @ingroup Physics
 * @brief Enumeration of type shape collision
 */
enum ECollisionShape
{
	CS_Box			/**< Collision box */
};

/**
 * @ingroup Physics
 * @brief Enumeration of collision channel
 */
enum ECollisionChannel
{
	CC_WorldStatic,		/**< Static world */
	CC_Visibility,		/**< Visibility */
	CC_Character,		/**< Character */
	CC_Max				/**< Count collision channels */
};

/**
 * @ingroup Physics
 * @brief Enumeration indicating how each type should respond
 */
enum ECollisionResponse
{
	CR_Ignore,			/**< Ignore collision */
	CR_Overlap,			/**< Overlap collision */
	CR_Block,			/**< Block collision */
	CR_Max				/**< Count responses */
};

/**
 * @ingroup Physics
 * @brief Enumeration of surface type
 */
enum ESurfaceType
{
	ST_Default,			/**< Default surface type */
	ST_Surface1,		/**< Custom surface 1 */
	ST_Surface2,		/**< Custom surface 2 */
	ST_Surface3,		/**< Custom surface 3 */
	ST_Surface4,		/**< Custom surface 4 */
	ST_Surface5,		/**< Custom surface 5 */
	ST_Surface6,		/**< Custom surface 6 */
	ST_Surface7,		/**< Custom surface 7 */
	ST_Surface8,		/**< Custom surface 8 */
	ST_Surface9,		/**< Custom surface 9 */
	ST_Surface10,		/**< Custom surface 10 */
	ST_Max				/**< Count surface types */
};

/**
 * @ingroup Physics
 * @brief Struct of collision profile
 */
struct CollisionProfile
{
	/**
	 * @brief Constructor
	 */
	CollisionProfile()
		: name( TEXT( "Unknown" ) )
		, objectType( CC_WorldStatic )
	{
		Sys_Memzero( responses, sizeof( ECollisionResponse ) * CC_Max );
	}

	/**
	 * Overload operator << for serialize
	 */
	friend CArchive& operator<<( CArchive& InAr, CollisionProfile*& InCollisionProfile );

	/**
	 * Overload operator << for serialize
	 */
	friend CArchive& operator<<( CArchive& InAr, const CollisionProfile*& InCollisionProfile );

	std::wstring				name;					/**< Profile name */
	ECollisionChannel			objectType;				/**< Object type */
	ECollisionResponse			responses[ CC_Max ];	/**< Map of responses each collision channel */

	static const std::wstring	noCollision_ProfileName;	/**< No collision profile name */
	static const std::wstring	blockAll_ProfileName;		/**< Block all profile name */
};

/**
 * @ingroup Physics
 * @brief Struct of result ray cast
 */
struct HitResult
{
	/**
	 * @brief Constructor
	 */
	HitResult()
		: actor( nullptr )
		, component( nullptr )
		, impactNormal( Math::vectorZero )
		, impactPoint( Math::vectorZero )
		, physMaterial( nullptr )
	{ }

	class AActor*						actor;			/**< Actor hit by the trace */
	class CPrimitiveComponent*			component;		/**< PrimitiveComponent hit by the trace */
	Vector								impactNormal;	/**< Normal of the hit in world space, for the object that was hit by the sweep, if any */
	Vector								impactPoint;	/**< Location in world space of the actual contact of the trace shape (box, sphere, ray, etc) with the impacted object */
	TSharedPtr<class CPhysicsMaterial>	physMaterial;	/**< Physical material that was hit */
};

/**
 * @ingroup Physics
 * @brief Structure that defines parameters passed into collision function 
 */
struct CollisionQueryParams
{
	/**
	 * @brief Constructor
	 */
	CollisionQueryParams()
		: bReturnPhysicalMaterial( false )
	{}

	bool							bReturnPhysicalMaterial;		/**< Whether we want to include the physical material in the results */
	static CollisionQueryParams	defaultQueryParam;				/**< Static variable for default data to be used without reconstructing everytime */
};

//
// Serialize
//

FORCEINLINE CArchive& operator<<( CArchive& InAr, ESurfaceType& InSurfaceType )
{
	InAr.Serialize( &InSurfaceType, sizeof( ESurfaceType ) );
	return InAr;
}

FORCEINLINE CArchive& operator<<( CArchive& InAr, const ESurfaceType& InSurfaceType )
{
	Assert( InAr.IsSaving() );
	InAr.Serialize( ( void* ) &InSurfaceType, sizeof( ESurfaceType ) );
	return InAr;
}

#endif // !PHYSICSTYPES_H