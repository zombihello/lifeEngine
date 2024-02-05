/**
 * @file
 * @addtogroup Engine Engine
 *
 * ************************************************************
 *                  This file is part of:
 *                      LIFEENGINE
 *          https://github.com/zombihello/lifeEngine
 * ************************************************************
 * Copyright (C) 2024 Yehor Pohuliaka.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef DEPTHSONLYHADER_H
#define DEPTHSONLYHADER_H

#include "Engine/Render/Shaders/Shader.h"
#include "Engine/Render/Shaders/ShaderManager.h"

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
	static FORCEINLINE void ModifyCompilationEnvironment( bool InIsUseWorldPos, ShaderCompilerEnvironment& InEnvironment )
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
	virtual void Init( const CShaderCache::ShaderCacheItem& InShaderCacheItem ) override
	{
		CShader::Init( InShaderCacheItem );

		// Bind shader parameters
		CVertexFactoryMetaType* vertexFactoryType = CVertexFactoryMetaType::ContainerVertexFactoryMetaType::Get()->FindRegisteredType( GetVertexFactoryHash() );
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
	virtual void SetMesh( class CBaseDeviceContextRHI* InDeviceContextRHI, const struct MeshBatch& InMesh, const class CVertexFactory* InVertexFactory, const class CSceneView* InView, uint32 InNumInstances = 1, uint32 InStartInstanceID = 0 ) const override
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
	static void ModifyCompilationEnvironment( EShaderPlatform InShaderPlatform, ShaderCompilerEnvironment& InEnvironment )
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