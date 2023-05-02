/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef LIGHTINGSHADER_H
#define LIGHTINGSHADER_H

#include "Shader.h"
#include "ShaderManager.h"
#include "Render/VertexFactory/LightVertexFactory.h"
#include "Render/Shaders/DepthOnlyShader.h"
#include "Components/LightComponent.h"

/**
 * @ingroup Engine
 * @brief Class of utils for lighting shaders
 */
class CLightingShadersUtils
{
public:
#if WITH_EDITOR
	/**
	 * @brief Modify compilation environment
	 * This method added to the compiler environment base definitions of lighting system
	 *
	 * @param InLightType	Light type
	 * @param InEnvironment Shader compiler environment
	 */
	static FORCEINLINE void ModifyCompilationEnvironment( ELightType InLightType, SShaderCompilerEnvironment& InEnvironment )
	{
		tchar*	pointLightValue			= TEXT( "0" );
		tchar*	spotLightValue			= TEXT( "0" );
		tchar*	directionalLightValue	= TEXT( "0" );

		switch ( InLightType )
		{
		case LT_Point:
			pointLightValue = TEXT( "1" );
			break;

		case LT_Spot:
			spotLightValue = TEXT( "1" );
			break;

		case LT_Directional:
			directionalLightValue = TEXT( "1" );
			break;

		default:
			appErrorf( TEXT( "Unknown light type 0x%X" ), InLightType );
			return;
		}

		InEnvironment.difinitions.insert( std::make_pair( TEXT( "POINT_LIGHT" ),		pointLightValue ) );
		InEnvironment.difinitions.insert( std::make_pair( TEXT( "SPOT_LIGHT" ),			spotLightValue ) );
		InEnvironment.difinitions.insert( std::make_pair( TEXT( "DIRECTIONAL_LIGHT" ),	directionalLightValue ) );
	}

	/**
	 * @brief Is need compile shader for platform
	 *
	 * @param InShaderPlatform	Shader platform
	 * @param InVFMetaType		Vertex factory meta type. If him is nullptr - return general check
	 * @return Return true if need compile shader, else returning false
	 */
	static FORCEINLINE bool ShouldCache( EShaderPlatform InShaderPlatform, class CVertexFactoryMetaType* InVFMetaType = nullptr )
	{
		if ( !InVFMetaType )
		{
			return true;
		}

		// Shader supported only light vertex factory
		return InVFMetaType->GetHash() == CLightVertexFactory::staticType.GetHash();
	}
#endif // WITH_EDITOR
};

/**
 * @ingroup Engine
 * @brief Base class of lighting vertex shader
 */
class CBaseLightingVertexShader : public CShader
{
public:
	/**
	 * @brief Construct a new CBasePassVertexShader object
	 */
	CBaseLightingVertexShader();

	/**
	 * @brief Destructor of a CBasePassVertexShader object
	 */
	virtual ~CBaseLightingVertexShader();

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

protected:
	class CLightVertexShaderParameters*		vertexFactoryParameters;	/**< Vertex factory shader parameters */
};

/**
 * @ingroup Engine
 * @brief Empty template class of lighting vertex shader (don't use!)
 */
template<ELightType InType>
class TLightingVertexShader : public CBaseLightingVertexShader
{
	static_assert( InType != LT_Unknown, "This template class cannot be used" );
};

/**
 * @ingroup Engine
 * @brief Class of point lighting vertex shader
 */
