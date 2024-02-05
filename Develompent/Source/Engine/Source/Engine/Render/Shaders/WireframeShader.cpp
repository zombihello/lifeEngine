/**
 * ************************************************************
 *                  This file is part of:
 *                      LIFEENGINE
 *          https://github.com/zombihello/lifeEngine
 * ************************************************************
 * Copyright (C) 2024 Yehor Pohuliaka.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "Engine/Render/Shaders/WireframeShader.h"
#include "Engine/Render/VertexFactory/VertexFactory.h"
#include "Engine/Render/Material.h"

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
#include "Core/System/Application.h"
#include "Engine/Render/VertexFactory/LightVertexFactory.h"

/*
==================
CWireframeVertexShader::ShouldCache
==================
*/
bool CWireframeVertexShader::ShouldCache( EShaderPlatform InShaderPlatform, class CVertexFactoryMetaType* InVFMetaType /* = nullptr */ )
{
	return CApplication::Get().GetType() == AT_Editor || g_IsCookEditorContent && ( !InVFMetaType || InVFMetaType->GetHash() != CLightVertexFactory::staticType.GetHash() );
}

/*
==================
CWireframeVertexShader::ShouldCache
==================
*/
bool CWireframePixelShader::ShouldCache( EShaderPlatform InShaderPlatform, class CVertexFactoryMetaType* InVFMetaType /* = nullptr */ )
{
	return CApplication::Get().GetType() == AT_Editor || g_IsCookEditorContent && ( !InVFMetaType || InVFMetaType->GetHash() != CLightVertexFactory::staticType.GetHash() );
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