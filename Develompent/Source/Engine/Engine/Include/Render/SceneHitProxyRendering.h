/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef SCENEHITPROXYRENDERING_H
#define SCENEHITPROXYRENDERING_H

#include "LEBuild.h"

#if ENABLE_HITPROXY
#include "DrawingPolicy.h"
#include "Render/Shaders/HitProxyShader.h"

/**
 * @ingroup Engine
 * Draw policy of hit proxy
 */
class CHitProxyDrawingPolicy : public CMeshDrawingPolicy
{
public:
	/**
	 * Initialize mesh drawing policy
	 *
	 * @param InVertexFactory	Vertex factory
	 * @param InMaterial		Material
	 * @param InDepthBias		Depth bias
	 */
	FORCEINLINE void Init( class CVertexFactory* InVertexFactory, const TAssetHandle<CMaterial>& InMaterial, float InDepthBias = 0.f )
	{
		InitInternal( InVertexFactory, InMaterial, InDepthBias );

		// Override shaders for hit proxy id rendering
		uint64			vertexFactoryHash = InVertexFactory->GetType()->GetHash();
		vertexShader	= g_ShaderManager->FindInstance<CHitProxyVertexShader>( vertexFactoryHash );
		pixelShader		= g_ShaderManager->FindInstance<CHitProxyPixelShader>( vertexFactoryHash );
	}
};
#endif // ENABLE_HITPROXY

#endif // !SCENEHITPROXYRENDERING_H