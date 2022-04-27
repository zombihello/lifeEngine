/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef ENGINEDEFINES_H
#define ENGINEDEFINES_H

/**
 * @ingroup Engine
 * Maximum size of the world
 */
#define WORLD_MAX					524288.0

/**
 * @ingroup Engine
 * Half the maximum size of the world
 */
#define HALF_WORLD_MAX				( WORLD_MAX * 0.5 )

 /**
  * @ingroup Engine
  * Half the maximum size of the world - 1
  */
#define HALF_WORLD_MAX1				( HALF_WORLD_MAX - 1.0 )

/**
 * @ingroup Engine
 * Limit of 2D viewport zoom in
 */
#define MIN_ORTHOZOOM				250.0

/**
 * @ingroup Engine
 * Limit of 2D viewport zoom out
 */
#define MAX_ORTHOZOOM				16000000.0

#endif // !ENGINEDEFINES_H