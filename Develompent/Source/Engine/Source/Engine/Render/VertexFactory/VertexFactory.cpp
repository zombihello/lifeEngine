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

#include "Core/LEBuild.h"
#include "Core/Misc/Misc.h"
#include "RHI/BaseRHI.h"
#include "Engine/Misc/EngineGlobals.h"
#include "Engine/Render/RenderingThread.h"
#include "Engine/Render/Shaders/ShaderManager.h"
#include "Engine/Render/VertexFactory/VertexFactory.h"

/*
==================
CVertexFactoryMetaType::CVertexFactoryMetaType
==================
*/
CVertexFactoryMetaType::CVertexFactoryMetaType( const std::wstring& InFactoryName, const std::wstring& InFileName, bool InSupportsInstancing, uint32 InInstanceStreamIndex, ConstructParametersType InConstructParameters
#if WITH_EDITOR
												, ShouldCacheFn_t InShouldCacheFunc, ModifyCompilationEnvironmentFn_t InModifyCompilationEnvironmentFunc
#endif // WITH_EDITOR
)
	: factoryName( InFactoryName )
	, hash( Sys_CalcHash( InFactoryName ) )
	, ConstructParameters( InConstructParameters )
#if USE_INSTANCING
	, bSupportsInstancing( InSupportsInstancing )
#else
	, bSupportsInstancing( false )
#endif // USE_INSTANCING
	, instanceStreamIndex( InInstanceStreamIndex )
#if WITH_EDITOR
	, sourceFilename( InFileName )
	, ShouldCacheFunc( InShouldCacheFunc )
	, ModifyCompilationEnvironmentFunc( InModifyCompilationEnvironmentFunc )
#endif // WITH_EDITOR
{
	ContainerVertexFactoryMetaType::Get()->RegisterType( this );
}


/*
==================
CVertexFactory::~CVertexFactory
==================
*/
CVertexFactory::~CVertexFactory()
{
	BeginInitResource( this );
}

/*
==================
CVertexFactory::ReleaseRHI
==================
*/
void CVertexFactory::ReleaseRHI()
{
	declaration.SafeRelease();
	streams.clear();
}

/*
==================
CVertexFactory::Set
==================
*/
void CVertexFactory::Set( class CBaseDeviceContextRHI* InDeviceContextRHI ) const
{
	Assert( IsInitialized() );
	for ( uint32 streamIndex = 0, streamNum = ( uint32 )streams.size(); streamIndex < streamNum; ++streamIndex )
	{
		const VertexStream&		stream = streams[ streamIndex ];
		Assert( stream.vertexBuffer );
		g_RHI->SetStreamSource( InDeviceContextRHI, streamIndex, stream.vertexBuffer, stream.stride, 0 );
	}
}

/*
==================
CVertexFactory::SetupInstancing
==================
*/
void CVertexFactory::SetupInstancing( class CBaseDeviceContextRHI* InDeviceContextRHI, const struct MeshBatch& InMesh, const class CSceneView* InView, uint32 InNumInstances /* = 1 */, uint32 InStartInstanceID /* = 0 */ ) const
{
	Sys_Errorf( TEXT( "CVertexFactory::SetupInstancing :: Not implemented" ) );
}

/*
==================
CVertexFactory::SetShaderParameters
==================
*/
void CVertexFactory::SetShaderParameters( class CBaseDeviceContextRHI* InDeviceContextRHI )
{}

/*
==================
CVertexFactory::InitDeclaration
==================
*/
void CVertexFactory::InitDeclaration( const VertexDeclarationElementList_t& InElements )
{
	declaration = g_RHI->CreateVertexDeclaration( InElements );
}

/*
==================
CVertexFactory::InitDeclaration
==================
*/
void CVertexFactory::InitDeclaration( const VertexDeclarationRHIParamRef_t InDeclaration )
{
	Assert( InDeclaration );
	declaration = InDeclaration;
}

/*
==================
CVertexFactory::Init
==================
*/
void CVertexFactory::Init()
{
	BeginInitResource( this );
}

/*
==================
CVertexFactory::GetTypeHash
==================
*/
uint64 CVertexFactory::GetTypeHash() const
{
	uint64		hash = Sys_MemFastHash( declaration, GetType()->GetHash() );
	for ( uint32 index = 0, count = streams.size(); index < count; ++index )
	{
		hash = Sys_MemFastHash( streams[index], hash );
	}

	return hash;
}

#if WITH_EDITOR
/*
==================
CVertexFactory::ShouldCache
==================
*/
bool CVertexFactory::ShouldCache( EShaderPlatform InShaderPlatform )
{
	return true;
}

/*
==================
CVertexFactory::ModifyCompilationEnvironment
==================
*/
void CVertexFactory::ModifyCompilationEnvironment( EShaderPlatform InShaderPlatform, ShaderCompilerEnvironment& InEnvironment )
{}
#endif // WITH_EDITOR