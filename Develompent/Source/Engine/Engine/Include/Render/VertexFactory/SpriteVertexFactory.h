/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef SPRITEVERTEXFACTORY_H
#define SPRITEVERTEXFACTORY_H

#include "Math/Math.h"
#include "Math/Rect.h"
#include "Render/VertexFactory/VertexFactory.h"
#include "Render/VertexFactory/GeneralVertexFactoryParams.h"
#include "Render/RenderUtils.h"

 /**
  * @ingroup Engine
  * Vertex type for sprite
  */
struct SSpriteVertexType
{
	Vector4D		position;		/**< Position vertex */
	Vector2D		texCoord;		/**< Texture coords */
	Vector4D		normal;			/**< Normal */

	/**
	 * Overload operator ==
	 */
	bool FORCEINLINE operator==( const SSpriteVertexType& InOther ) const
	{
		return position == InOther.position &&
			texCoord == InOther.texCoord &&
			normal == InOther.normal;
	}
};

/**
 * @ingroup Engine
 * The sprite vertex declaration resource type
 */
class CSpriteVertexDeclaration : public CRenderResource
{
public:
	/**
	 * @brief Get vertex declaration RHI
	 * @return Return vertex declaration RHI
	 */
	FORCEINLINE VertexDeclarationRHIRef_t GetVertexDeclarationRHI()
	{
		if ( !vertexDeclarationRHI )
		{
			InitRHI();
		}
		return vertexDeclarationRHI;
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
	VertexDeclarationRHIRef_t		vertexDeclarationRHI;		/**< Vertex declaration RHI */
};

/**
 * @ingroup Engine
 * Global resource of sprite vertex declaration
 */
extern TGlobalResource< CSpriteVertexDeclaration >			g_SpriteVertexDeclaration;

/**
 * @ingroup Engine
 * @brief Vertex factory shader parameters for sprites
 */
class CSpriteVertexShaderParameters : public CGeneralVertexShaderParameters
{
public:
	/**
	 * Constructor
	 */
	CSpriteVertexShaderParameters();

	/**
	 * @brief Bind shader parameters
	 *
	 * @param InParameterMap Shader parameter map
	 */
	virtual void Bind( const class CShaderParameterMap& InParameterMap ) override;

	/**
	 * @brief Set any shader data specific to this vertex factory
	 *
	 * @param InDeviceContextRHI RHI device context
	 * @param InVertexFactory Vertex factory
	 */
	virtual void Set( class CBaseDeviceContextRHI* InDeviceContextRHI, const class CVertexFactory* InVertexFactory ) const override;

private:
	CShaderParameter		flipVerticalParameter;		/**< Flag is need flip by vertical parameter */
	CShaderParameter		flipHorizontalParameter;	/**< Flag is need flip by horizontal parameter */
	CShaderParameter		textureRectParameter;		/**< Texture rect parameter */
	CShaderParameter		spriteSizeParameter;		/**< Sprite size parameter */
};

/**
 * @ingroup Engine
 * Vertex factory for render sprites
 */
class CSpriteVertexFactory : public CVertexFactory
{
	DECLARE_VERTEX_FACTORY_TYPE( CSpriteVertexFactory )

public:
	enum EStreamSourceSlot
	{
		SSS_Main		= 0,	/**< Main vertex buffer */
		SSS_Instance	= 1		/**< Instance buffer */
	};

	/**
	 * @brief Constructor
	 */
	CSpriteVertexFactory();

	/**
	 * @brief Initializes the RHI resources used by this resource.
	 * Called when the resource is initialized.
	 * This is only called by the rendering thread.
	 */
	virtual void InitRHI() override;

	/**
	 * @brief Setup instancing
	 *
	 * @param InDeviceContextRHI RHI device context
	 * @param InMesh Mesh data
	 * @param InView Scene view
	 * @param InNumInstances Number instances
	 * @param InStartInstanceID ID of first instance
	 */
	virtual void SetupInstancing( class CBaseDeviceContextRHI* InDeviceContextRHI, const struct SMeshBatch& InMesh, const class CSceneView* InView, uint32 InNumInstances = 1, uint32 InStartInstanceID = 0 ) const override;

	/**
	 * @brief Get type hash
	 * @return Return hash of vertex factory
	 */
	virtual uint64 GetTypeHash() const override;

	/**
	 * @brief Construct vertex factory shader parameters
	 * 
	 * @param InShaderFrequency Shader frequency
	 * @return Return instance of vertex factory shader parameters
	 */
	static CVertexFactoryShaderParameters* ConstructShaderParameters( EShaderFrequency InShaderFrequency );

	/**
	 * @brief Set texture rect
	 * @param InTextureRect Texture rect
	 */
	FORCEINLINE void SetTextureRect( const RectFloat_t& InTextureRect )
	{
		textureRect = InTextureRect;
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
	}

	/**
	 * @brief Set flip by vertical
	 * @param InFlipVertical Is need flip sprite by vertical
	 */
	FORCEINLINE void SetFlipVertical( bool InFlipVertical )
	{
		bFlipVertical = InFlipVertical;
	}

	/**
	 * @brief Set flip by horizontal
	 * @param InFlipHorizontal Is need flip sprite by horizontal
	 */
	FORCEINLINE void SetFlipHorizontal( bool InFlipHorizontal )
	{
		bFlipHorizontal = InFlipHorizontal;
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
	bool				bFlipVertical;		/**< Is need flip sprite by vertical */
	bool				bFlipHorizontal;	/**< Is need flip sprite by horizontal */
	RectFloat_t			textureRect;		/**< Texture rect */
	Vector2D			spriteSize;			/**< Sprite size */
};

//
// Serialization
//

FORCEINLINE CArchive& operator<<( CArchive& InArchive, SSpriteVertexType& InValue )
{
	InArchive << InValue.position;
	InArchive << InValue.texCoord;
	InArchive << InValue.normal;
	return InArchive;
}

FORCEINLINE CArchive& operator<<( CArchive& InArchive, const SSpriteVertexType& InValue )
{
	InArchive << InValue.position;
	InArchive << InValue.texCoord;
	InArchive << InValue.normal;
	return InArchive;
}

#endif // !SPRITEVERTEXFACTORY_H
