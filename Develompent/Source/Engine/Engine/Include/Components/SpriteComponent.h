/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef SPRITECOMPONENT_H
#define SPRITECOMPONENT_H

#include "Containers/EnumAsByte.h"
#include "Components/PrimitiveComponent.h"
#include "Render/Scene.h"
#include "Render/Sprite.h"

#if ENABLE_HITPROXY
#include "Render/SceneHitProxyRendering.h"
#endif // ENABLE_HITPROXY

/**
 * @ingroup Engine
 * @brief Enumeration of sprite types
 */
enum ESpriteType
{
    ST_Static,                  /**< Static sprite */
    ST_Rotating,                /**< Rotating sprite to player camera */
    ST_RotatingOnlyVertical     /**< Rotating sprite to player camera only by vertical */
};

DECLARE_ENUM( ESpriteType, TEXT( "Engine" ) )
#define FOREACH_ENUM_SPRITETYPE( X ) \
	X( ST_Static ) \
	X( ST_Rotating ) \
	X( ST_RotatingOnlyVertical )

 /**
  * @ingroup Engine
  * @brief Component for work with sprite
  */
class CSpriteComponent : public CPrimitiveComponent
{
	DECLARE_CLASS_INTRINSIC( CSpriteComponent, CPrimitiveComponent, 0, 0, TEXT( "Engine" ) )

public:
    /**
     * @brief Constructor
     */
    CSpriteComponent();

	/**
	 * @brief Adds mesh batches for draw in scene
	 *
	 * @param InSceneView Current view of scene
	 */
	virtual void AddToDrawList( const class CSceneView& InSceneView ) override;

	/**
	 * @brief Serialize component
	 * @param[in] InArchive Archive for serialize
	 */
	virtual void Serialize( class CArchive& InArchive ) override;

	/**
	 * @brief Do any object-specific cleanup required immediately after loading an object
	 * @note This is not called for newly-created objects, and by default will always execute on the game thread
	 */
	virtual void PostLoad() override;

#if WITH_EDITOR
	/**
	 * @brief Function called by the editor when property is changed
	 * @param InPropertyChangedEvenet    Property changed event
	 */
	virtual void PostEditChangeProperty( const PropertyChangedEvenet& InPropertyChangedEvenet ) override;
#endif // WITH_EDITOR

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
	FORCEINLINE void SetTextureRect( const RectFloat_t& InTextureRect )
	{
		sprite->SetTextureRect( InTextureRect );
		bIsDirtyDrawingPolicyLink = true;
	}

	/**
	 * @brief Get texture rect
	 * @return Return texture rect
	 */
	FORCEINLINE const RectFloat_t& GetTextureRect() const
	{
		return sprite->GetTextureRect();
	}

	/**
	 * @brief Set sprite size
	 * @param InSpriteSize Sprite size
	 */
	FORCEINLINE void SetSpriteSize( const Vector2D& InSpriteSize )
	{
		sprite->SetSpriteSize( InSpriteSize );
		bIsDirtyDrawingPolicyLink = true;
	}

	/**
	 * @brief Get sprite size
	 * @return Return sprite size
	 */
	FORCEINLINE const Vector2D& GetSpriteSize() const
	{
		return sprite->GetSpriteSize();
	}

