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
 * @brief Reference to FSprite
 */
typedef TRefCountPtr< class FSprite >				FSpriteRef;

/**
 * @ingroup Engine
 * Surface in sprite mesh
 */
struct FSpriteSurface
{
	uint32		baseVertexIndex;		/**< First index vertex in vertex buffer */
	uint32		firstIndex;				/**< First index */
	uint32		numPrimitives;			/**< Number primitives in the surface */
};

/**
 * @ingroup Engine
 * @brief Sprite mesh data for rendering sprites
 */
class FSpriteMesh : public FRenderResource
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
	FORCEINLINE FSpriteSurface GetSurface() const
	{
		return FSpriteSurface{ 0, 0, 2 };
	}

	/**
	 * Get RHI vertex buffer
	 * @return Return RHI vertex buffer, if not created return nullptr
	 */
	FORCEINLINE FVertexBufferRHIRef GetVertexBufferRHI() const
	{
		return vertexBufferRHI;
	}

	/**
	 * Get RHI index buffer
	 * @return Return RHI index buffer, if not created return nullptr
	 */
	FORCEINLINE FIndexBufferRHIRef GetIndexBufferRHI() const
	{
		return indexBufferRHI;
	}

private:
	FVertexBufferRHIRef			vertexBufferRHI;		/**< Vertex buffer RHI */
	FIndexBufferRHIRef			indexBufferRHI;			/**< Index buffer RHI */
};

extern TGlobalResource< FSpriteMesh >		GSpriteMesh;			/**< The global sprite mesh data for rendering sprites */

/**
 * @ingroup Engine
 * @brief Implementation for sprite mesh
 */
class FSprite : public FRenderResource, public FRefCounted
{
public:
	/**
	 * @brief Constructor
	 */
	FSprite();

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
	FORCEINLINE void SetMaterial( const TWeakPtr<FMaterial>& InMaterial )
	{
		material = InMaterial;
	}

	/**
	 * Get surface
	 * @return Return surface info
	 */
	FORCEINLINE FSpriteSurface GetSurface() const
	{
		return GSpriteMesh.GetSurface();
	}

	/**
	 * @brief Get material
	 * @return Return pointer to material. If not setted returning nullptr
	 */
	FORCEINLINE TWeakPtr<FMaterial> GetMaterial() const
	{
		return material;
	}

	/**
	 * Get vertex factory
	 * @return Return vertex factory
	 */
	FORCEINLINE TRefCountPtr< FSpriteVertexFactory > GetVertexFactory() const
	{
		return vertexFactory;
	}

	/**
	 * Get RHI vertex buffer
	 * @return Return RHI vertex buffer, if not created return nullptr
	 */
	FORCEINLINE FVertexBufferRHIRef GetVertexBufferRHI() const
	{
		return GSpriteMesh.GetVertexBufferRHI();
	}

	/**
	 * Get RHI index buffer
	 * @return Return RHI index buffer, if not created return nullptr
	 */
	FORCEINLINE FIndexBufferRHIRef GetIndexBufferRHI() const
	{
		return GSpriteMesh.GetIndexBufferRHI();
	}

	/**
	 * @brief Set texture rect
	 * @param InTextureRect Texture rect
	 */
	FORCEINLINE void SetTextureRect( const FRectFloat& InTextureRect )
	{
		check( vertexFactory );
		vertexFactory->SetTextureRect( InTextureRect );
	}

	/**
	 * @brief Get texture rect
	 * @return Return texture rect
	 */
	FORCEINLINE const FRectFloat& GetTextureRect() const
	{
		check( vertexFactory );
		return vertexFactory->GetTextureRect();
	}

	/**
	 * @brief Set sprite size
	 * @param InSpriteSize Sprite size
	 */
	FORCEINLINE void SetSpriteSize( const FVector2D& InSpriteSize )
	{
		check( vertexFactory );
		vertexFactory->SetSpriteSize( InSpriteSize );
	}

	/**
	 * @brief Set flip by vertical
	 * @param InFlipVertical Is need flip sprite by vertical
	 */
	FORCEINLINE void SetFlipVertical( bool InFlipVertical )
	{
		check( vertexFactory );
		vertexFactory->SetFlipVertical( InFlipVertical );
	}

	/**
	 * @brief Set flip by horizontal
	 * @param InFlipHorizontal Is need flip sprite by horizontal
	 */
	FORCEINLINE void SetFlipHorizontal( bool InFlipHorizontal )
	{
		check( vertexFactory );
		vertexFactory->SetFlipHorizontal( InFlipHorizontal );
	}

	/**
	 * @brief Get sprite size
	 * @return Return sprite size
	 */
	FORCEINLINE const FVector2D& GetSpriteSize() const
	{
		check( vertexFactory );
		return vertexFactory->GetSpriteSize();
	}

	/**
	 * @brief Is fliped by vertical
	 * @return Return TRUE if sprite fliped by vertical
	 */
	FORCEINLINE bool IsFlipedVertical() const
	{
		check( vertexFactory );
		return vertexFactory->IsFlipedVertical();
	}

	/**
	 * @brief Is fliped by horizontal
	 * @return Return TRUE if sprite fliped by horizontal
	 */
	FORCEINLINE bool IsFlipedHorizontal() const
	{
		check( vertexFactory );
		return vertexFactory->IsFlipedHorizontal();
	}

private:
	TRefCountPtr< FSpriteVertexFactory >	vertexFactory;			/**< Vertex factory */
	TWeakPtr<FMaterial>						material;				/**< Material */
};

#endif // !SPRITE_H
