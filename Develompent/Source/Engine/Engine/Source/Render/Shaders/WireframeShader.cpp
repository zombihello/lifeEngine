#include "Render/Shaders/WireframeShader.h"
#include "Render/VertexFactory/VertexFactory.h"
#include "Render/Material.h"

IMPLEMENT_SHADER_TYPE(, CWireframeVertexShader, TEXT( "WireframeShaders.hlsl" ), TEXT( "MainVS" ), SF_Vertex, true );
IMPLEMENT_SHADER_TYPE(, CWireframePixelShader, TEXT( "WireframeShaders.hlsl" ), TEXT( "MainPS" ), SF_Pixel, true );

/*
==================
CWireframeVertexShader::CWireframeVertexShader
==================
*/
CWireframeVertexShader::CWireframeVertexShader()
	: vertexFactoryParameters( nullptr )
{}

/*
==================
CWireframeVertexShader::~CWireframeVertexShader
==================
*/
CWireframeVertexShader::~CWireframeVertexShader()
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
CWireframeVertexShader::ShouldCache
==================
*/
bool CWireframeVertexShader::ShouldCache( EShaderPlatform InShaderPlatform, class CVertexFactoryMetaType* InVFMetaType /* = nullptr */ )
{
	return g_IsEditor || g_IsCookEditorContent && ( !InVFMetaType || InVFMetaType->GetHash() != CLightVertexFactory::staticType.GetHash() );
}

/*
==================
CWireframeVertexShader::ShouldCache
==================
*/
bool CWireframePixelShader::ShouldCache( EShaderPlatform InShaderPlatform, class CVertexFactoryMetaType* InVFMetaType /* = nullptr */ )
{
	return g_IsEditor || g_IsCookEditorContent && ( !InVFMetaType || InVFMetaType->GetHash() != CLightVertexFactory::staticType.GetHash() );
}
#endif // WITH_EDITOR

/*
==================
CWireframeVertexShader::Init
==================
*/
void CWireframeVertexShader::Init( const CShaderCache::ShaderCacheItem& InShaderCacheItem )
{
	CShader::Init( InShaderCacheItem );

	// Bind shader parameters
	CVertexFactoryMetaType* vertexFactoryType = CVertexFactoryMetaType::ContainerVertexFactoryMetaType::Get()->FindRegisteredType( GetVertexFactoryHash() );
	Assert( vertexFactoryType );

	vertexFactoryParameters = vertexFactoryType->CreateShaderParameters( SF_Vertex );
	vertexFactoryParameters->Bind( InShaderCacheItem.parameterMap );
}

/*
==================
CWireframeVertexShader::SetConstantParameters
==================
*/
void CWireframeVertexShader::SetConstantParameters( class CBaseDeviceContextRHI* InDeviceContextRHI, const class CVertexFactory* InVertexFactory, const TSharedPtr<class CMaterial>& InMaterialResource ) const
{
	Assert( vertexFactoryParameters );
	vertexFactoryParameters->Set( InDeviceContextRHI, InVertexFactory );
}

/*
==================
CWireframeVertexShader::SetMesh
==================
*/
void CWireframeVertexShader::SetMesh( class CBaseDeviceContextRHI* InDeviceContextRHI, const struct MeshBatch& InMesh, const class CVertexFactory* InVertexFactory, const class CSceneView* InView, uint32 InNumInstances /* = 1 */, uint32 InStartInstanceID /* = 0 */ ) const
{
	Assert( vertexFactoryParameters );
	vertexFactoryParameters->SetMesh( InDeviceContextRHI, InMesh, InVertexFactory, InView, InNumInstances, InStartInstanceID );
}


/*
==================
CWireframePixelShader::Init
==================
*/
void CWireframePixelShader::Init( const CShaderCache::ShaderCacheItem& InShaderCacheItem )
{
	CShader::Init( InShaderCacheItem );

	// Bind shader parameters
	wireframeColorParameter.Bind( InShaderCacheItem.parameterMap, TEXT( "wireframeColor" ) );
}

/*
==================
CWireframePixelShader::SetConstantParameters
==================
*/
void CWireframePixelShader::SetConstantParameters( class CBaseDeviceContextRHI* InDeviceContextRHI, const class CVertexFactory* InVertexFactory, const TSharedPtr<class CMaterial>& InMaterialResource ) const
{
	Assert( InMaterialResource );
	Vector4D		wireframeColor = CColor::black.ToNormalizedVector4D();
	InMaterialResource->GetVectorParameterValue( TEXT( "wireframeColor" ), wireframeColor );

	SetPixelShaderValue( InDeviceContextRHI, wireframeColorParameter, wireframeColor );
}