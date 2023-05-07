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
 * @brief Class of utils for depth only shaders
 */
class CDepthOnlyShadersUtils
{
public:
#if WITH_EDITOR
	/**
	 * @brief Modify compilation environment
	 * This method added to the compiler environment base definitions of lighting system
	 *
	 * @param InIsUseWorldPos	Is need use in the shader world position
	 * @param InEnvironment Shader compiler environment
	 */
	static FORCEINLINE void ModifyCompilationEnvironment( bool InIsUseWorldPos, SShaderCompilerEnvironment& InEnvironment )
	{
		InEnvironment.difinitions.insert( std::make_pair( TEXT( "WORLD_POS" ), InIsUseWorldPos ? TEXT( "1" ) : TEXT( "0" ) ) );
	}
#endif // WITH_EDITOR
};

/**
 * @ingroup Engine
 * @brief Class of depth only vertex shader
 */
template<bool InIsUseWorldPos = true>
class TDepthOnlyVertexShader : public CShader
{
	DECLARE_SHADER_TYPE( TDepthOnlyVertexShader )

public:
	/**
	 * @brief Construct a new TDepthOnlyVertexShader object
	 */
	TDepthOnlyVertexShader()
		: vertexFactoryParameters( nullptr )
	{}

	/**
	 * @brief Destructor of a TDepthOnlyVertexShader object
	 */
	virtual ~TDepthOnlyVertexShader()
	{
		if ( vertexFactoryParameters )
		{
			delete vertexFactoryParameters;
		}
	}

	/**
	 * @brief Initialize shader
	 * @param[in] InShaderCacheItem Cache of shader
	 */
	virtual void Init( const CShaderCache::SShaderCacheItem& InShaderCacheItem ) override
	{
		CShader::Init( InShaderCacheItem );

		// Bind shader parameters
		CVertexFactoryMetaType* vertexFactoryType = CVertexFactoryMetaType::SContainerVertexFactoryMetaType::Get()->FindRegisteredType( GetVertexFactoryHash() );
		Assert( vertexFactoryType );

		vertexFactoryParameters = vertexFactoryType->CreateShaderParameters( SF_Vertex );
		vertexFactoryParameters->Bind( InShaderCacheItem.parameterMap );
	}

	/**
	 * @brief Set the constant shader parameters
	 *
	 * @param InDeviceContextRHI Device context
	 * @param InVertexFactory Vertex factory
	 * @param InMaterialResource Material
	 */
	virtual void SetConstantParameters( class CBaseDeviceContextRHI* InDeviceContextRHI, const class CVertexFactory* InVertexFactory, const TSharedPtr<class CMaterial>& InMaterialResource ) const override
	{
		Assert( vertexFactoryParameters );
		vertexFactoryParameters->Set( InDeviceContextRHI, InVertexFactory );
	}

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
	virtual void SetMesh( class CBaseDeviceContextRHI* InDeviceContextRHI, const struct SMeshBatch& InMesh, const class CVertexFactory* InVertexFactory, const class CSceneView* InView, uint32 InNumInstances = 1, uint32 InStartInstanceID = 0 ) const override
	{
		Assert( vertexFactoryParameters );
		vertexFactoryParameters->SetMesh( InDeviceContextRHI, InMesh, InVertexFactory, InView, InNumInstances, InStartInstanceID );
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
		CDepthOnlyShadersUtils::ModifyCompilationEnvironment( InIsUseWorldPos, InEnvironment );
	}
#endif // WITH_EDITOR

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