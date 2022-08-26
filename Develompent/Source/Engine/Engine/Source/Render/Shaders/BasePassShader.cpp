#include "Render/Shaders/BasePassShader.h"
#include "Render/VertexFactory/VertexFactory.h"
#include "Render/Scene.h"

IMPLEMENT_SHADER_TYPE(, CBasePassVertexShader, TEXT( "BasePassVertexShader.hlsl" ), TEXT( "MainVS" ), SF_Vertex, false );
IMPLEMENT_SHADER_TYPE(, CBasePassPixelShader, TEXT( "BasePassPixelShader.hlsl" ), TEXT( "MainPS" ), SF_Pixel, false );

CBasePassVertexShader::CBasePassVertexShader()
    : vertexFactoryParameters( nullptr )
{}

CBasePassVertexShader::~CBasePassVertexShader()
{
    if ( vertexFactoryParameters )
    {
        delete vertexFactoryParameters;
    }
}

void CBasePassVertexShader::Init( const CShaderCache::SShaderCacheItem& InShaderCacheItem )
{
    CShader::Init( InShaderCacheItem );

    // Bind shader parameters
    CVertexFactoryMetaType*			vertexFactoryType = CVertexFactoryMetaType::SContainerVertexFactoryMetaType::Get()->FindRegisteredType( GetVertexFactoryHash() );
    check( vertexFactoryType );

    vertexFactoryParameters = vertexFactoryType->CreateShaderParameters( SF_Vertex );
    vertexFactoryParameters->Bind( InShaderCacheItem.parameterMap );
}

void CBasePassVertexShader::SetConstantParameters( class CBaseDeviceContextRHI* InDeviceContextRHI, const class CVertexFactory* InVertexFactory, const TSharedPtr<class CMaterial>& InMaterialResource ) const
{
    check( vertexFactoryParameters );
    vertexFactoryParameters->Set( InDeviceContextRHI, InVertexFactory );
}

void CBasePassVertexShader::SetMesh( class CBaseDeviceContextRHI* InDeviceContextRHI, const struct SMeshBatch& InMesh, const class CVertexFactory* InVertexFactory, const class CSceneView* InView, uint32 InNumInstances /* = 1 */, uint32 InStartInstanceID /* = 0 */ ) const
{
    check( vertexFactoryParameters );
    vertexFactoryParameters->SetMesh( InDeviceContextRHI, InMesh, InVertexFactory, InView, InNumInstances, InStartInstanceID );
}


void CBasePassPixelShader::Init( const CShaderCache::SShaderCacheItem& InShaderCacheItem )
{
    CShader::Init( InShaderCacheItem );

    // Diffuse
    diffuseParameter.Bind( InShaderCacheItem.parameterMap, TEXT( "diffuseTexture" ) );
    diffuseSamplerParameter.Bind( InShaderCacheItem.parameterMap, TEXT( "diffuseSampler" ) );

    // Normal
    normalParameter.Bind( InShaderCacheItem.parameterMap, TEXT( "normalTexture" ), true );              // TODO: Need remove 'true', because Normal, Metallic and Roughness this is non-optional parameters.
    normalSamplerParameter.Bind( InShaderCacheItem.parameterMap, TEXT( "normalSampler" ), true );

    // Metallic
    metallicParameter.Bind( InShaderCacheItem.parameterMap, TEXT( "metallicTexture" ), true );
    metallicSamplerParameter.Bind( InShaderCacheItem.parameterMap, TEXT( "metallicSampler" ), true );

    // Roughness
    roughnessParameter.Bind( InShaderCacheItem.parameterMap, TEXT( "roughnessTexture" ), true );
    roughnessSamplerParameter.Bind( InShaderCacheItem.parameterMap, TEXT( "roughnessSampler" ), true );
}

void CBasePassPixelShader::SetConstantParameters( class CBaseDeviceContextRHI* InDeviceContextRHI, const class CVertexFactory* InVertexFactory, const TSharedPtr<class CMaterial>& InMaterialResource ) const
{
    check( InMaterialResource );

    // Bind diffuse texture
    TAssetHandle<CTexture2D>    diffuseTexture;
    InMaterialResource->GetTextureParameterValue( CMaterial::diffuseTextureParamName, diffuseTexture );
    if ( diffuseTexture.IsAssetValid() )
    {
        TSharedPtr<CTexture2D>		texture2DRef = diffuseTexture.ToSharedPtr();
        SetTextureParameter( InDeviceContextRHI, diffuseParameter, texture2DRef->GetTexture2DRHI() );
        SetSamplerStateParameter( InDeviceContextRHI, diffuseSamplerParameter, GRHI->CreateSamplerState( texture2DRef->GetSamplerStateInitialiser() ) );
    }

    // Getting normal texture
    TAssetHandle<CTexture2D>    normalTexture;
    InMaterialResource->GetTextureParameterValue( CMaterial::normalTextureParamName, normalTexture );
    if ( normalTexture.IsAssetValid() )
    {
		TSharedPtr<CTexture2D>		texture2DRef = normalTexture.ToSharedPtr();
		SetTextureParameter( InDeviceContextRHI, normalParameter, texture2DRef->GetTexture2DRHI() );
		SetSamplerStateParameter( InDeviceContextRHI, normalSamplerParameter, GRHI->CreateSamplerState( texture2DRef->GetSamplerStateInitialiser() ) );
    }

    // Getting metallic texture
    TAssetHandle<CTexture2D>    metallicTexture;
    InMaterialResource->GetTextureParameterValue( CMaterial::metallicTextureParamName, metallicTexture );
    if ( metallicTexture.IsValid() )
    {
		TSharedPtr<CTexture2D>		texture2DRef = metallicTexture.ToSharedPtr();
		SetTextureParameter( InDeviceContextRHI, metallicParameter, texture2DRef->GetTexture2DRHI() );
		SetSamplerStateParameter( InDeviceContextRHI, metallicSamplerParameter, GRHI->CreateSamplerState( texture2DRef->GetSamplerStateInitialiser() ) );
    }

    // Getting roughness texture
    TAssetHandle<CTexture2D>    roughnessTexture;
    InMaterialResource->GetTextureParameterValue( CMaterial::roughnessTextureParamName, roughnessTexture );
    if ( roughnessTexture.IsAssetValid() )
    {
		TSharedPtr<CTexture2D>		texture2DRef = roughnessTexture.ToSharedPtr();
		SetTextureParameter( InDeviceContextRHI, roughnessParameter, texture2DRef->GetTexture2DRHI() );
		SetSamplerStateParameter( InDeviceContextRHI, roughnessSamplerParameter, GRHI->CreateSamplerState( texture2DRef->GetSamplerStateInitialiser() ) );
    }
}