template<>
class TLightingVertexShader<LT_Point> : public CBaseLightingVertexShader
{
	DECLARE_SHADER_TYPE( TLightingVertexShader<LT_Point> )

public:
	/**
	 * @brief Set the l2w transform shader
	 *
	 * @param InDeviceContextRHI	RHI device context
	 * @param InLights				List of point light componenets
	 * @param InMesh				Mesh data
	 * @param InVertexFactory		Vertex factory
	 * @param InView				Scene view
	 * @param InNumInstances		Number instances
	 * @param InStartInstanceID		ID of first instance
	 */
	void SetMesh( class CBaseDeviceContextRHI* InDeviceContextRHI, const std::list<TRefCountPtr<CPointLightComponent>>& InLights, const class CVertexFactory* InVertexFactory, const class CSceneView* InView, uint32 InNumInstances = 1, uint32 InStartInstanceID = 0 ) const
	{
		check( vertexFactoryParameters && InVertexFactory && InVertexFactory->GetType()->GetHash() == CLightVertexFactory::staticType.GetHash() );
		vertexFactoryParameters->SetMesh( InDeviceContextRHI, InLights, ( CLightVertexFactory* )InVertexFactory, InView, InNumInstances, InStartInstanceID );
	}

#if WITH_EDITOR
	/**
	 * @brief Modify compilation environment
	 *
	 * @param InShaderPlatform Shader platform
	 * @param InEnvironment Shader compiler environment
	 */
	static void ModifyCompilationEnvironment( EShaderPlatform InShaderPlatform, SShaderCompilerEnvironment& InEnvironment )
	{
		CLightingShadersUtils::ModifyCompilationEnvironment( LT_Point, InEnvironment );
	}

	/**
	 * @brief Is need compile shader for platform
	 *
	 * @param InShaderPlatform Shader platform
	 * @param InVFMetaType Vertex factory meta type. If him is nullptr - return general check
	 * @return Return true if need compile shader, else returning false
	 */
	static bool ShouldCache( EShaderPlatform InShaderPlatform, class CVertexFactoryMetaType* InVFMetaType = nullptr )
	{
		return CLightingShadersUtils::ShouldCache( InShaderPlatform, InVFMetaType );
	}
#endif // WITH_EDITOR
};

/**
 * @ingroup Engine
 * @brief Class of spot lighting vertex shader
 */
template<>
class TLightingVertexShader<LT_Spot> : public CBaseLightingVertexShader
{
	DECLARE_SHADER_TYPE( TLightingVertexShader<LT_Spot> )

public:
	/**
	 * @brief Set the l2w transform shader
	 *
	 * @param InDeviceContextRHI	RHI device context
	 * @param InLights				List of spot light componenets
	 * @param InMesh				Mesh data
	 * @param InVertexFactory		Vertex factory
	 * @param InView				Scene view
	 * @param InNumInstances		Number instances
	 * @param InStartInstanceID		ID of first instance
	 */
	void SetMesh( class CBaseDeviceContextRHI* InDeviceContextRHI, const std::list<TRefCountPtr<CSpotLightComponent>>& InLights, const class CVertexFactory* InVertexFactory, const class CSceneView* InView, uint32 InNumInstances = 1, uint32 InStartInstanceID = 0 ) const
	{
		check( vertexFactoryParameters && InVertexFactory && InVertexFactory->GetType()->GetHash() == CLightVertexFactory::staticType.GetHash() );
		vertexFactoryParameters->SetMesh( InDeviceContextRHI, InLights, ( CLightVertexFactory* )InVertexFactory, InView, InNumInstances, InStartInstanceID );
	}

#if WITH_EDITOR
	/**
	 * @brief Modify compilation environment
	 *
	 * @param InShaderPlatform Shader platform
	 * @param InEnvironment Shader compiler environment
	 */
	static void ModifyCompilationEnvironment( EShaderPlatform InShaderPlatform, SShaderCompilerEnvironment& InEnvironment )
	{
		CLightingShadersUtils::ModifyCompilationEnvironment( LT_Spot, InEnvironment );
	}

	/**
	 * @brief Is need compile shader for platform
	 *
	 * @param InShaderPlatform Shader platform
	 * @param InVFMetaType Vertex factory meta type. If him is nullptr - return general check
	 * @return Return true if need compile shader, else returning false
	 */
	static bool ShouldCache( EShaderPlatform InShaderPlatform, class CVertexFactoryMetaType* InVFMetaType = nullptr )
	{
		return CLightingShadersUtils::ShouldCache( InShaderPlatform, InVFMetaType );
	}
#endif // WITH_EDITOR
};

/**
 * @ingroup Engine
 * @brief Class of directional lighting vertex shader
 */
