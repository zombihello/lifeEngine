/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef GENERALVERTEXFACTORYPARAMS_H
#define GENERALVERTEXFACTORYPARAMS_H

#include "Render/VertexFactory/VertexFactory.h"
#include "Render/Shaders/ShaderManager.h"

/**
 * @ingroup Engine
 * @brief Vertex factory shader parameters for each meshes
 */
class FGeneralVertexShaderParameters : public FVertexFactoryShaderParameters
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

    /**
     * @brief Set the l2w transform shader
     *
     * @param InDeviceContextRHI RHI device context
     * @param InMesh Mesh data
     * @param InBatchElementIndex Batch element index
     * @param InView Scene view
     */
    virtual void SetMesh( class FBaseDeviceContextRHI* InDeviceContextRHI, const struct FMeshBatch& InMesh, uint32 InBatchElementIndex, const class FSceneView* InView ) const override;

private:
    FShaderParameter		localToWorldMatrixParameter;		/**< Local to world parameter */
};

#endif // !GENERALVERTEXFACTORYPARAMS_H
