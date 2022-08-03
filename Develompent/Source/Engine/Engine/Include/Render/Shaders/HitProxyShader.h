/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef HITPROXYSHADER_H
#define HITPROXYSHADER_H

#include "LEBuild.h"

#if ENABLE_HITPROXY
#include "Shader.h"
#include "ShaderManager.h"

/**
 * @ingroup Engine
 * @brief Class of hit proxy vertex shader
 */
class CHitProxyVertexShader : public CShader
{
	DECLARE_SHADER_TYPE( CHitProxyVertexShader )

public:
	/**
	 * @brief Construct a new CHitProxyVertexShader object
	 */
	CHitProxyVertexShader();

	/**
	 * @brief Destructor of a CHitProxyVertexShader object
	 */
	virtual ~CHitProxyVertexShader();

#if WITH_EDITOR
	/**
	 * @brief Is need compile shader for platform
	 *
	 * @param InShaderPlatform Shader platform
	 * @param InVFMetaType Vertex factory meta type. If him is nullptr - return general check
	 * @return Return true if need compile shader, else returning false
	 */
	static bool ShouldCache( EShaderPlatform InShaderPlatform, class CVertexFactoryMetaType* InVFMetaType = nullptr );
#endif // WITH_EDITOR

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
	CShaderParameter						hitProxyIdParameter;		/**< Hit proxy id parameter */
};

/**
 * @ingroup Engine
 * @brief Class of hit proxy pixel shader
 */
class CHitProxyPixelShader : public CShader
{
	DECLARE_SHADER_TYPE( CHitProxyPixelShader )

public:
#if WITH_EDITOR
	/**
	 * @brief Is need compile shader for platform
	 *
	 * @param InShaderPlatform Shader platform
	 * @param InVFMetaType Vertex factory meta type. If him is nullptr - return general check
	 * @return Return true if need compile shader, else returning false
	 */
	static bool ShouldCache( EShaderPlatform InShaderPlatform, class CVertexFactoryMetaType* InVFMetaType = nullptr );
#endif // WITH_EDITOR
};
#endif // ENABLE_HITPROXY

#endif // !HITPROXYSHADER_H