/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef ASPRITE_H
#define ASPRITE_H

#include <string>

#include "Actors/Actor.h"
#include "Components/SpriteComponent.h"

 /**
  * @ingroup Engine
  * Actor of sprite
  */
class ASprite : public AActor
{
    DECLARE_CLASS( ASprite, AActor )

public:
    /**
     * Constructor
     */
    ASprite();

    /**
     * Destructor
     */
    virtual ~ASprite();

private:
    TRefCountPtr< LSpriteComponent >			spriteComponent;		/**< Sprite component */
};

#endif // !ASPRITE_H
