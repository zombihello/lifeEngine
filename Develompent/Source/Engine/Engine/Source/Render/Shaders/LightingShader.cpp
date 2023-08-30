#include "Math/Math.h"
#include "Render/Shaders/LightingShader.h"
#include "Render/VertexFactory/VertexFactory.h"
#include "Render/VertexFactory/SimpleElementVertexFactory.h"

// Vertex shaders
IMPLEMENT_SHADER_TYPE(, TLightingVertexShader<LT_Point>, TEXT( "LightingVertexShaders.hlsl" ), TEXT( "MainVS" ), SF_Vertex, true );
IMPLEMENT_SHADER_TYPE(, TLightingVertexShader<LT_Spot>, TEXT( "LightingVertexShaders.hlsl" ), TEXT( "MainVS" ), SF_Vertex, true );
IMPLEMENT_SHADER_TYPE(, TLightingVertexShader<LT_Directional>, TEXT( "LightingVertexShaders.hlsl" ), TEXT( "MainVS" ), SF_Vertex, true );
IMPLEMENT_SHADER_TYPE(, TDepthOnlyLightingVertexShader<LT_Point>, TEXT( "DepthOnlyVertexShader.hlsl" ), TEXT( "MainVS" ), SF_Vertex, true );
IMPLEMENT_SHADER_TYPE(, TDepthOnlyLightingVertexShader<LT_Spot>, TEXT( "DepthOnlyVertexShader.hlsl" ), TEXT( "MainVS" ), SF_Vertex, true );
IMPLEMENT_SHADER_TYPE(, TDepthOnlyLightingVertexShader<LT_Directional>, TEXT( "DepthOnlyVertexShader.hlsl" ), TEXT( "MainVS" ), SF_Vertex, true );

// Pixel shaders
IMPLEMENT_SHADER_TYPE(, TLightingPixelShader<LT_Point>, TEXT( "LightingPixelShaders.hlsl" ), TEXT( "MainPS" ), SF_Pixel, true );
IMPLEMENT_SHADER_TYPE(, TLightingPixelShader<LT_Spot>, TEXT( "LightingPixelShaders.hlsl" ), TEXT( "MainPS" ), SF_Pixel, true );
IMPLEMENT_SHADER_TYPE(, TLightingPixelShader<LT_Directional>, TEXT( "LightingPixelShaders.hlsl" ), TEXT( "MainPS" ), SF_Pixel, true );

/*
==================
CBaseLightingVertexShader::CBaseLightingVertexShader
==================
*/
CBaseLightingVertexShader::CBaseLightingVertexShader()
	: vertexFactoryParameters( nullptr )
{}

/*
==================
CBaseLightingVertexShader::~CBaseLightingVertexShader
==================
*/
CBaseLightingVertexShader::~CBaseLightingVertexShader()
{
	if ( vertexFactoryParameters )
	{
		delete vertexFactoryParameters;
	}
}

/*
==================
CBaseLightingVertexShader::Init
==================
*/
void CBaseLightingVertexShader::Init( const CShaderCache::ShaderCacheItem& InShaderCacheItem )
{
	CShader::Init( InShaderCacheItem );

	// Supported only light vertex factory
	Assert( GetVertexFactoryHash() == CLightVertexFactory::staticType.GetHash() );

	// Bind shader parameters
	vertexFactoryParameters = ( CLightVertexShaderParameters* )CLightVertexFactory::staticType.CreateShaderParameters( SF_Vertex );
	vertexFactoryParameters->Bind( InShaderCacheItem.parameterMap );
}

/*
==================
CBaseLightingVertexShader::SetConstantParameters
==================
*/
void CBaseLightingVertexShader::SetConstantParameters( class CBaseDeviceContextRHI* InDeviceContextRHI, const class CVertexFactory* InVertexFactory, const TSharedPtr<class CMaterial>& InMaterialResource ) const
{
	Assert( vertexFactoryParameters );
	vertexFactoryParameters->Set( InDeviceContextRHI, InVertexFactory );
}

/*
==================
CBaseLightingVertexShader::SetMesh
==================
*/
void CBaseLightingVertexShader::SetMesh( class CBaseDeviceContextRHI* InDeviceContextRHI, const struct MeshBatch& InMesh, const class CVertexFactory* InVertexFactory, const class CSceneView* InView, uint32 InNumInstances /* = 1 */, uint32 InStartInstanceID /* = 0 */ ) const
{
	Sys_Errorf( TEXT( "CBaseLightingVertexShader::SetMesh( MeshBatch ) Not supported" ) );
}


/*
==================
CBaseLightingPixelShader::Init
==================
*/
void CBaseLightingPixelShader::Init( const CShaderCache::ShaderCacheItem& InShaderCacheItem )
{
	CShader::Init( InShaderCacheItem );

	// Diffuse Roughness GBuffer
	albedoRoughnessGBufferParameter.Bind( InShaderCacheItem.parameterMap, TEXT( "albedoRoughnessGBufferTexture" ), true );
	albedoRoughnessGBufferSamplerParameter.Bind( InShaderCacheItem.parameterMap, TEXT( "albedoRoughnessGBufferSampler" ), true );

	// Normal Metal GBuffer
	normalMetalGBufferParameter.Bind( InShaderCacheItem.parameterMap, TEXT( "normalMetalGBufferTexture" ), true );
	normalMetalGBufferSamplerParameter.Bind( InShaderCacheItem.parameterMap, TEXT( "normalMetalGBufferSampler" ), true );

	// Emission GBuffer
	emissionGBufferParameter.Bind( InShaderCacheItem.parameterMap, TEXT( "emissionGBufferTexture" ), true );
	emissionGBufferSamplerParameter.Bind( InShaderCacheItem.parameterMap, TEXT( "emissionGBufferSampler" ), true );

	// Depth buffer
	depthBufferParameter.Bind( InShaderCacheItem.parameterMap, TEXT( "depthBufferTexture" ), true );
	depthBufferSamplerParameter.Bind( InShaderCacheItem.parameterMap, TEXT( "depthBufferSampler" ), true );

	// Gamma
	gammaParameter.Bind( InShaderCacheItem.parameterMap, TEXT( "gamma" ), true );
}