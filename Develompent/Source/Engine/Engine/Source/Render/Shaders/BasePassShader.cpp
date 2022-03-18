#include "Render/Shaders/BasePassShader.h"
#include "Render/VertexFactory/VertexFactory.h"

IMPLEMENT_SHADER_TYPE( FBasePassVertexShader, TEXT( "BasePassVertexShader.hlsl" ), TEXT( "MainVS" ), SF_Vertex );
IMPLEMENT_SHADER_TYPE( FBasePassPixelShader, TEXT( "BasePassPixelShader.hlsl" ), TEXT( "MainPS" ), SF_Pixel );

FBasePassVertexShader::FBasePassVertexShader()
    : vertexFactoryParameters( nullptr )
{}

FBasePassVertexShader::~FBasePassVertexShader()
{
    if ( vertexFactoryParameters )
    {
        delete vertexFactoryParameters;
    }
}

void FBasePassVertexShader::Init( const FShaderCache::FShaderCacheItem& InShaderCacheItem )
{
    FShader::Init( InShaderCacheItem );

    // Bind shader parameters
    FVertexFactoryMetaType*			vertexFactoryType = FVertexFactoryMetaType::FContainerVertexFactoryMetaType::Get()->FindRegisteredType( GetVertexFactoryHash() );
    check( vertexFactoryType );

    vertexFactoryParameters = vertexFactoryType->CreateShaderParameters( SF_Vertex );
    vertexFactoryParameters->Bind( InShaderCacheItem.parameterMap );
}

void FBasePassVertexShader::SetMesh( class FBaseDeviceContextRHI* InDeviceContextRHI, const struct FMeshBatch& InMesh, uint32 InBatchElementIndex, const class FSceneView* InView ) const
{
    check( vertexFactoryParameters );
    vertexFactoryParameters->SetMesh( InDeviceContextRHI, InMesh, InBatchElementIndex, InView );
}
