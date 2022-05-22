/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef SPRITECOMPONENT_H
#define SPRITECOMPONENT_H

#include "Components/PrimitiveComponent.h"
#include "Render/Scene.h"
#include "Render/Sprite.h"

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
class LSpriteComponent : public LPrimitiveComponent
{
	DECLARE_CLASS( LSpriteComponent, LPrimitiveComponent )

public:
    /**
     * @brief Constructor
     */
    LSpriteComponent();

	/**
	 * @brief Adds mesh batches for draw in scene
	 *
	 * @param InSceneView Current view of scene
	 */
	virtual void AddToDrawList( const class FSceneView& InSceneView ) override;

	/**
	 * @brief Serialize component
	 * @param[in] InArchive Archive for serialize
	 */
	virtual void Serialize( class FArchive& InArchive ) override;

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

	/**
	 * @brief Set texture rect
	 * @param InTextureRect Texture rect
	 */
	FORCEINLINE void SetTextureRect( const FRectFloat& InTextureRect )
	{
		sprite->SetTextureRect( InTextureRect );
		bIsDirtyDrawingPolicyLink = true;
	}

	/**
	 * @brief Get texture rect
	 * @return Return texture rect
	 */
	FORCEINLINE const FRectFloat& GetTextureRect() const
	{
		return sprite->GetTextureRect();
	}

	/**
	 * @brief Set sprite size
	 * @param InSpriteSize Sprite size
	 */
	FORCEINLINE void SetSpriteSize( const FVector2D& InSpriteSize )
	{
		sprite->SetSpriteSize( InSpriteSize );
		bIsDirtyDrawingPolicyLink = true;
	}

	/**
	 * @brief Get sprite size
	 * @return Return sprite size
	 */
	FORCEINLINE const FVector2D& GetSpriteSize() const
	{
		return sprite->GetSpriteSize();
	}

	/**
	 * @brief Set material
	 * @param InMaterial Material
	 */
	FORCEINLINE void SetMaterial( const TAssetHandle<FMaterial> InMaterial )
	{
		sprite->SetMaterial( InMaterial );
		bIsDirtyDrawingPolicyLink = true;
	}

	/**
	 * @brief Set flip by vertical
	 * @param InFlipVertical Is need flip sprite by vertical
	 */
	FORCEINLINE void SetFlipVertical( bool InFlipVertical )
	{
		sprite->SetFlipVertical( InFlipVertical );
		bFlipVertical = InFlipVertical;
	}

	/**
	 * @brief Set flip by horizontal
	 * @param InFlipHorizontal Is need flip sprite by horizontal
	 */
	FORCEINLINE void SetFlipHorizontal( bool InFlipHorizontal )
	{
		sprite->SetFlipHorizontal( InFlipHorizontal );
		bFlipHorizontal = InFlipHorizontal;
	}

	/**
	 * @brief Get material
	 * @return Return pointer to material. If not setted returning nullptr
	 */
	FORCEINLINE TAssetHandle<FMaterial> GetMaterial() const
	{
		return sprite->GetMaterial();
	}

	/**
	 * @brief Is fliped by vertical
	 * @return Return TRUE if sprite fliped by vertical
	 */
	FORCEINLINE bool IsFlipedVertical() const
	{
		return bFlipVertical;
	}

	/**
	 * @brief Is fliped by horizontal
	 * @return Return TRUE if sprite fliped by horizontal
	 */
	FORCEINLINE bool IsFlipedHorizontal() const
	{
		return bFlipHorizontal;
	}

private:
	/**
	 * @brief Typedef of drawing policy link
	 */
	typedef FMeshDrawList< FStaticMeshDrawPolicy >::FDrawingPolicyLink			FDrawingPolicyLink;

	/**
	 * @brief Typedef of reference on drawing policy link in scene
	 */
	typedef FMeshDrawList< FStaticMeshDrawPolicy >::FDrawingPolicyLinkRef		FDrawingPolicyLinkRef;

	/**
	 * @brief Calculate transformation matrix
	 *
	 * @param InSceneView Current view of scene
	 * @param OutResult Output calculated transformation matrix
	 */
	void CalcTransformationMatrix( const class FSceneView& InSceneView, FMatrix& OutResult ) const;

	/**
	 * @brief Adds a draw policy link in SDGs
	 */
	virtual void LinkDrawList() override;

	/**
	 * @brief Removes a draw policy link from SDGs
	 */
	virtual void UnlinkDrawList() override;

	bool						bFlipVertical;					/**< Is need flip sprite by vertical */
	bool						bFlipHorizontal;				/**< Is need flip sprite by horizontal */
    ESpriteType					type;							/**< Sprite type */
	FSpriteRef					sprite;							/**< Sprite mesh */
	FDrawingPolicyLinkRef		drawingPolicyLink;				/**< Reference to drawing policy link in scene */
	const FMeshBatch*			meshBatchLink;					/**< Reference to mesh batch in drawing policy link */
};

//
// Serialization
//

/**
 * Overload operator << for serialize ESpriteType
 */
FORCEINLINE FArchive& operator<<( FArchive& InArchive, ESpriteType& InValue )
{
	InArchive.Serialize( &InValue, sizeof( ESpriteType ) );
	return InArchive;
}

/**
 * Overload operator << for serialize ESpriteType
 */
FORCEINLINE FArchive& operator<<( FArchive& InArchive, const ESpriteType& InValue )
{
	check( InArchive.IsSaving() );
	InArchive.Serialize( ( void* ) &InValue, sizeof( ESpriteType ) );
	return InArchive;
}

#endif // !SPRITECOMPONENT_H