template<>
class TLightingVertexShader<LT_Directional> : public CBaseLightingVertexShader
{
	DECLARE_SHADER_TYPE( TLightingVertexShader<LT_Directional> )

public:
	/**
	 * @brief Set the l2w transform shader
	 *
	 * @param InDeviceContextRHI	RHI device context
	 * @param InLights				List of directional light componenets
	 * @param InMesh				Mesh data
	 * @param InVertexFactory		Vertex factory
	 * @param InView				Scene view
	 * @param InNumInstances		Number instances
	 * @param InStartInstanceID		ID of first instance
	 */
	void SetMesh( class CBaseDeviceContextRHI* InDeviceContextRHI, const std::list<TRefCountPtr<CDirectionalLightComponent>>& InLights, const class CVertexFactory* InVertexFactory, const class CSceneView* InView, uint32 InNumInstances = 1, uint32 InStartInstanceID = 0 ) const
	{
		check( vertexFactoryParameters && InVertexFactory && InVertexFactory->GetType()->GetHash() == CLightVertexFactory::staticType.GetHash() );
		vertexFactoryParameters->SetMesh( InDeviceContextRHI, InLights, ( CLightVertexFactory* )InVertexFactory, InView, InNumInstances, InStartInstanceID );
	}

#if WITH_EDITOR
	/**
	 * @brief Modify compilation environment
	 *
	 * @param InShaderPlatform Shader platform
	 * @param InEnvironment Shader compiler environment
	 */
	static void ModifyCompilationEnvironment( EShaderPlatform InShaderPlatform, SShaderCompilerEnvironment& InEnvironment )
	{
		CLightingShadersUtils::ModifyCompilationEnvironment( LT_Directional, InEnvironment );
	}

	/**
	 * @brief Is need compile shader for platform
	 *
	 * @param InShaderPlatform Shader platform
	 * @param InVFMetaType Vertex factory meta type. If him is nullptr - return general check
	 * @return Return true if need compile shader, else returning false
	 */
	static bool ShouldCache( EShaderPlatform InShaderPlatform, class CVertexFactoryMetaType* InVFMetaType = nullptr )
	{
		return CLightingShadersUtils::ShouldCache( InShaderPlatform, InVFMetaType );
	}
#endif // WITH_EDITOR
};

/**
 * @ingroup Engine
 * @brief Empty template class of depth only lighting vertex shader (don't use!)
 */
template<ELightType InType>
class TDepthOnlyLightingVertexShader : public CBaseLightingVertexShader
{
	static_assert( InType != LT_Unknown, "This template class cannot be used" );
};

/**
 * @ingroup Engine
 * @brief Class of depth only point lighting vertex shader
 */
template<>
class TDepthOnlyLightingVertexShader<LT_Point> : public CBaseLightingVertexShader
{
	DECLARE_SHADER_TYPE( TDepthOnlyLightingVertexShader<LT_Point> )

public:
	/**
	 * @brief Set the l2w transform shader
	 *
	 * @param InDeviceContextRHI	RHI device context
	 * @param InLights				List of point light componenets
	 * @param InMesh				Mesh data
	 * @param InVertexFactory		Vertex factory
	 * @param InView				Scene view
	 * @param InNumInstances		Number instances
	 * @param InStartInstanceID		ID of first instance
	 */
	void SetMesh( class CBaseDeviceContextRHI* InDeviceContextRHI, const std::list<TRefCountPtr<CPointLightComponent>>& InLights, const class CVertexFactory* InVertexFactory, const class CSceneView* InView, uint32 InNumInstances = 1, uint32 InStartInstanceID = 0 ) const
	{
		check( vertexFactoryParameters && InVertexFactory && InVertexFactory->GetType()->GetHash() == CLightVertexFactory::staticType.GetHash() );
		vertexFactoryParameters->SetMesh( InDeviceContextRHI, InLights, ( CLightVertexFactory* )InVertexFactory, InView, InNumInstances, InStartInstanceID );
	}

#if WITH_EDITOR
	/**
	 * @brief Modify compilation environment
	 *
	 * @param InShaderPlatform Shader platform
	 * @param InEnvironment Shader compiler environment
	 */
	static void ModifyCompilationEnvironment( EShaderPlatform InShaderPlatform, SShaderCompilerEnvironment& InEnvironment )
	{
		CLightingShadersUtils::ModifyCompilationEnvironment( LT_Point, InEnvironment );
		CDepthOnlyShadersUtils::ModifyCompilationEnvironment( true, InEnvironment );
	}

