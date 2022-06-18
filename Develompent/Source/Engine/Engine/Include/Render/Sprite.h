/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef SPRITE_H
#define SPRITE_H

#include "RenderResource.h"
#include "RenderUtils.h"
#include "Misc/RefCounted.h"
#include "Render/VertexFactory/SpriteVertexFactory.h"
#include "RHI/BaseBufferRHI.h"
#include "RHI/TypesRHI.h"
#include "Render/Material.h"

/**
 * @ingroup Engine
 * @brief Reference to CSprite
 */
typedef TRefCountPtr< class CSprite >				SpriteRef_t;

/**
 * @ingroup Engine
 * Surface in sprite mesh
 */
struct SSpriteSurface
{
	uint32		baseVertexIndex;		/**< First index vertex in vertex buffer */
	uint32		firstIndex;				/**< First index */
	uint32		numPrimitives;			/**< Number primitives in the surface */
};

/**
 * @ingroup Engine
 * @brief Sprite mesh data for rendering sprites
 */
class CSpriteMesh : public CRenderResource
{
public:
	/**
	 * @brief Initializes the RHI resources used by this resource.
	 * Called when the resource is initialized.
	 * This is only called by the rendering thread.
	 */
	virtual void InitRHI() override;

	/**
	 * @brief Releases the RHI resources used by this resource.
	 * Called when the resource is released.
	 * This is only called by the rendering thread.
	 */
	virtual void ReleaseRHI() override;

	/**
	 * Get surface
	 * @return Return surface info
	 */
	FORCEINLINE SSpriteSurface GetSurface() const
	{
		return SSpriteSurface{ 0, 0, 2 };
	}

	/**
	 * Get RHI vertex buffer
	 * @return Return RHI vertex buffer, if not created return nullptr
	 */
	FORCEINLINE VertexBufferRHIRef_t GetVertexBufferRHI() const
	{
		return vertexBufferRHI;
	}

	/**
	 * Get RHI index buffer
	 * @return Return RHI index buffer, if not created return nullptr
	 */
	FORCEINLINE IndexBufferRHIRef_t GetIndexBufferRHI() const
	{
		return indexBufferRHI;
	}

private:
	VertexBufferRHIRef_t		vertexBufferRHI;		/**< Vertex buffer RHI */
	IndexBufferRHIRef_t			indexBufferRHI;			/**< Index buffer RHI */
};

extern TGlobalResource< CSpriteMesh >		GSpriteMesh;			/**< The global sprite mesh data for rendering sprites */

/**
 * @ingroup Engine
 * @brief Implementation for sprite mesh
 */
class CSprite : public CRenderResource, public CRefCounted
{
public:
	/**
	 * @brief Constructor
	 */
	CSprite();

	/**
	 * @brief Initializes the RHI resources used by this resource.
	 * Called when the resource is initialized.
	 * This is only called by the rendering thread.
	 */
	virtual void InitRHI() override;

	/**
	 * @brief Releases the RHI resources used by this resource.
	 * Called when the resource is released.
	 * This is only called by the rendering thread.
	 */
	virtual void ReleaseRHI() override;

	/**
	 * @brief Set material
	 * @param InMaterial Material
	 */
	FORCEINLINE void SetMaterial( const TAssetHandle<CMaterial>& InMaterial )
	{
		material = InMaterial;
	}

	/**
	 * Get surface
	 * @return Return surface info
	 */
	FORCEINLINE SSpriteSurface GetSurface() const
	{
		return GSpriteMesh.GetSurface();
	}

	/**
	 * @brief Get material
	 * @return Return pointer to material. If not setted returning nullptr
	 */
	FORCEINLINE TAssetHandle<CMaterial> GetMaterial() const
	{
		return material;
	}

	/**
	 * Get vertex factory
	 * @return Return vertex factory
	 */
	FORCEINLINE TRefCountPtr<CSpriteVertexFactory> GetVertexFactory() const
	{
		return vertexFactory;
	}

	/**
	 * Get RHI vertex buffer
	 * @return Return RHI vertex buffer, if not created return nullptr
	 */
	FORCEINLINE VertexBufferRHIRef_t GetVertexBufferRHI() const
	{
		return GSpriteMesh.GetVertexBufferRHI();
	}

	/**
	 * Get RHI index buffer
	 * @return Return RHI index buffer, if not created return nullptr
	 */
	FORCEINLINE IndexBufferRHIRef_t GetIndexBufferRHI() const
	{
		return GSpriteMesh.GetIndexBufferRHI();
	}

	/**
	 * @brief Set texture rect
	 * @param InTextureRect Texture rect
	 */
	FORCEINLINE void SetTextureRect( const RectFloat_t& InTextureRect )
	{
		textureRect = InTextureRect;
		if ( vertexFactory )
		{
			vertexFactory->SetTextureRect( InTextureRect );
		}
	}

	/**
	 * @brief Get texture rect
	 * @return Return texture rect
	 */
	FORCEINLINE const RectFloat_t& GetTextureRect() const
	{
		return textureRect;
	}

	/**
	 * @brief Set sprite size
	 * @param InSpriteSize Sprite size
	 */
	FORCEINLINE void SetSpriteSize( const Vector2D& InSpriteSize )
	{
		spriteSize = InSpriteSize;
		if ( vertexFactory )
		{
			vertexFactory->SetSpriteSize( InSpriteSize );
		}
	}

	/**
	 * @brief Set flip by vertical
	 * @param InFlipVertical Is need flip sprite by vertical
	 */
	FORCEINLINE void SetFlipVertical( bool InFlipVertical )
	{
		bFlipVertical = InFlipVertical;
		if ( vertexFactory )
		{
			vertexFactory->SetFlipVertical( InFlipVertical );
		}
	}

	/**
	 * @brief Set flip by horizontal
	 * @param InFlipHorizontal Is need flip sprite by horizontal
	 */
	FORCEINLINE void SetFlipHorizontal( bool InFlipHorizontal )
	{
		bFlipHorizontal = InFlipHorizontal;
		if ( vertexFactory )
		{
			vertexFactory->SetFlipHorizontal( InFlipHorizontal );
		}
	}

	/**
	 * @brief Get sprite size
	 * @return Return sprite size
	 */
	FORCEINLINE const Vector2D& GetSpriteSize() const
	{
		return spriteSize;
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
	bool									bFlipVertical;		/**< Is need flip sprite by vertical */
	bool									bFlipHorizontal;	/**< Is need flip sprite by horizontal */
	RectFloat_t								textureRect;		/**< Texture rect */
	Vector2D								spriteSize;			/**< Sprite size */
	TRefCountPtr<CSpriteVertexFactory>		vertexFactory;		/**< Vertex factory */
	TAssetHandle<CMaterial>					material;			/**< Material */
};

#endif // !SPRITE_H
