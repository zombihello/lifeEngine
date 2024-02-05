/**
 * @file
 * @addtogroup Engine Engine
 *
 * ************************************************************
 *                  This file is part of:
 *                      LIFEENGINE
 *          https://github.com/zombihello/lifeEngine
 * ************************************************************
 * Copyright (C) 2024 Yehor Pohuliaka.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef SPRITE_H
#define SPRITE_H

#include "Engine/Render/RenderResource.h"
#include "Engine/Render/RenderUtils.h"
#include "Core/Misc/RefCounted.h"
#include "Engine/Render/VertexFactory/SpriteVertexFactory.h"
#include "RHI/BaseBufferRHI.h"
#include "RHI/TypesRHI.h"
#include "Engine/Render/Material.h"

/**
 * @ingroup Engine
 * @brief Reference to CSprite
 */
typedef TRefCountPtr< class CSprite >				SpriteRef_t;

/**
 * @ingroup Engine
 * Surface in sprite mesh
 */
struct SpriteSurface
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
	 * Get surface
	 * @return Return surface info
	 */
	FORCEINLINE SpriteSurface GetSurface() const
	{
		return SpriteSurface{ 0, 0, 2 };
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

protected:
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

private:
	VertexBufferRHIRef_t		vertexBufferRHI;		/**< Vertex buffer RHI */
	IndexBufferRHIRef_t			indexBufferRHI;			/**< Index buffer RHI */
};

extern TGlobalResource< CSpriteMesh >		g_SpriteMesh;			/**< The global sprite mesh data for rendering sprites */

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
	FORCEINLINE SpriteSurface GetSurface() const
	{
		return g_SpriteMesh.GetSurface();
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
		return g_SpriteMesh.GetVertexBufferRHI();
	}

	/**
	 * Get RHI index buffer
	 * @return Return RHI index buffer, if not created return nullptr
	 */
	FORCEINLINE IndexBufferRHIRef_t GetIndexBufferRHI() const
	{
		return g_SpriteMesh.GetIndexBufferRHI();
	}

	/**
	 * @brief Set texture rect
	 * @param InTextureRect Texture rect
	 */
	FORCEINLINE void SetTextureRect( const RectFloat_t& InTextureRect )
	{
		vertexFactory->SetTextureRect( InTextureRect );
	}

	/**
	 * @brief Get texture rect
	 * @return Return texture rect
	 */
	FORCEINLINE const RectFloat_t& GetTextureRect() const
	{
		return vertexFactory->GetTextureRect();
	}

	/**
	 * @brief Set sprite size
	 * @param InSpriteSize Sprite size
	 */
	FORCEINLINE void SetSpriteSize( const Vector2D& InSpriteSize )
	{
		vertexFactory->SetSpriteSize( InSpriteSize );
	}

	/**
	 * @brief Set flip by vertical
	 * @param InFlipVertical Is need flip sprite by vertical
	 */
	FORCEINLINE void SetFlipVertical( bool InFlipVertical )
	{
		vertexFactory->SetFlipVertical( InFlipVertical );
	}

	/**
	 * @brief Set flip by horizontal
	 * @param InFlipHorizontal Is need flip sprite by horizontal
	 */
	FORCEINLINE void SetFlipHorizontal( bool InFlipHorizontal )
	{
		vertexFactory->SetFlipHorizontal( InFlipHorizontal );
	}

	/**
	 * @brief Get sprite size
	 * @return Return sprite size
	 */
	FORCEINLINE const Vector2D& GetSpriteSize() const
	{
		return vertexFactory->GetSpriteSize();
	}

	/**
	 * @brief Is fliped by vertical
	 * @return Return TRUE if sprite fliped by vertical
	 */
	FORCEINLINE bool IsFlipedVertical() const
	{
		return vertexFactory->IsFlipedVertical();
	}

	/**
	 * @brief Is fliped by horizontal
	 * @return Return TRUE if sprite fliped by horizontal
	 */
	FORCEINLINE bool IsFlipedHorizontal() const
	{
		return vertexFactory->IsFlipedHorizontal();
	}

protected:
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

private:
	TRefCountPtr<CSpriteVertexFactory>		vertexFactory;		/**< Vertex factory */
	TAssetHandle<CMaterial>					material;			/**< Material */
};

#endif // !SPRITE_H
