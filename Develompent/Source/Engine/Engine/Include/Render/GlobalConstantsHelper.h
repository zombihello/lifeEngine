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
 * @param[out] OutGlobalContents	Output global constant buffer
 * @param[in] InSceneView			Scene view info
 * @param[in] InScreenAndTexelSize	Screen and buffer size
 */
void SetGlobalConstants( SGlobalConstantBufferContents& OutGlobalContents, const CSceneView& InSceneView, const Vector4D& InScreenAndBufferSize );

#endif // !GLOBALCONSTANTSHELPER_H