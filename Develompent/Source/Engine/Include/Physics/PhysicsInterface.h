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

#ifndef PHYSICSINTERFACE_H
#define PHYSICSINTERFACE_H

#ifndef WITH_PHYSX
	#define WITH_PHYSX	0
#endif // !WITH_PHYSX

#ifndef WITH_BOX2D
	#define WITH_BOX2D	0
#endif // !WITH_BOX2D

#include "Physics/PhysicsInterfaceTypes.h"

// PhysX implementation
#if WITH_PHYSX
#include "Physics/PhysicsInterfacePhysX.h"
#include "Physics/Misc/PhysXGlobals.h"
#include "Physics/System/PhysXScene.h"

typedef PhysicsInterfacePhysX				CPhysicsInterface;
typedef PhysicsMaterialHandlePhysX			PhysicsMaterialHandle_t;
typedef PhysicsShapeHandlePhysX				PhysicsShapeHandle_t;
typedef PhysicsActorHandlePhysX				PhysicsActorHandle_t;

// Box 2D implementation
#elif WITH_BOX2D
#include "Physics/PhysicsInterfaceBox2D.h"
#include "Physics/Misc/Box2DGlobals.h"
#include "Physics/System/Box2DScene.h"

typedef PhysicsInterfaceBox2D				CPhysicsInterface;
typedef PhysicsMaterialHandleBox2D			PhysicsMaterialHandle_t;
typedef PhysicsShapeHandleBox2D				PhysicsShapeHandle_t;
typedef PhysicsActorHandleBox2D				PhysicsActorHandle_t;

#else
	static_assert( false, "A physics engine interface must be defined to build" );
#endif // !WITH_PHYSX

#endif // !PHYSICSINTERFACE_H