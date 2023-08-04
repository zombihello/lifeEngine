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
#include "Components/BoxComponent.h"

 /**
  * @ingroup Engine
  * Actor of sprite
  */
class ASprite : public AActor
{
    DECLARE_CLASS( ASprite, AActor, 0, 0 )

public:
    /**
     * Constructor
     */
    ASprite();

    /**
     * Destructor
     */
    virtual ~ASprite();

    /**
     * Get sprite component
     * @return Return pointer to sprite component
     */
    FORCEINLINE TRefCountPtr<CSpriteComponent> GetSpriteComponent() const
    {
        return spriteComponent;
    }

#if WITH_EDITOR
    /**
     * @brief Get path to icon of actor for exploer level in WorldEd
     * @return Return path to actor icon from Sys_BaseDir()
     */
    virtual std::wstring GetActorIcon() const override;
#endif // WITH_EDITOR

private:
    TRefCountPtr<CSpriteComponent>			spriteComponent;		/**< Sprite component */
};

#endif // !ASPRITE_H
