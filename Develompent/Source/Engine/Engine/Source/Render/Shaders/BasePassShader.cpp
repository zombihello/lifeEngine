#include "Misc/CoreGlobals.h"
#include "System/Package.h"
#include "Render/Shaders/BasePassShader.h"
#include "Render/RenderUtils.h"
#include "Render/VertexFactory/VertexFactory.h"
#include "Render/Scene.h"
#include "RHI/StaticStatesRHI.h"

IMPLEMENT_SHADER_TYPE(, CBasePassVertexShader, TEXT( "BasePassVertexShader.hlsl" ), TEXT( "MainVS" ), SF_Vertex, false );
IMPLEMENT_SHADER_TYPE(, CBasePassPixelShader, TEXT( "BasePassPixelShader.hlsl" ), TEXT( "MainPS" ), SF_Pixel, false );

/*
==================
CBasePassVertexShader::CBasePassVertexShader
==================
*/
CBasePassVertexShader::CBasePassVertexShader()
    : vertexFactoryParameters( nullptr )
{}

/*
==================
CBasePassVertexShader::~CBasePassVertexShader
==================
*/
CBasePassVertexShader::~CBasePassVertexShader()
{
    if ( vertexFactoryParameters )
    {
        delete vertexFactoryParameters;
    }
}

/*
==================
CBasePassVertexShader::Init
==================
*/
void CBasePassVertexShader::Init( const CShaderCache::ShaderCacheItem& InShaderCacheItem )
{
    CShader::Init( InShaderCacheItem );

    // Bind shader parameters
    CVertexFactoryMetaType*			vertexFactoryType = CVertexFactoryMetaType::ContainerVertexFactoryMetaType::Get()->FindRegisteredType( GetVertexFactoryHash() );
    Assert( vertexFactoryType );

    vertexFactoryParameters = vertexFactoryType->CreateShaderParameters( SF_Vertex );
    vertexFactoryParameters->Bind( InShaderCacheItem.parameterMap );
}

/*
==================
CBasePassVertexShader::SetConstantParameters
==================
*/
void CBasePassVertexShader::SetConstantParameters( class CBaseDeviceContextRHI* InDeviceContextRHI, const class CVertexFactory* InVertexFactory, const TSharedPtr<class CMaterial>& InMaterialResource ) const
{
    Assert( vertexFactoryParameters );
    vertexFactoryParameters->Set( InDeviceContextRHI, InVertexFactory );
}

/*
==================
CBasePassVertexShader::SetMesh
==================
*/
void CBasePassVertexShader::SetMesh( class CBaseDeviceContextRHI* InDeviceContextRHI, const struct MeshBatch& InMesh, const class CVertexFactory* InVertexFactory, const class CSceneView* InView, uint32 InNumInstances /* = 1 */, uint32 InStartInstanceID /* = 0 */ ) const
{
    Assert( vertexFactoryParameters );
    vertexFactoryParameters->SetMesh( InDeviceContextRHI, InMesh, InVertexFactory, InView, InNumInstances, InStartInstanceID );
}


/*
==================
CBasePassPixelShader::Init
==================
*/
void CBasePassPixelShader::Init( const CShaderCache::ShaderCacheItem& InShaderCacheItem )
{
    CShader::Init( InShaderCacheItem );

    // Albedo
    albedoParameter.Bind( InShaderCacheItem.parameterMap, TEXT( "albedoTexture" ) );
    albedoSamplerParameter.Bind( InShaderCacheItem.parameterMap, TEXT( "albedoSampler" ) );

    // Normal
    normalParameter.Bind( InShaderCacheItem.parameterMap, TEXT( "normalTexture" ), true );              // TODO: Need remove 'true', because Normal, Metallic and Roughness this is non-optional parameters.
    normalSamplerParameter.Bind( InShaderCacheItem.parameterMap, TEXT( "normalSampler" ), true );

    // Metallic
    metallicParameter.Bind( InShaderCacheItem.parameterMap, TEXT( "metallicTexture" ), true );
    metallicSamplerParameter.Bind( InShaderCacheItem.parameterMap, TEXT( "metallicSampler" ), true );

    // Roughness
    roughnessParameter.Bind( InShaderCacheItem.parameterMap, TEXT( "roughnessTexture" ), true );
    roughnessSamplerParameter.Bind( InShaderCacheItem.parameterMap, TEXT( "roughnessSampler" ), true );

    // Emission
	emissionParameter.Bind( InShaderCacheItem.parameterMap, TEXT( "emissionTexture" ), true );
	emissionSamplerParameter.Bind( InShaderCacheItem.parameterMap, TEXT( "emissionSampler" ), true );

	// AO
	aoParameter.Bind( InShaderCacheItem.parameterMap, TEXT( "aoTexture" ), true );
	aoSamplerParameter.Bind( InShaderCacheItem.parameterMap, TEXT( "aoSampler" ), true );
}

