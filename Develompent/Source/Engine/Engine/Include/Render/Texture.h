/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef TEXTURE_H
#define TEXTURE_H

#include "RenderResource.h"

/**
 * @ingroup Engine
 * @brief Implementation for texture
 */
class FTexture : public FRenderResource
{
public:
	/**
	 * @brief Destructor
	 */
	virtual ~FTexture() {}
};

/**
 * @ingroup Engine
 * @brief Implementation for 2D textures
 */
class FTexture2D : public FTexture
{
public:
	/**
	 * @brief Destructor
	 */
	virtual ~FTexture2D() {}
};

#endif // !TEXTURE_H