/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef ENGINETYPES_H
#define ENGINETYPES_H

#include "Misc/RefCountPtr.h"

/**
 * @ingroup Engine Engine
 * @brief Reference to actor component
 */
typedef TRefCountPtr< class LActorComponent >			LActorComponentRef;

/**
 * @ingroup Engine Engine
 * @brief Reference to actor
 */
typedef TRefCountPtr< class AActor >					AActorRef;

#endif // !ENGINETYPES_H