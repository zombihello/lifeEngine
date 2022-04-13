/**
 * @file
 * @addtogroup Physics Physics
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef PHYSICSINTERFACETYPES_H
#define PHYSICSINTERFACETYPES_H

#include "Math/Transform.h"

/**
 * @ingroup Physics
 * @brief Enumeration of rigid body lock flag
 */
enum EBodyLockFlag
{
	BLF_None		= 0,		/**< Nope flag */
	BLF_LockMoveX	= 1 << 0,	/**< Lock moving by X axis */
	BLF_LockMoveY	= 1 << 1,	/**< Lock moving by Y axis */
	BLF_LockMoveZ	= 1 << 2,	/**< Lock moving by Z axis */
	BLF_LockRotateX	= 1 << 3,	/**< Lock rotating by X axis */
	BLF_LockRotateY	= 1 << 4,	/**< Lock rotating by Y axis */
	BLF_LockRotateZ	= 1 << 5	/**< Lock rotating by Z axis */
};

/**
 * @ingroup Physics
 * @brief Parameters to create physics actor
 */
struct FActorCreationParams
{
	/**
	 * @brief Constructor
	 */
	FActorCreationParams()
		: bStatic( false )
		, bQueryOnly( false )
		, bEnableGravity( false )
		, bSimulatePhysics( false )
		, bStartAwake( false )
		, lockFlags( BLF_None )
		, mass( 0.f )
		, debugName( nullptr )
	{}

	FTransform		initialTM;			/**< Initial transform */
	bool			bStatic;			/**< Is static actor */
	bool			bQueryOnly;			/**< Query only */
	bool			bEnableGravity;		/**< Enable gravity */
	bool			bSimulatePhysics;	/**< Need simulate physics */
	bool			bStartAwake;		/**< Start awake */
	uint32			lockFlags;			/**< Lock flags (see EBodyLockFlag) */
	float			mass;				/**< Mass */
	char*			debugName;			/**< Debug name */
};

#endif // !PHYSICSINTERFACETYPES_H