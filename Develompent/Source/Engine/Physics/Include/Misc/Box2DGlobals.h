/**
 * @file
 * @addtogroup Physics Physics
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef BOX2DGLOBALS_H
#define BOX2DGLOBALS_H

#if WITH_BOX2D
#include <box2d/box2d.h>

/**
 * @ingroup Physics
 * Physics scene
 */
extern class CBox2DScene		GPhysicsScene;
#endif // WITH_BOX2D

#endif // !BOX2DGLOBALS_H