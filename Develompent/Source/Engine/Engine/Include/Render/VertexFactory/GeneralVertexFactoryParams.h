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
class CGeneralVertexShaderParameters : public CVertexFactoryShaderParameters
{
public:
    /**
     * Constructor
     * 
     * @param InSupportsInstancing Is supported instancing
     */
    CGeneralVertexShaderParameters( bool InSupportsInstancing );

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

	/**
	 * @brief Set the l2w transform shader
	 *
	 * @param InDeviceContextRHI RHI device context
	 * @param InMesh Mesh data
	 * @param InVertexFactory Vertex factory
	 * @param InView Scene view
	 * @param InNumInstances Number instances
	 * @param InStartInstanceID ID of first instance
	 */
	virtual void SetMesh( class CBaseDeviceContextRHI* InDeviceContextRHI, const struct SMeshBatch& InMesh, const class CVertexFactory* InVertexFactory, const class CSceneView* InView, uint32 InNumInstances = 1, uint32 InStartInstanceID = 0 ) const override;

private:
    CShaderParameter		localToWorldMatrixParameter;	/**< Local to world parameter */
    bool					bSupportsInstancing;			/**< Is supported instancing */

#if WITH_EDITOR
	CShaderParameter		colorOverlayParameter;			/**< Color overlay parameter for selection actors */
#endif // WITH_EDITOR
};

#endif // !GENERALVERTEXFACTORYPARAMS_H