	/**
	 * @brief Set material
	 * @param InMaterial Material
	 */
	FORCEINLINE void SetMaterial( const TAssetHandle<CMaterial> InMaterial )
	{
		sprite->SetMaterial( InMaterial );
		material = InMaterial;
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
	FORCEINLINE TAssetHandle<CMaterial> GetMaterial() const
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

#if WITH_EDITOR
	/**
	 * @brief Set gizmo component
	 * @param InIsGizmo		Is current component is gizmo?
	 */
	FORCEINLINE void SetGizmo( bool InIsGizmo )
	{
		bIsDirtyDrawingPolicyLink	= true;
		bGizmo						= InIsGizmo;
	}

	/**
	 * @brief Is gizmo component
	 * @return Return TRUE if component is gizmo
	 */
	FORCEINLINE bool IsGizmo() const
	{
		return bGizmo;
	}
#endif // WITH_EDITOR

private:
	/**
	 * @brief Typedef of drawing policy link
	 */
	typedef CMeshDrawList<CMeshDrawingPolicy>::DrawingPolicyLink					DrawingPolicyLink_t;

	/**
	 * @brief Typedef of reference on drawing policy link in scene
	 */
	typedef CMeshDrawList<CMeshDrawingPolicy>::DrawingPolicyLinkRef_t				DrawingPolicyLinkRef_t;

#if WITH_EDITOR
	/**
	 * @brief Typedef of gizmo drawing policy link
	 */
	typedef CMeshDrawList<CMeshDrawingPolicy, false>::DrawingPolicyLink				GizmoDrawingPolicyLink_t;

	/**
	 * @brief Typedef of reference on gizmo drawing policy link in scene
	 */
	typedef CMeshDrawList<CMeshDrawingPolicy, false>::DrawingPolicyLinkRef_t			GizmoDrawingPolicyLinkRef_t;
#endif // WITH_EDITOR

#if ENABLE_HITPROXY
	/**
	 * @brief Typedef of hit proxy drawing policy link
	 */
	typedef CMeshDrawList<CHitProxyDrawingPolicy, false>::DrawingPolicyLink			HitProxyDrawingPolicyLink_t;

	/**
	 * @brief Typedef of reference on hit proxy drawing policy link in scene
	 */
	typedef CMeshDrawList<CHitProxyDrawingPolicy, false>::DrawingPolicyLinkRef_t		HitProxyDrawingPolicyLinkRef_t;
#endif // ENABLE_HITPROXY

	/**
	 * @brief Typedef of depth drawing policy link
	 */
	typedef CMeshDrawList<CDepthDrawingPolicy>::DrawingPolicyLink						DepthDrawingPolicyLink_t;

	/**
	 * @brief Typedef of reference on depth drawing policy link in scene
	 */
	typedef CMeshDrawList<CDepthDrawingPolicy>::DrawingPolicyLinkRef_t					DepthDrawingPolicyLinkRef_t;

	/**
	 * @brief Calculate transformation matrix
	 *
	 * @param InSceneView Current view of scene
	 * @param OutResult Output calculated transformation matrix
	 */
	void CalcTransformationMatrix( const class CSceneView& InSceneView, Matrix& OutResult ) const;

	/**
	 * @brief Adds a draw policy link in SDGs
	 */
	virtual void LinkDrawList() override;

	/**
	 * @brief Removes a draw policy link from SDGs
	 */
	virtual void UnlinkDrawList() override;

#if WITH_EDITOR
	bool								bGizmo;							/**< This sprite component is gizmo */
	GizmoDrawingPolicyLinkRef_t			gizmoDrawingPolicyLink;			/**< Reference to gizmo drawing policy link in scene */
#endif // WITH_EDITOR

	bool								bFlipVertical;					/**< Is need flip sprite by vertical */
	bool								bFlipHorizontal;				/**< Is need flip sprite by horizontal */
    TEnumAsByte<ESpriteType>			type;							/**< Sprite type */
	SpriteRef_t							sprite;							/**< Sprite mesh */
	TAssetHandle<CMaterial>				material;						/**< Sprite material */
	DrawingPolicyLinkRef_t				drawingPolicyLink;				/**< Reference to drawing policy link in scene */
	DepthDrawingPolicyLinkRef_t			depthDrawingPolicyLink;			/**< Reference to depth drawing policy link in scene */
	std::vector<const MeshBatch*>		meshBatchLinks;					/**< Reference to mesh batch in drawing policy link */

#if ENABLE_HITPROXY
	HitProxyDrawingPolicyLinkRef_t		hitProxyDrawingPolicyLink;		/**< Reference to hit proxy drawing policy link in scene */
#endif // ENABLE_HITPROXY
};

//
// Serialization
//

/**
 * Overload operator << for serialize ESpriteType
 */
FORCEINLINE CArchive& operator<<( CArchive& InArchive, ESpriteType& InValue )
{
	InArchive.Serialize( &InValue, sizeof( ESpriteType ) );
	return InArchive;
}

/**
 * Overload operator << for serialize ESpriteType
 */
FORCEINLINE CArchive& operator<<( CArchive& InArchive, const ESpriteType& InValue )
{
	Assert( InArchive.IsSaving() );
	InArchive.Serialize( ( void* ) &InValue, sizeof( ESpriteType ) );
	return InArchive;
}

#endif // !SPRITECOMPONENT_H
