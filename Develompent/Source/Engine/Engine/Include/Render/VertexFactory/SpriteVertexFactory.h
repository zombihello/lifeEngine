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

 /**
  * @ingroup Engine
  * Vertex type for sprite
  */
struct FSpriteVertexType
{
	FVector4D		position;		/**< Position vertex */
	FVector2D		texCoord;		/**< Texture coords */
	FVector4D		normal;			/**< Normal */

	/**
	 * Overload operator ==
	 */
	bool FORCEINLINE operator==( const FSpriteVertexType& InOther ) const
	{
		return position == InOther.position &&
			texCoord == InOther.texCoord &&
			normal == InOther.normal;
	}
};

/**
 * @ingroup Engine
 * @brief Vertex factory shader parameters for sprites
 */
class FSpriteVertexShaderParameters : public FGeneralVertexShaderParameters
{
public:
	/**
	 * @brief Bind shader parameters
	 *
	 * @param InParameterMap Shader parameter map
	 */
	virtual void Bind( const class FShaderParameterMap& InParameterMap ) override;

	/**
	 * @brief Set any shader data specific to this vertex factory
	 *
	 * @param InDeviceContextRHI RHI device context
	 * @param InVertexFactory Vertex factory
	 * @param InView Scene view
	 */
	virtual void Set( class FBaseDeviceContextRHI* InDeviceContextRHI, const class FVertexFactory* InVertexFactory, const class FSceneView* InView ) const override;

private:
	FShaderParameter		textureRectParameter;		/**< Texture rect parameter */
	FShaderParameter		spriteSizeParameter;		/**< Sprite size parameter */
};

/**
 * @ingroup Engine
 * Vertex factory for render sprites
 */
class FSpriteVertexFactory : public FVertexFactory
{
	DECLARE_VERTEX_FACTORY_TYPE( FSpriteVertexFactory )

public:
	enum EStreamSourceSlot
	{
		SSS_Main = 0		/**< Main vertex buffer */
	};

	/**
	 * @brief Constructor
	 */
	FSpriteVertexFactory();

	/**
	 * @brief Initializes the RHI resources used by this resource.
	 * Called when the resource is initialized.
	 * This is only called by the rendering thread.
	 */
	virtual void InitRHI() override;

	/**
	 * @brief Construct vertex factory shader parameters
	 * 
	 * @param InShaderFrequency Shader frequency
	 * @return Return instance of vertex factory shader parameters
	 */
	static FVertexFactoryShaderParameters* ConstructShaderParameters( EShaderFrequency InShaderFrequency );

	/**
	 * @brief Set texture rect
	 * @param InTextureRect Texture rect
	 */
	FORCEINLINE void SetTextureRect( const FRectFloat& InTextureRect )
	{
		textureRect = InTextureRect;
	}

	/**
	 * @brief Get texture rect
	 * @return Return texture rect
	 */
	FORCEINLINE const FRectFloat& GetTextureRect() const
	{
		return textureRect;
	}

	/**
	 * @brief Set sprite size
	 * @param InSpriteSize Sprite size
	 */
	FORCEINLINE void SetSpriteSize( const FVector2D& InSpriteSize )
	{
		spriteSize = InSpriteSize;
	}

	/**
	 * @brief Get sprite size
	 * @return Return sprite size
	 */
	FORCEINLINE const FVector2D& GetSpriteSize() const
	{
		return spriteSize;
	}

private:
	FRectFloat			textureRect;		/**< Texture rect */
	FVector2D			spriteSize;			/**< Sprite size */
};

//
// Serialization
//

FORCEINLINE FArchive& operator<<( FArchive& InArchive, FSpriteVertexType& InValue )
{
	InArchive << InValue.position;
	InArchive << InValue.texCoord;
	InArchive << InValue.normal;
	return InArchive;
}

FORCEINLINE FArchive& operator<<( FArchive& InArchive, const FSpriteVertexType& InValue )
{
	InArchive << InValue.position;
	InArchive << InValue.texCoord;
	InArchive << InValue.normal;
	return InArchive;
}

#endif // !SPRITEVERTEXFACTORY_H
