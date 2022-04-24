/**
 * @file
 * @addtogroup WorldEd WorldEd
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef WORLDGRIDDRAWINGPOLICY_H
#define WORLDGRIDDRAWINGPOLICY_H

#include "Render/DrawingPolicy.h"

/**
 * @ingroup Engine
 * Draw policy of static mesh
 */
class FWorldGridDrawingPolicy : public FMeshDrawingPolicy
{
protected:
	/**
	 * Initialize mesh drawing policy (internal implementation for override by child classes)
	 *
	 * @param InVertexFactory	Vertex factory
	 * @param InMaterial		Material
	 * @param InDepthBias		Depth bias
	 */
	virtual void InitInternal( class FVertexFactory* InVertexFactory, class FMaterial* InMaterial, float InDepthBias = 0.f ) override;
};

#endif // !WORLDGRIDDRAWINGPOLICY_H