	/**
	 * @brief Is need compile shader for platform
	 *
	 * @param InShaderPlatform Shader platform
	 * @param InVFMetaType Vertex factory meta type. If him is nullptr - return general check
	 * @return Return true if need compile shader, else returning false
	 */
	static bool ShouldCache( EShaderPlatform InShaderPlatform, class CVertexFactoryMetaType* InVFMetaType = nullptr )
	{
		return CLightingShadersUtils::ShouldCache( InShaderPlatform, InVFMetaType );
	}
#endif // WITH_EDITOR
};

/**
 * @ingroup Engine
 * @brief Class of spot lighting vertex shader
 */
template<>
class TDepthOnlyLightingVertexShader<LT_Spot> : public CBaseLightingVertexShader
{
	DECLARE_SHADER_TYPE( TDepthOnlyLightingVertexShader<LT_Spot> )

public:
	/**
	 * @brief Set the l2w transform shader
	 *
	 * @param InDeviceContextRHI	RHI device context
	 * @param InLights				List of spot light componenets
	 * @param InMesh				Mesh data
	 * @param InVertexFactory		Vertex factory
	 * @param InView				Scene view
	 * @param InNumInstances		Number instances
	 * @param InStartInstanceID		ID of first instance
	 */
	void SetMesh( class CBaseDeviceContextRHI* InDeviceContextRHI, const std::list<TRefCountPtr<CSpotLightComponent>>& InLights, const class CVertexFactory* InVertexFactory, const class CSceneView* InView, uint32 InNumInstances = 1, uint32 InStartInstanceID = 0 ) const
	{
		check( vertexFactoryParameters && InVertexFactory && InVertexFactory->GetType()->GetHash() == CLightVertexFactory::staticType.GetHash() );
		vertexFactoryParameters->SetMesh( InDeviceContextRHI, InLights, ( CLightVertexFactory* )InVertexFactory, InView, InNumInstances, InStartInstanceID );
	}

#if WITH_EDITOR
	/**
	 * @brief Modify compilation environment
	 *
	 * @param InShaderPlatform Shader platform
	 * @param InEnvironment Shader compiler environment
	 */
	static void ModifyCompilationEnvironment( EShaderPlatform InShaderPlatform, SShaderCompilerEnvironment& InEnvironment )
	{
		CLightingShadersUtils::ModifyCompilationEnvironment( LT_Spot, InEnvironment );
		CDepthOnlyShadersUtils::ModifyCompilationEnvironment( true, InEnvironment );
	}

	/**
	 * @brief Is need compile shader for platform
	 *
	 * @param InShaderPlatform Shader platform
	 * @param InVFMetaType Vertex factory meta type. If him is nullptr - return general check
	 * @return Return true if need compile shader, else returning false
	 */
	static bool ShouldCache( EShaderPlatform InShaderPlatform, class CVertexFactoryMetaType* InVFMetaType = nullptr )
	{
		return CLightingShadersUtils::ShouldCache( InShaderPlatform, InVFMetaType );
	}
#endif // WITH_EDITOR
};

/**
 * @ingroup Engine
 * @brief Class of directional lighting vertex shader
 */
