/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef SPRITECOMPONENT_H
#define SPRITECOMPONENT_H

#include "Components/StaticMeshComponent.h"

enum ESpriteType
{
    ST_Static,                  /**< Static sprite */
    ST_Rotating,                /**< Rotating sprite to player camera */
    ST_RotatingOnlyVertical     /**< Rotating sprite to player camera only by vertical */
};

 /**
  * @ingroup Engine
  * @brief Component for work with sprite
  */
class LSpriteComponent : public LStaticMeshComponent
{
    DECLARE_CLASS( LSpriteComponent, LStaticMeshComponent )

public:
    /**
     * @brief Constructor
     */
    LSpriteComponent();

    /**
     * Begins Play for the component.
     * Called when the owning Actor begins play or when the component is created if the Actor has already begun play.
     */
    virtual void BeginPlay() override;

    /**
     * @brief Set sprite type
     *
     * @param InType Sprite type
     */
    FORCEINLINE void SetType( ESpriteType InType )
    {
        type = InType;
    }

    /**
     * @brief Get sprite type
     * @return Return sprite type
     */
    FORCEINLINE ESpriteType GetType() const
    {
        return type;
    }

protected:
    /**
     * @brief Calculate transformation matrix
     *
     * @param InSceneView Current view of scene
     * @return Return calculated transformation matrix
     */
    virtual FMatrix CalcTransformationMatrix( const class FSceneView& InSceneView ) const override;

private:
    ESpriteType         type;       /**< Sprite type */
};

#endif // !SPRITECOMPONENT_H
