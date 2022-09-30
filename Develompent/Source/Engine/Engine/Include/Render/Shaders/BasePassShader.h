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
class CBasePassVertexShader : public CShader
{
	DECLARE_SHADER_TYPE( CBasePassVertexShader )

public:
	/**
	 * @brief Construct a new CBasePassVertexShader object
	 */
	CBasePassVertexShader();

    /**
     * @brief Destructor of a CBasePassVertexShader object
     */
    virtual ~CBasePassVertexShader();

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

private:
	class CVertexFactoryShaderParameters*		vertexFactoryParameters;		/**< Vertex factory shader parameters */
};

/**
 * @ingroup Engine
 * @brief Class of base pass pixel shader
 */
class CBasePassPixelShader : public CShader
{
	DECLARE_SHADER_TYPE( CBasePassPixelShader )

public:
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

private:
	CShaderResourceParameter		diffuseParameter;				/**< Diffuse texture parameter */
	CShaderResourceParameter		diffuseSamplerParameter;		/**< Diffuse sampler parameter */
	CShaderResourceParameter		normalParameter;				/**< Normal texture parameter */
	CShaderResourceParameter		normalSamplerParameter;			/**< Normal sampler parameter */
	CShaderResourceParameter		metallicParameter;				/**< Metallic texture parameter */
	CShaderResourceParameter		metallicSamplerParameter;		/**< Metallic sampler parameter */
	CShaderResourceParameter		roughnessParameter;				/**< Roughness texture parameter */
	CShaderResourceParameter		roughnessSamplerParameter;		/**< Roughness sampler parameter */
};

#endif // !BASEPASSSHADER_H

