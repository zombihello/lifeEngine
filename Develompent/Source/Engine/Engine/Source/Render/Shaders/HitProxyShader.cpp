#include "LEBuild.h"

#if ENABLE_HITPROXY
#include "Render/Shaders/HitProxyShader.h"
#include "Render/VertexFactory/VertexFactory.h"
#include "Render/Material.h"
#include "Render/Scene.h"

IMPLEMENT_SHADER_TYPE( , CHitProxyVertexShader, TEXT( "HitProxyShaders.hlsl" ), TEXT( "MainVS" ), SF_Vertex, true );
IMPLEMENT_SHADER_TYPE( , CHitProxyPixelShader, TEXT( "HitProxyShaders.hlsl" ), TEXT( "MainPS" ), SF_Pixel, true );

CHitProxyVertexShader::CHitProxyVertexShader()
	: vertexFactoryParameters( nullptr )
{}

CHitProxyVertexShader::~CHitProxyVertexShader()
{
	if ( vertexFactoryParameters )
	{
		delete vertexFactoryParameters;
	}
}

#if WITH_EDITOR
#include "Render/VertexFactory/LightVertexFactory.h"

bool CHitProxyVertexShader::ShouldCache( EShaderPlatform InShaderPlatform, class CVertexFactoryMetaType* InVFMetaType /* = nullptr */ )
{
	return !GIsCooker && ( !InVFMetaType || InVFMetaType->GetHash() != CLightVertexFactory::staticType.GetHash() );
}

bool CHitProxyPixelShader::ShouldCache( EShaderPlatform InShaderPlatform, class CVertexFactoryMetaType* InVFMetaType /* = nullptr */ )
{
	return !GIsCooker && ( !InVFMetaType || InVFMetaType->GetHash() != CLightVertexFactory::staticType.GetHash() );
}
#endif // WITH_EDITOR

void CHitProxyVertexShader::Init( const CShaderCache::SShaderCacheItem& InShaderCacheItem )
{
	CShader::Init( InShaderCacheItem );

	// Bind shader parameters
	CVertexFactoryMetaType* vertexFactoryType = CVertexFactoryMetaType::SContainerVertexFactoryMetaType::Get()->FindRegisteredType( GetVertexFactoryHash() );
	check( vertexFactoryType );

	vertexFactoryParameters = vertexFactoryType->CreateShaderParameters( SF_Vertex );	
	if ( !vertexFactoryType->SupportsInstancing() )
	{
		hitProxyIdParameter.Bind( InShaderCacheItem.parameterMap, TEXT( "hitProxyId" ) );
	}

	vertexFactoryParameters->Bind( InShaderCacheItem.parameterMap );
}

void CHitProxyVertexShader::SetConstantParameters( class CBaseDeviceContextRHI* InDeviceContextRHI, const class CVertexFactory* InVertexFactory, const TSharedPtr<class CMaterial>& InMaterialResource ) const
{
	check( vertexFactoryParameters );
	vertexFactoryParameters->Set( InDeviceContextRHI, InVertexFactory );
}

void CHitProxyVertexShader::SetMesh( class CBaseDeviceContextRHI* InDeviceContextRHI, const struct SMeshBatch& InMesh, const class CVertexFactory* InVertexFactory, const class CSceneView* InView, uint32 InNumInstances /* = 1 */, uint32 InStartInstanceID /* = 0 */ ) const
{
	check( vertexFactoryParameters );
	vertexFactoryParameters->SetMesh( InDeviceContextRHI, InMesh, InVertexFactory, InView, InNumInstances, InStartInstanceID );

	// Set hit proxy id for not instancing version
	if ( !InVertexFactory->SupportsInstancing() )
	{
		check( InNumInstances == 1 );
		SetVertexShaderValue( InDeviceContextRHI, hitProxyIdParameter, InMesh.instances[ InStartInstanceID ].hitProxyId.GetColor().ToNormalizedVector4D() );
	}
}
#endif // ENABLE_HITPROXY