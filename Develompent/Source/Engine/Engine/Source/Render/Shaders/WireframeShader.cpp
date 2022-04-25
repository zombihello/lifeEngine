#include "Render/Shaders/WireframeShader.h"
#include "Render/VertexFactory/VertexFactory.h"
#include "Render/Material.h"

IMPLEMENT_SHADER_TYPE( FWireframeVertexShader, TEXT( "WireframeShaders.hlsl" ), TEXT( "MainVS" ), SF_Vertex, true );
IMPLEMENT_SHADER_TYPE( FWireframePixelShader, TEXT( "WireframeShaders.hlsl" ), TEXT( "MainPS" ), SF_Pixel, true );

FWireframeVertexShader::FWireframeVertexShader()
	: vertexFactoryParameters( nullptr )
{}

FWireframeVertexShader::~FWireframeVertexShader()
{
	if ( vertexFactoryParameters )
	{
		delete vertexFactoryParameters;
	}
}

#if WITH_EDITOR
bool FWireframeVertexShader::ShouldCache( EShaderPlatform InShaderPlatform )
{
	return GIsEditor || GIsCookEditorContent;
}
#endif // WITH_EDITOR

void FWireframeVertexShader::Init( const FShaderCache::FShaderCacheItem& InShaderCacheItem )
{
	FShader::Init( InShaderCacheItem );

	// Bind shader parameters
	FVertexFactoryMetaType* vertexFactoryType = FVertexFactoryMetaType::FContainerVertexFactoryMetaType::Get()->FindRegisteredType( GetVertexFactoryHash() );
	check( vertexFactoryType );

	vertexFactoryParameters = vertexFactoryType->CreateShaderParameters( SF_Vertex );
	vertexFactoryParameters->Bind( InShaderCacheItem.parameterMap );
}

void FWireframeVertexShader::SetConstantParameters( class FBaseDeviceContextRHI* InDeviceContextRHI, const class FVertexFactory* InVertexFactory, const class FMaterial* InMaterialResource ) const
{
	check( vertexFactoryParameters );
	vertexFactoryParameters->Set( InDeviceContextRHI, InVertexFactory );
}

void FWireframeVertexShader::SetMesh( class FBaseDeviceContextRHI* InDeviceContextRHI, const struct FMeshBatch& InMesh, const class FVertexFactory* InVertexFactory, const class FSceneView* InView, uint32 InNumInstances /* = 1 */, uint32 InStartInstanceID /* = 0 */ ) const
{
	check( vertexFactoryParameters );
	vertexFactoryParameters->SetMesh( InDeviceContextRHI, InMesh, InVertexFactory, InView, InNumInstances, InStartInstanceID );
}

void FWireframePixelShader::Init( const FShaderCache::FShaderCacheItem& InShaderCacheItem )
{
	FShader::Init( InShaderCacheItem );

	// Bind shader parameters
	wireframeColorParameter.Bind( InShaderCacheItem.parameterMap, TEXT( "wireframeColor" ) );
}

void FWireframePixelShader::SetConstantParameters( class FBaseDeviceContextRHI* InDeviceContextRHI, const class FVertexFactory* InVertexFactory, const class FMaterial* InMaterialResource ) const
{
	check( InMaterialResource );
	FVector4D		wireframeColor = FColor::black.ToNormalizedVector4D();
	InMaterialResource->GetVectorParameterValue( TEXT( "wireframeColor" ), wireframeColor );

	SetPixelShaderValue( InDeviceContextRHI, wireframeColorParameter, wireframeColor );
}

#if WITH_EDITOR
bool FWireframePixelShader::ShouldCache( EShaderPlatform InShaderPlatform )
{
	return GIsEditor || GIsCookEditorContent;
}
#endif // WITH_EDITOR