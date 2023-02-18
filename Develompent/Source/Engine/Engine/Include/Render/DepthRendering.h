/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef DEPTHRENDERING_H
#define DEPTHRENDERING_H

#include "DrawingPolicy.h"
#include "Render/Shaders/DepthOnlyShader.h"

 /**
  * @ingroup Engine
  * Draw policy of only depth
  */
class CDepthDrawingPolicy : public CMeshDrawingPolicy
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

		// Override shaders for depth only rendering
		uint64			vertexFactoryHash = InVertexFactory->GetType()->GetHash();
		vertexShader	= GShaderManager->FindInstance<CDepthOnlyVertexShader>( vertexFactoryHash );
		pixelShader		= GShaderManager->FindInstance<CDepthOnlyPixelShader>( vertexFactoryHash );
	}
};

#endif // !DEPTHRENDERING_H