/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef ALIGHT_H
#define ALIGHT_H

#include "Actors/Actor.h"

 /**
  * @ingroup Engine
  * Actor of base light
  */
class ALight : public AActor
{
	DECLARE_CLASS_NO_CTOR( ALight, AActor, CLASS_Intrinsic | CLASS_Abstract, 0, TEXT( "Engine" ) )
};

#endif // !ALIGHT_H