template<>
class TDepthOnlyLightingVertexShader<LT_Directional> : public CBaseLightingVertexShader
{
	DECLARE_SHADER_TYPE( TDepthOnlyLightingVertexShader<LT_Directional> )

public:
	/**
	 * @brief Set the l2w transform shader
	 *
	 * @param InDeviceContextRHI	RHI device context
	 * @param InLights				List of directional light componenets
	 * @param InMesh				Mesh data
	 * @param InVertexFactory		Vertex factory
	 * @param InView				Scene view
	 * @param InNumInstances		Number instances
	 * @param InStartInstanceID		ID of first instance
	 */
	void SetMesh( class CBaseDeviceContextRHI* InDeviceContextRHI, const std::list<TRefCountPtr<CDirectionalLightComponent>>& InLights, const class CVertexFactory* InVertexFactory, const class CSceneView* InView, uint32 InNumInstances = 1, uint32 InStartInstanceID = 0 ) const
	{
		check( vertexFactoryParameters && InVertexFactory && InVertexFactory->GetType()->GetHash() == CLightVertexFactory::staticType.GetHash() );
		vertexFactoryParameters->SetMesh( InDeviceContextRHI, InLights, ( CLightVertexFactory* )InVertexFactory, InView, InNumInstances, InStartInstanceID );
	}

#if WITH_EDITOR
	/**
	 * @brief Modify compilation environment
	 *
	 * @param InShaderPlatform Shader platform
	 * @param InEnvironment Shader compiler environment
	 */
	static void ModifyCompilationEnvironment( EShaderPlatform InShaderPlatform, SShaderCompilerEnvironment& InEnvironment )
	{
		CLightingShadersUtils::ModifyCompilationEnvironment( LT_Directional, InEnvironment );
		CDepthOnlyShadersUtils::ModifyCompilationEnvironment( false, InEnvironment );
	}

	/**
	 * @brief Is need compile shader for platform
	 *
	 * @param InShaderPlatform Shader platform
	 * @param InVFMetaType Vertex factory meta type. If him is nullptr - return general check
	 * @return Return true if need compile shader, else returning false
	 */
	static bool ShouldCache( EShaderPlatform InShaderPlatform, class CVertexFactoryMetaType* InVFMetaType = nullptr )
	{
		return CLightingShadersUtils::ShouldCache( InShaderPlatform, InVFMetaType );
	}
#endif // WITH_EDITOR
};

/**
 * @ingroup Engine
 * @brief Base class of lighting pixel shader
 */
class CBaseLightingPixelShader : public CShader
{
public:
	/**
	 * @brief Initialize shader
	 * @param[in] InShaderCacheItem Cache of shader
	 */
	virtual void Init( const CShaderCache::SShaderCacheItem& InShaderCacheItem ) override;

	/**
	 * @brief Set AlbedoRoughnessGBuffer texture parameter
	 *
	 * @param InDeviceContextRHI	RHI device context
	 * @param InTextureRHI			RHI Texture
	 */
	FORCEINLINE void SetAlbedoRoughnessGBufferTexture( class CBaseDeviceContextRHI* InDeviceContextRHI, const Texture2DRHIParamRef_t InTextureRHI )
	{
		SetTextureParameter( InDeviceContextRHI, albedoRoughnessGBufferParameter, InTextureRHI );
	}

	/**
	 * @brief Set AlbedoRoughnessGBuffer sampler state parameter
	 *
	 * @param InDeviceContextRHI	RHI device context
	 * @param InSamplerStateRHI		RHI sampler state
	 */
	FORCEINLINE void SetAlbedoRoughnessGBufferSamplerState( class CBaseDeviceContextRHI* InDeviceContextRHI, const SamplerStateRHIParamRef_t InSamplerStateRHI )
	{
		SetSamplerStateParameter( InDeviceContextRHI, albedoRoughnessGBufferSamplerParameter, InSamplerStateRHI );
	}

	/**
	 * @brief Set NormalMetalGBuffer texture parameter
	 *
	 * @param InDeviceContextRHI	RHI device context
	 * @param InTextureRHI			RHI Texture
	 */
	FORCEINLINE void SetNormalMetalGBufferTexture( class CBaseDeviceContextRHI* InDeviceContextRHI, const Texture2DRHIParamRef_t InTextureRHI )
	{
		SetTextureParameter( InDeviceContextRHI, normalMetalGBufferParameter, InTextureRHI );
	}

