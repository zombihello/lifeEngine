#include "Render/Shaders/BasePassShader.h"
#include "Render/VertexFactory/VertexFactory.h"

IMPLEMENT_SHADER_TYPE(, FBasePassVertexShader, TEXT( "BasePassVertexShader.hlsl" ), TEXT( "MainVS" ), SF_Vertex, false );
IMPLEMENT_SHADER_TYPE(, FBasePassPixelShader, TEXT( "BasePassPixelShader.hlsl" ), TEXT( "MainPS" ), SF_Pixel, false );

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

void FBasePassVertexShader::SetConstantParameters( class FBaseDeviceContextRHI* InDeviceContextRHI, const class FVertexFactory* InVertexFactory, const class FMaterial* InMaterialResource ) const
{
    check( vertexFactoryParameters );
    vertexFactoryParameters->Set( InDeviceContextRHI, InVertexFactory );
}

void FBasePassVertexShader::SetMesh( class FBaseDeviceContextRHI* InDeviceContextRHI, const struct FMeshBatch& InMesh, const class FVertexFactory* InVertexFactory, const class FSceneView* InView, uint32 InNumInstances /* = 1 */, uint32 InStartInstanceID /* = 0 */ ) const
{
    check( vertexFactoryParameters );
    vertexFactoryParameters->SetMesh( InDeviceContextRHI, InMesh, InVertexFactory, InView, InNumInstances, InStartInstanceID );
}
