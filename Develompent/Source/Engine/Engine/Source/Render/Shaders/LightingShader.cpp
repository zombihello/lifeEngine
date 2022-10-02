#include "Math/Math.h"
#include "Render/Shaders/LightingShader.h"
#include "Render/VertexFactory/VertexFactory.h"

// Vertex shaders
IMPLEMENT_SHADER_TYPE(, TLightingVertexShader<LT_Point>, TEXT( "LightingVertexShaders.hlsl" ), TEXT( "MainVS" ), SF_Vertex, true );
IMPLEMENT_SHADER_TYPE(, TLightingVertexShader<LT_Spot>, TEXT( "LightingVertexShaders.hlsl" ), TEXT( "MainVS" ), SF_Vertex, true );
IMPLEMENT_SHADER_TYPE(, TLightingVertexShader<LT_Directional>, TEXT( "LightingVertexShaders.hlsl" ), TEXT( "MainVS" ), SF_Vertex, true );

// Pixel shaders
IMPLEMENT_SHADER_TYPE(, TLightingPixelShader<LT_Point>, TEXT( "LightingPixelShaders.hlsl" ), TEXT( "MainPS" ), SF_Pixel, true );
IMPLEMENT_SHADER_TYPE(, TLightingPixelShader<LT_Spot>, TEXT( "LightingPixelShaders.hlsl" ), TEXT( "MainPS" ), SF_Pixel, true );
IMPLEMENT_SHADER_TYPE(, TLightingPixelShader<LT_Directional>, TEXT( "LightingPixelShaders.hlsl" ), TEXT( "MainPS" ), SF_Pixel, true );

CBaseLightingVertexShader::CBaseLightingVertexShader()
	: vertexFactoryParameters( nullptr )
{}

CBaseLightingVertexShader::~CBaseLightingVertexShader()
{
	if ( vertexFactoryParameters )
	{
		delete vertexFactoryParameters;
	}
}

void CBaseLightingVertexShader::Init( const CShaderCache::SShaderCacheItem& InShaderCacheItem )
{
	CShader::Init( InShaderCacheItem );

	// Supported only light vertex factory
	check( GetVertexFactoryHash() == CLightVertexFactory::staticType.GetHash() );

	// Bind shader parameters
	vertexFactoryParameters = ( CLightVertexShaderParameters* )CLightVertexFactory::staticType.CreateShaderParameters( SF_Vertex );
	vertexFactoryParameters->Bind( InShaderCacheItem.parameterMap );
}

void CBaseLightingVertexShader::SetConstantParameters( class CBaseDeviceContextRHI* InDeviceContextRHI, const class CVertexFactory* InVertexFactory, const TSharedPtr<class CMaterial>& InMaterialResource ) const
{
	check( vertexFactoryParameters );
	vertexFactoryParameters->Set( InDeviceContextRHI, InVertexFactory );
}

void CBaseLightingVertexShader::SetMesh( class CBaseDeviceContextRHI* InDeviceContextRHI, const struct SMeshBatch& InMesh, const class CVertexFactory* InVertexFactory, const class CSceneView* InView, uint32 InNumInstances /* = 1 */, uint32 InStartInstanceID /* = 0 */ ) const
{
	appErrorf( TEXT( "CBaseLightingVertexShader::SetMesh( MeshBatch ) Not supported" ) );
}


void CBaseLightingPixelShader::Init( const CShaderCache::SShaderCacheItem& InShaderCacheItem )
{
	CShader::Init( InShaderCacheItem );

	// Diffuse Roughness GBuffer
	diffuseRoughnessGBufferParameter.Bind( InShaderCacheItem.parameterMap, TEXT( "diffuseRoughnessGBufferTexture" ), true );
	diffuseRoughnessGBufferSamplerParameter.Bind( InShaderCacheItem.parameterMap, TEXT( "diffuseRoughnessGBufferSampler" ), true );

	// Normal Metal GBuffer
	normalMetalGBufferParameter.Bind( InShaderCacheItem.parameterMap, TEXT( "normalMetalGBufferTexture" ), true );
	normalMetalGBufferSamplerParameter.Bind( InShaderCacheItem.parameterMap, TEXT( "normalMetalGBufferSampler" ), true );

	// Depth buffer
	depthBufferParameter.Bind( InShaderCacheItem.parameterMap, TEXT( "depthBufferTexture" ), true );
	depthBufferSamplerParameter.Bind( InShaderCacheItem.parameterMap, TEXT( "depthBufferSampler" ), true );
}