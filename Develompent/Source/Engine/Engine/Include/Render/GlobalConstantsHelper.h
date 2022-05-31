/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef GLOBALCONSTANTSHELPER_H
#define GLOBALCONSTANTSHELPER_H

#include "Render/Scene.h"
#include "CPP_GlobalConstantBuffers.hlsl"

/**
 * @ingroup Engine
 * Fill global constants buffer
 * 
 * @param[out] OutGlobalContents Output global constant buffer
 * @param[in] InSceneView Scene view info
 */
void SetGlobalConstants( SGlobalConstantBufferContents& OutGlobalContents, const CSceneView& InSceneView );

#endif // !GLOBALCONSTANTSHELPER_H