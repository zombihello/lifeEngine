#include "LEBuild.h"
#include "Misc/EngineGlobals.h"
#include "Misc/Misc.h"
#include "RHI/BaseRHI.h"
#include "Render/RenderingThread.h"
#include "Render/Shaders/ShaderManager.h"
#include "Render/VertexFactory/VertexFactory.h"

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
	SContainerVertexFactoryMetaType::Get()->RegisterType( this );
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
		const SVertexStream&		stream = streams[ streamIndex ];
		Assert( stream.vertexBuffer );
		g_RHI->SetStreamSource( InDeviceContextRHI, streamIndex, stream.vertexBuffer, stream.stride, 0 );
	}
}

/*
==================
CVertexFactory::SetupInstancing
==================
*/
void CVertexFactory::SetupInstancing( class CBaseDeviceContextRHI* InDeviceContextRHI, const struct SMeshBatch& InMesh, const class CSceneView* InView, uint32 InNumInstances /* = 1 */, uint32 InStartInstanceID /* = 0 */ ) const
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
void CVertexFactory::ModifyCompilationEnvironment( EShaderPlatform InShaderPlatform, SShaderCompilerEnvironment& InEnvironment )
{}
#endif // WITH_EDITOR