/*
==================
CBasePassPixelShader::SetConstantParameters
==================
*/
void CBasePassPixelShader::SetConstantParameters( class CBaseDeviceContextRHI* InDeviceContextRHI, const class CVertexFactory* InVertexFactory, const TSharedPtr<class CMaterial>& InMaterialResource ) const
{
    Assert( InMaterialResource );

    // Bind diffuse texture
    TAssetHandle<CTexture2D>    albedoTexture;
    InMaterialResource->GetTextureParameterValue( CMaterial::albedoTextureParamName, albedoTexture );
    if ( albedoTexture.IsAssetValid() )
    {
        TSharedPtr<CTexture2D>		texture2DRef = albedoTexture.ToSharedPtr();
        SetTextureParameter( InDeviceContextRHI, albedoParameter, texture2DRef->GetTexture2DRHI() );
        SetSamplerStateParameter( InDeviceContextRHI, albedoSamplerParameter, g_RHI->CreateSamplerState( texture2DRef->GetSamplerStateInitialiser() ) );
    }
	else
	{
		SetTextureParameter( InDeviceContextRHI, albedoParameter, g_BlackTexture.GetTexture2DRHI() );
		SetSamplerStateParameter( InDeviceContextRHI, albedoSamplerParameter, TStaticSamplerStateRHI<>::GetRHI() );
	}

    // Getting normal texture
    TAssetHandle<CTexture2D>    normalTexture;
    InMaterialResource->GetTextureParameterValue( CMaterial::normalTextureParamName, normalTexture );
    if ( normalTexture.IsAssetValid() && !g_PackageManager->IsDefaultAsset( normalTexture ) )
    {
		TSharedPtr<CTexture2D>		texture2DRef = normalTexture.ToSharedPtr();
		SetTextureParameter( InDeviceContextRHI, normalParameter, texture2DRef->GetTexture2DRHI() );
		SetSamplerStateParameter( InDeviceContextRHI, normalSamplerParameter, g_RHI->CreateSamplerState( texture2DRef->GetSamplerStateInitialiser() ) );
    }
	else
	{
		SetTextureParameter( InDeviceContextRHI, normalParameter, g_EmptyNormalTexture.GetTexture2DRHI() );
		SetSamplerStateParameter( InDeviceContextRHI, normalSamplerParameter, TStaticSamplerStateRHI<>::GetRHI() );
	}

    // Getting metallic texture
    TAssetHandle<CTexture2D>    metallicTexture;
    InMaterialResource->GetTextureParameterValue( CMaterial::metallicTextureParamName, metallicTexture );
    if ( metallicTexture.IsValid() && !g_PackageManager->IsDefaultAsset( metallicTexture ) )
    {
		TSharedPtr<CTexture2D>		texture2DRef = metallicTexture.ToSharedPtr();
		SetTextureParameter( InDeviceContextRHI, metallicParameter, texture2DRef->GetTexture2DRHI() );
		SetSamplerStateParameter( InDeviceContextRHI, metallicSamplerParameter, g_RHI->CreateSamplerState( texture2DRef->GetSamplerStateInitialiser() ) );
    }
	else
	{
		SetTextureParameter( InDeviceContextRHI, metallicParameter, g_BlackTexture.GetTexture2DRHI() );
		SetSamplerStateParameter( InDeviceContextRHI, metallicSamplerParameter, TStaticSamplerStateRHI<>::GetRHI() );
	}

    // Getting roughness texture
    TAssetHandle<CTexture2D>    roughnessTexture;
    InMaterialResource->GetTextureParameterValue( CMaterial::roughnessTextureParamName, roughnessTexture );
    if ( roughnessTexture.IsAssetValid() && !g_PackageManager->IsDefaultAsset( roughnessTexture ) )
    {
		TSharedPtr<CTexture2D>		texture2DRef = roughnessTexture.ToSharedPtr();
		SetTextureParameter( InDeviceContextRHI, roughnessParameter, texture2DRef->GetTexture2DRHI() );
		SetSamplerStateParameter( InDeviceContextRHI, roughnessSamplerParameter, g_RHI->CreateSamplerState( texture2DRef->GetSamplerStateInitialiser() ) );
    }
	else
	{
		SetTextureParameter( InDeviceContextRHI, roughnessParameter, g_BlackTexture.GetTexture2DRHI() );
		SetSamplerStateParameter( InDeviceContextRHI, roughnessSamplerParameter, TStaticSamplerStateRHI<>::GetRHI() );
	}

    // Getting emission texture
    TAssetHandle<CTexture2D>    emissionTexture;
    InMaterialResource->GetTextureParameterValue( CMaterial::emissionTextureParamName, emissionTexture );
    if ( emissionTexture.IsAssetValid() && !g_PackageManager->IsDefaultAsset( emissionTexture ) )
    {
        TSharedPtr<CTexture2D>      texture2DRef = emissionTexture.ToSharedPtr();
		SetTextureParameter( InDeviceContextRHI, emissionParameter, texture2DRef->GetTexture2DRHI() );
		SetSamplerStateParameter( InDeviceContextRHI, emissionSamplerParameter, g_RHI->CreateSamplerState( texture2DRef->GetSamplerStateInitialiser() ) );
    }
    else
    {
		SetTextureParameter( InDeviceContextRHI, emissionParameter, g_BlackTexture.GetTexture2DRHI() );
		SetSamplerStateParameter( InDeviceContextRHI, emissionSamplerParameter, TStaticSamplerStateRHI<>::GetRHI() );
    }

	// Getting AO texture
	TAssetHandle<CTexture2D>    aoTexture;
	InMaterialResource->GetTextureParameterValue( CMaterial::aoTextureParamName, aoTexture );
	if ( aoTexture.IsAssetValid() && !g_PackageManager->IsDefaultAsset( aoTexture ) )
	{
		TSharedPtr<CTexture2D>      texture2DRef = aoTexture.ToSharedPtr();
		SetTextureParameter( InDeviceContextRHI, aoParameter, texture2DRef->GetTexture2DRHI() );
		SetSamplerStateParameter( InDeviceContextRHI, aoSamplerParameter, g_RHI->CreateSamplerState( texture2DRef->GetSamplerStateInitialiser() ) );
	}
	else
	{
		SetTextureParameter( InDeviceContextRHI, aoParameter, g_WhiteTexture.GetTexture2DRHI() );
		SetSamplerStateParameter( InDeviceContextRHI, aoSamplerParameter, TStaticSamplerStateRHI<>::GetRHI() );
	}
}

#if WITH_EDITOR
#include "Render/VertexFactory/LightVertexFactory.h"

/*
==================
CBasePassVertexShader::ShouldCache
==================
*/
bool CBasePassVertexShader::ShouldCache( EShaderPlatform InShaderPlatform, class CVertexFactoryMetaType* InVFMetaType /* = nullptr */ )
{
	if ( !InVFMetaType )
	{
		return true;
	}

	return InVFMetaType->GetHash() != CLightVertexFactory::staticType.GetHash();
}

/*
==================
CBasePassPixelShader::ShouldCache
==================
*/
bool CBasePassPixelShader::ShouldCache( EShaderPlatform InShaderPlatform, class CVertexFactoryMetaType* InVFMetaType /* = nullptr */ )
{
	if ( !InVFMetaType )
	{
		return true;
	}

	return InVFMetaType->GetHash() != CLightVertexFactory::staticType.GetHash();
}
#endif // WITH_EDITOR