/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef BASEPASSSHADER_H
#define BASEPASSSHADER_H

#include "Shader.h"
#include "ShaderManager.h"

/**
 * @ingroup Engine
 * @brief Class of base pass vertex shader
 */
class FBasePassVertexShader : public FShader
{
	DECLARE_SHADER_TYPE( FBasePassVertexShader )

public:
	/**
	 * @brief Construct a new FBasePassVertexShader object
	 */
	FBasePassVertexShader();

    /**
     * @brief Destructor of a FBasePassVertexShader object
     */
    virtual ~FBasePassVertexShader();

	/**
	 * @brief Initialize shader
	 * @param[in] InShaderCacheItem Cache of shader
	 */
	virtual void Init( const FShaderCache::FShaderCacheItem& InShaderCacheItem ) override;

	/**
	 * @brief Set the constant shader parameters
	 *
	 * @param InDeviceContextRHI Device context
	 * @param InVertexFactory Vertex factory
	 * @param InMaterialResource Material
	 */
	virtual void SetConstantParameters( class FBaseDeviceContextRHI* InDeviceContextRHI, const class FVertexFactory* InVertexFactory, const class FMaterial* InMaterialResource ) const;

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
	virtual void SetMesh( class FBaseDeviceContextRHI* InDeviceContextRHI, const struct FMeshBatch& InMesh, const class FVertexFactory* InVertexFactory, const class FSceneView* InView, uint32 InNumInstances = 1, uint32 InStartInstanceID = 0 ) const override;

private:
	class FVertexFactoryShaderParameters*		vertexFactoryParameters;		/**< Vertex factory shader parameters */
};

/**
 * @ingroup Engine
 * @brief Class of base pass pixel shader
 */
class FBasePassPixelShader : public FShader
{
	DECLARE_SHADER_TYPE( FBasePassPixelShader )

public:
};

#endif // !BASEPASSSHADER_H

