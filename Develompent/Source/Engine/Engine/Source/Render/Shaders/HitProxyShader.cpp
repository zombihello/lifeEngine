#include "LEBuild.h"

#if ENABLE_HITPROXY
#include "Render/Shaders/HitProxyShader.h"
#include "Render/VertexFactory/VertexFactory.h"
#include "Render/Material.h"
#include "Render/Scene.h"

IMPLEMENT_SHADER_TYPE( , CHitProxyVertexShader, TEXT( "HitProxyShaders.hlsl" ), TEXT( "MainVS" ), SF_Vertex, true );
IMPLEMENT_SHADER_TYPE( , CHitProxyPixelShader, TEXT( "HitProxyShaders.hlsl" ), TEXT( "MainPS" ), SF_Pixel, true );

/*
==================
CHitProxyVertexShader::CHitProxyVertexShader
==================
*/
CHitProxyVertexShader::CHitProxyVertexShader()
	: vertexFactoryParameters( nullptr )
{}

/*
==================
CHitProxyVertexShader::~CHitProxyVertexShader
==================
*/
CHitProxyVertexShader::~CHitProxyVertexShader()
{
	if ( vertexFactoryParameters )
	{
		delete vertexFactoryParameters;
	}
}

#if WITH_EDITOR
#include "Render/VertexFactory/LightVertexFactory.h"

/*
==================
CHitProxyVertexShader::CHitProxyVertexShader
==================
*/
bool CHitProxyVertexShader::ShouldCache( EShaderPlatform InShaderPlatform, class CVertexFactoryMetaType* InVFMetaType /* = nullptr */ )
{
	return !g_IsCooker && ( !InVFMetaType || InVFMetaType->GetHash() != CLightVertexFactory::staticType.GetHash() );
}

/*
==================
CHitProxyPixelShader::ShouldCache
==================
*/
bool CHitProxyPixelShader::ShouldCache( EShaderPlatform InShaderPlatform, class CVertexFactoryMetaType* InVFMetaType /* = nullptr */ )
{
	return !g_IsCooker && ( !InVFMetaType || InVFMetaType->GetHash() != CLightVertexFactory::staticType.GetHash() );
}
#endif // WITH_EDITOR

/*
==================
CHitProxyVertexShader::Init
==================
*/
void CHitProxyVertexShader::Init( const CShaderCache::SShaderCacheItem& InShaderCacheItem )
{
	CShader::Init( InShaderCacheItem );

	// Bind shader parameters
	CVertexFactoryMetaType* vertexFactoryType = CVertexFactoryMetaType::SContainerVertexFactoryMetaType::Get()->FindRegisteredType( GetVertexFactoryHash() );
	Assert( vertexFactoryType );

	vertexFactoryParameters = vertexFactoryType->CreateShaderParameters( SF_Vertex );	
	if ( !vertexFactoryType->SupportsInstancing() )
	{
		hitProxyIdParameter.Bind( InShaderCacheItem.parameterMap, TEXT( "hitProxyId" ) );
	}

	vertexFactoryParameters->Bind( InShaderCacheItem.parameterMap );
}

/*
==================
CHitProxyVertexShader::SetConstantParameters
==================
*/
void CHitProxyVertexShader::SetConstantParameters( class CBaseDeviceContextRHI* InDeviceContextRHI, const class CVertexFactory* InVertexFactory, const TSharedPtr<class CMaterial>& InMaterialResource ) const
{
	Assert( vertexFactoryParameters );
	vertexFactoryParameters->Set( InDeviceContextRHI, InVertexFactory );
}

/*
==================
CHitProxyVertexShader::SetMesh
==================
*/
void CHitProxyVertexShader::SetMesh( class CBaseDeviceContextRHI* InDeviceContextRHI, const struct SMeshBatch& InMesh, const class CVertexFactory* InVertexFactory, const class CSceneView* InView, uint32 InNumInstances /* = 1 */, uint32 InStartInstanceID /* = 0 */ ) const
{
	Assert( vertexFactoryParameters );
	vertexFactoryParameters->SetMesh( InDeviceContextRHI, InMesh, InVertexFactory, InView, InNumInstances, InStartInstanceID );

	// Set hit proxy id for not instancing version
	if ( !InVertexFactory->SupportsInstancing() )
	{
		Assert( InNumInstances == 1 );
		SetVertexShaderValue( InDeviceContextRHI, hitProxyIdParameter, InMesh.instances[ InStartInstanceID ].hitProxyId.GetColor().ToNormalizedVector4D() );
	}
}
#endif // ENABLE_HITPROXY