#include "Render/Shaders/DepthOnlyShader.h"
#include "Render/VertexFactory/VertexFactory.h"
#include "Render/Material.h"
#include "Render/Scene.h"

IMPLEMENT_SHADER_TYPE(, CDepthOnlyVertexShader, TEXT( "DepthOnlyVertexShader.hlsl" ), TEXT( "MainVS" ), SF_Vertex, true );
IMPLEMENT_SHADER_TYPE(, CDepthOnlyPixelShader, TEXT( "DepthOnlyPixelShader.hlsl" ), TEXT( "MainPS" ), SF_Pixel, true );

CDepthOnlyVertexShader::CDepthOnlyVertexShader()
	: vertexFactoryParameters( nullptr )
{}

CDepthOnlyVertexShader::~CDepthOnlyVertexShader()
{
	if ( vertexFactoryParameters )
	{
		delete vertexFactoryParameters;
	}
}

void CDepthOnlyVertexShader::Init( const CShaderCache::SShaderCacheItem& InShaderCacheItem )
{
	CShader::Init( InShaderCacheItem );

	// Bind shader parameters
	CVertexFactoryMetaType*		vertexFactoryType = CVertexFactoryMetaType::SContainerVertexFactoryMetaType::Get()->FindRegisteredType( GetVertexFactoryHash() );
	check( vertexFactoryType );

	vertexFactoryParameters = vertexFactoryType->CreateShaderParameters( SF_Vertex );
	vertexFactoryParameters->Bind( InShaderCacheItem.parameterMap );
}

void CDepthOnlyVertexShader::SetConstantParameters( class CBaseDeviceContextRHI* InDeviceContextRHI, const class CVertexFactory* InVertexFactory, const TSharedPtr<class CMaterial>& InMaterialResource ) const
{
	check( vertexFactoryParameters );
	vertexFactoryParameters->Set( InDeviceContextRHI, InVertexFactory );
}

void CDepthOnlyVertexShader::SetMesh( class CBaseDeviceContextRHI* InDeviceContextRHI, const struct SMeshBatch& InMesh, const class CVertexFactory* InVertexFactory, const class CSceneView* InView, uint32 InNumInstances /* = 1 */, uint32 InStartInstanceID /* = 0 */ ) const
{
	check( vertexFactoryParameters );
	vertexFactoryParameters->SetMesh( InDeviceContextRHI, InMesh, InVertexFactory, InView, InNumInstances, InStartInstanceID );
}