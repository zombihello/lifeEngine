/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef DEPTHSONLYHADER_H
#define DEPTHSONLYHADER_H

#include "Shader.h"
#include "ShaderManager.h"

/**
 * @ingroup Engine
 * @brief Class of depth only vertex shader
 */
class CDepthOnlyVertexShader : public CShader
{
	DECLARE_SHADER_TYPE( CDepthOnlyVertexShader )

public:
	/**
	 * @brief Construct a new CDepthVertexShader object
	 */
	CDepthOnlyVertexShader();

	/**
	 * @brief Destructor of a CDepthVertexShader object
	 */
	virtual ~CDepthOnlyVertexShader();

	/**
	 * @brief Initialize shader
	 * @param[in] InShaderCacheItem Cache of shader
	 */
	virtual void Init( const CShaderCache::SShaderCacheItem& InShaderCacheItem ) override;

	/**
	 * @brief Set the constant shader parameters
	 *
	 * @param InDeviceContextRHI Device context
	 * @param InVertexFactory Vertex factory
	 * @param InMaterialResource Material
	 */
	virtual void SetConstantParameters( class CBaseDeviceContextRHI* InDeviceContextRHI, const class CVertexFactory* InVertexFactory, const TSharedPtr<class CMaterial>& InMaterialResource ) const override;

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
	class CVertexFactoryShaderParameters*	vertexFactoryParameters;	/**< Vertex factory shader parameters */
};

/**
 * @ingroup Engine
 * @brief Class of depth only pixel shader
 */
class CDepthOnlyPixelShader : public CShader
{
	DECLARE_SHADER_TYPE( CDepthOnlyPixelShader )
};

#endif // !DEPTHSONLYHADER_H