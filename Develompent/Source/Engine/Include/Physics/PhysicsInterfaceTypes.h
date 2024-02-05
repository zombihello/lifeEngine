/**
 * @file
 * @addtogroup Physics Physics
 *
 * ************************************************************
 *                  This file is part of:
 *                      LIFEENGINE
 *          https://github.com/zombihello/lifeEngine
 * ************************************************************
 * Copyright (C) 2024 Yehor Pohuliaka.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef PHYSICSINTERFACETYPES_H
#define PHYSICSINTERFACETYPES_H

#include "Core/Math/Transform.h"

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
struct ActorCreationParams
{
	/**
	 * @brief Constructor
	 */
	ActorCreationParams()
		: bStatic( false )
		, bQueryOnly( false )
		, bEnableGravity( false )
		, bSimulatePhysics( false )
		, bStartAwake( false )
		, lockFlags( BLF_None )
		, mass( 0.f )
		, debugName( nullptr )
	{}

	CTransform		initialTM;			/**< Initial transform */
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