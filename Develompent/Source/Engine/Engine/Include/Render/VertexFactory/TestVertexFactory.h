/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef TESTVERTEXFACTORY_H
#define TESTVERTEXFACTORY_H

#include "Math/Math.h"
#include "Render/VertexFactory/VertexFactory.h"

/**
 * @ingroup Engine
 * Vertex type for test triangle
 */
struct FTestVertexType
{
	FVector			position;		/**< Position vertex */
	FVector2D		texCoord;		/**< Texture coords */
};

/**
 * @ingroup Engine
 * Test vertex factory for render test triangle
 */
class FTestVertexFactory : public FVertexFactory
{
	DECLARE_VERTEX_FACTORY_TYPE( FTestVertexFactory )

public:
	enum EStreamSourceSlot
	{
		SSS_Main			= 0		/**< Main vertex buffer */
	};

	/**
	 * @brief Initializes the RHI resources used by this resource.
	 * Called when the resource is initialized.
	 * This is only called by the rendering thread.
	 */
	virtual void InitRHI() override;
};

#endif // !TESTVERTEXFACTORY_H