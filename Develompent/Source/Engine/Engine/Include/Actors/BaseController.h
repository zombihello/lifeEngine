/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef BASECONTROLLER_H
#define BASECONTROLLER_H

#include "Actors/Actor.h"

 /**
  * @ingroup Engine
  * Base class of all controllers
  */
class ABaseController : public AActor
{
	DECLARE_CLASS( ABaseController, AActor )
};

#endif // !BASECONTROLLER_H