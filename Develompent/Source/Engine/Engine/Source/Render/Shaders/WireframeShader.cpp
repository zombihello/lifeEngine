#include "Render/Shaders/WireframeShader.h"
#include "Render/VertexFactory/VertexFactory.h"
#include "Render/Material.h"

IMPLEMENT_SHADER_TYPE(, CWireframeVertexShader, TEXT( "WireframeShaders.hlsl" ), TEXT( "MainVS" ), SF_Vertex, true );
IMPLEMENT_SHADER_TYPE(, CWireframePixelShader, TEXT( "WireframeShaders.hlsl" ), TEXT( "MainPS" ), SF_Pixel, true );

CWireframeVertexShader::CWireframeVertexShader()
	: vertexFactoryParameters( nullptr )
{}

CWireframeVertexShader::~CWireframeVertexShader()
{
	if ( vertexFactoryParameters )
	{
		delete vertexFactoryParameters;
	}
}

#if WITH_EDITOR
#include "Render/VertexFactory/LightVertexFactory.h"

bool CWireframeVertexShader::ShouldCache( EShaderPlatform InShaderPlatform, class CVertexFactoryMetaType* InVFMetaType /* = nullptr */ )
{
	return GIsEditor || GIsCookEditorContent && ( !InVFMetaType || InVFMetaType->GetHash() != CLightVertexFactory::staticType.GetHash() );
}

bool CWireframePixelShader::ShouldCache( EShaderPlatform InShaderPlatform, class CVertexFactoryMetaType* InVFMetaType /* = nullptr */ )
{
	return GIsEditor || GIsCookEditorContent && ( !InVFMetaType || InVFMetaType->GetHash() != CLightVertexFactory::staticType.GetHash() );
}
#endif // WITH_EDITOR

void CWireframeVertexShader::Init( const CShaderCache::SShaderCacheItem& InShaderCacheItem )
{
	CShader::Init( InShaderCacheItem );

	// Bind shader parameters
	CVertexFactoryMetaType* vertexFactoryType = CVertexFactoryMetaType::SContainerVertexFactoryMetaType::Get()->FindRegisteredType( GetVertexFactoryHash() );
	check( vertexFactoryType );

	vertexFactoryParameters = vertexFactoryType->CreateShaderParameters( SF_Vertex );
	vertexFactoryParameters->Bind( InShaderCacheItem.parameterMap );
}

void CWireframeVertexShader::SetConstantParameters( class CBaseDeviceContextRHI* InDeviceContextRHI, const class CVertexFactory* InVertexFactory, const TSharedPtr<class CMaterial>& InMaterialResource ) const
{
	check( vertexFactoryParameters );
	vertexFactoryParameters->Set( InDeviceContextRHI, InVertexFactory );
}

void CWireframeVertexShader::SetMesh( class CBaseDeviceContextRHI* InDeviceContextRHI, const struct SMeshBatch& InMesh, const class CVertexFactory* InVertexFactory, const class CSceneView* InView, uint32 InNumInstances /* = 1 */, uint32 InStartInstanceID /* = 0 */ ) const
{
	check( vertexFactoryParameters );
	vertexFactoryParameters->SetMesh( InDeviceContextRHI, InMesh, InVertexFactory, InView, InNumInstances, InStartInstanceID );
}

void CWireframePixelShader::Init( const CShaderCache::SShaderCacheItem& InShaderCacheItem )
{
	CShader::Init( InShaderCacheItem );

	// Bind shader parameters
	wireframeColorParameter.Bind( InShaderCacheItem.parameterMap, TEXT( "wireframeColor" ) );
}

void CWireframePixelShader::SetConstantParameters( class CBaseDeviceContextRHI* InDeviceContextRHI, const class CVertexFactory* InVertexFactory, const TSharedPtr<class CMaterial>& InMaterialResource ) const
{
	check( InMaterialResource );
	Vector4D		wireframeColor = CColor::black.ToNormalizedVector4D();
	InMaterialResource->GetVectorParameterValue( TEXT( "wireframeColor" ), wireframeColor );

	SetPixelShaderValue( InDeviceContextRHI, wireframeColorParameter, wireframeColor );
}