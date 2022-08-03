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