	/**
	 * @brief Set NormalMetalGBuffer sampler state parameter
	 *
	 * @param InDeviceContextRHI	RHI device context
	 * @param InSamplerStateRHI		RHI sampler state
	 */
	FORCEINLINE void SetNormalMetalGBufferSamplerState( class CBaseDeviceContextRHI* InDeviceContextRHI, const SamplerStateRHIParamRef_t InSamplerStateRHI )
	{
		SetSamplerStateParameter( InDeviceContextRHI, normalMetalGBufferSamplerParameter, InSamplerStateRHI );
	}

	/**
	 * @brief Set EmissionGBuffer texture parameter
	 *
	 * @param InDeviceContextRHI	RHI device context
	 * @param InTextureRHI			RHI Texture
	 */
	FORCEINLINE void SetEmissionGBufferTexture( class CBaseDeviceContextRHI* InDeviceContextRHI, const Texture2DRHIParamRef_t InTextureRHI )
	{
		SetTextureParameter( InDeviceContextRHI, emissionGBufferParameter, InTextureRHI );
	}

	/**
	 * @brief Set EmissionGBuffer sampler state parameter
	 *
	 * @param InDeviceContextRHI	RHI device context
	 * @param InSamplerStateRHI		RHI sampler state
	 */
	FORCEINLINE void SetEmissionGBufferSamplerState( class CBaseDeviceContextRHI* InDeviceContextRHI, const SamplerStateRHIParamRef_t InSamplerStateRHI )
	{
		SetSamplerStateParameter( InDeviceContextRHI, emissionGBufferSamplerParameter, InSamplerStateRHI );
	}

	/**
	 * @brief Set depth buffer texture parameter
	 *
	 * @param InDeviceContextRHI	RHI device context
	 * @param InTextureRHI			RHI Texture
	 */
	FORCEINLINE void SetDepthBufferTexture( class CBaseDeviceContextRHI* InDeviceContextRHI, const Texture2DRHIParamRef_t InTextureRHI )
	{
		SetTextureParameter( InDeviceContextRHI, depthBufferParameter, InTextureRHI );
	}

	/**
	 * @brief Set depth buffer sampler state parameter
	 *
	 * @param InDeviceContextRHI	RHI device context
	 * @param InSamplerStateRHI		RHI sampler state
	 */
	FORCEINLINE void SetDepthBufferSamplerState( class CBaseDeviceContextRHI* InDeviceContextRHI, const SamplerStateRHIParamRef_t InSamplerStateRHI )
	{
		SetSamplerStateParameter( InDeviceContextRHI, depthBufferSamplerParameter, InSamplerStateRHI );
	}

	/**
	 * @brief Set gamma
	 * 
	 * @param InDeviceContextRHI	RHI device context
	 * @param InGamma				Gamma
	 */
	FORCEINLINE void SetGamma( class CBaseDeviceContextRHI* InDeviceContextRHI, float InGamma )
	{
		SetPixelShaderValue( InDeviceContextRHI, gammaParameter, InGamma );
	}

private:
	CShaderResourceParameter		albedoRoughnessGBufferParameter;			/**< AlbedoRoughnessGBuffer texture parameter */
	CShaderResourceParameter		albedoRoughnessGBufferSamplerParameter;		/**< AlbedoRoughnessGBuffer sampler parameter */
	CShaderResourceParameter		normalMetalGBufferParameter;				/**< NormalMetalGBuffer texture parameter */
	CShaderResourceParameter		normalMetalGBufferSamplerParameter;			/**< NormalMetalGBuffer sampler parameter */
	CShaderResourceParameter		emissionGBufferParameter;					/**< EmissionGBuffer texture parameter */
	CShaderResourceParameter		emissionGBufferSamplerParameter;			/**< EmissionGBuffer sampler parameter */
	CShaderResourceParameter		depthBufferParameter;						/**< Depth buffer texture parameter */
	CShaderResourceParameter		depthBufferSamplerParameter;				/**< Depth buffer sampler parameter */
	CShaderParameter				gammaParameter;								/**< Gamma parameter */
};

/**
 * @ingroup Engine
 * @brief Empty template class of lighting pixel shader (don't use!)
 */
