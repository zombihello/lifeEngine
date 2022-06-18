#include "LEBuild.h"
#include "Misc/EngineGlobals.h"
#include "Misc/Misc.h"
#include "RHI/BaseRHI.h"
#include "Render/RenderingThread.h"
#include "Render/Shaders/ShaderManager.h"
#include "Render/VertexFactory/VertexFactory.h"

CVertexFactoryMetaType::CVertexFactoryMetaType( const std::wstring& InFactoryName, const std::wstring& InFileName, bool InSupportsInstancing, uint32 InInstanceStreamIndex, ConstructParametersType InConstructParameters
#if WITH_EDITOR
												, ShouldCacheFn_t InShouldCacheFunc, ModifyCompilationEnvironmentFn_t InModifyCompilationEnvironmentFunc
#endif // WITH_EDITOR
)
	: factoryName( InFactoryName )
	, hash( appCalcHash( InFactoryName ) )
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

CVertexFactory::~CVertexFactory()
{
	BeginInitResource( this );
}

void CVertexFactory::ReleaseRHI()
{
	declaration.SafeRelease();
	streams.clear();
}

void CVertexFactory::Set( class CBaseDeviceContextRHI* InDeviceContextRHI ) const
{
	check( IsInitialized() );
	for ( uint32 streamIndex = 0, streamNum = ( uint32 )streams.size(); streamIndex < streamNum; ++streamIndex )
	{
		const SVertexStream&		stream = streams[ streamIndex ];
		check( stream.vertexBuffer );
		GRHI->SetStreamSource( InDeviceContextRHI, streamIndex, stream.vertexBuffer, stream.stride, 0 );
	}
}

void CVertexFactory::SetupInstancing( class CBaseDeviceContextRHI* InDeviceContextRHI, const struct SMeshBatch& InMesh, const class CSceneView* InView, uint32 InNumInstances /* = 1 */, uint32 InStartInstanceID /* = 0 */ ) const
{
	appErrorf( TEXT( "CVertexFactory::SetupInstancing :: Not implemented" ) );
}

void CVertexFactory::SetShaderParameters( class CBaseDeviceContextRHI* InDeviceContextRHI )
{}

void CVertexFactory::InitDeclaration( const VertexDeclarationElementList_t& InElements )
{
	declaration = GRHI->CreateVertexDeclaration( InElements );
}

void CVertexFactory::InitDeclaration( const VertexDeclarationRHIParamRef_t InDeclaration )
{
	check( InDeclaration );
	declaration = InDeclaration;
}

void CVertexFactory::Init()
{
	BeginInitResource( this );
}

#if WITH_EDITOR
bool CVertexFactory::ShouldCache( EShaderPlatform InShaderPlatform )
{
	return true;
}

void CVertexFactory::ModifyCompilationEnvironment( EShaderPlatform InShaderPlatform, SShaderCompilerEnvironment& InEnvironment )
{}
#endif // WITH_EDITOR