template<ELightType InType>
class TLightingPixelShader : public CBaseLightingPixelShader
{
	static_assert( InType == LT_Unknown, "This template class cannot be used" );
};

/**
 * @ingroup Engine
 * @brief Class of point lighting pixel shader
 */
template<>
class TLightingPixelShader<LT_Point> : public CBaseLightingPixelShader
{
	DECLARE_SHADER_TYPE( TLightingPixelShader<LT_Point> )

public:
#if WITH_EDITOR
	/**
	 * @brief Modify compilation environment
	 *
	 * @param InShaderPlatform Shader platform
	 * @param InEnvironment Shader compiler environment
	 */
	static void ModifyCompilationEnvironment( EShaderPlatform InShaderPlatform, SShaderCompilerEnvironment& InEnvironment )
	{
		CLightingShadersUtils::ModifyCompilationEnvironment( LT_Point, InEnvironment );
	}

	/**
	 * @brief Is need compile shader for platform
	 *
	 * @param InShaderPlatform Shader platform
	 * @param InVFMetaType Vertex factory meta type. If him is nullptr - return general check
	 * @return Return true if need compile shader, else returning false
	 */
	static bool ShouldCache( EShaderPlatform InShaderPlatform, class CVertexFactoryMetaType* InVFMetaType = nullptr )
	{
		return CLightingShadersUtils::ShouldCache( InShaderPlatform, InVFMetaType );
	}
#endif // WITH_EDITOR
};

/**
 * @ingroup Engine
 * @brief Class of spot lighting vertex shader
 */
template<>
class TLightingPixelShader<LT_Spot> : public CBaseLightingPixelShader
{
	DECLARE_SHADER_TYPE( TLightingPixelShader<LT_Spot> )

public:
#if WITH_EDITOR
	/**
	 * @brief Modify compilation environment
	 *
	 * @param InShaderPlatform Shader platform
	 * @param InEnvironment Shader compiler environment
	 */
	static void ModifyCompilationEnvironment( EShaderPlatform InShaderPlatform, SShaderCompilerEnvironment& InEnvironment )
	{
		CLightingShadersUtils::ModifyCompilationEnvironment( LT_Spot, InEnvironment );
	}

	/**
	 * @brief Is need compile shader for platform
	 *
	 * @param InShaderPlatform Shader platform
	 * @param InVFMetaType Vertex factory meta type. If him is nullptr - return general check
	 * @return Return true if need compile shader, else returning false
	 */
	static bool ShouldCache( EShaderPlatform InShaderPlatform, class CVertexFactoryMetaType* InVFMetaType = nullptr )
	{
		return CLightingShadersUtils::ShouldCache( InShaderPlatform, InVFMetaType );
	}
#endif // WITH_EDITOR
};

/**
 * @ingroup Engine
 * @brief Class of directional lighting vertex shader
 */
template<>
class TLightingPixelShader<LT_Directional> : public CBaseLightingPixelShader
{
	DECLARE_SHADER_TYPE( TLightingPixelShader<LT_Directional> )

public:
#if WITH_EDITOR
	/**
	 * @brief Modify compilation environment
	 *
	 * @param InShaderPlatform Shader platform
	 * @param InEnvironment Shader compiler environment
	 */
	static void ModifyCompilationEnvironment( EShaderPlatform InShaderPlatform, SShaderCompilerEnvironment& InEnvironment )
	{
		CLightingShadersUtils::ModifyCompilationEnvironment( LT_Directional, InEnvironment );
	}

	/**
	 * @brief Is need compile shader for platform
	 *
	 * @param InShaderPlatform Shader platform
	 * @param InVFMetaType Vertex factory meta type. If him is nullptr - return general check
	 * @return Return true if need compile shader, else returning false
	 */
	static bool ShouldCache( EShaderPlatform InShaderPlatform, class CVertexFactoryMetaType* InVFMetaType = nullptr )
	{
		return CLightingShadersUtils::ShouldCache( InShaderPlatform, InVFMetaType );
	}
#endif // WITH_EDITOR
};

#endif // !LIGHTINGSHADER_H