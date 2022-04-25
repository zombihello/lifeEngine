#include "LEBuild.h"
#include "Misc/EngineGlobals.h"
#include "Misc/Misc.h"
#include "RHI/BaseRHI.h"
#include "Render/Shaders/ShaderManager.h"
#include "Render/VertexFactory/VertexFactory.h"

FVertexFactoryMetaType::FVertexFactoryMetaType( const std::wstring& InFactoryName, const std::wstring& InFileName, bool InSupportsInstancing, uint32 InInstanceStreamIndex, ConstructParametersType InConstructParameters
#if WITH_EDITOR
												, FShouldCacheFunc InShouldCacheFunc, FModifyCompilationEnvironmentFunc InModifyCompilationEnvironmentFunc
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
	FContainerVertexFactoryMetaType::Get()->RegisterType( this );
}

FVertexFactory::~FVertexFactory()
{}

void FVertexFactory::ReleaseRHI()
{
	declaration.SafeRelease();
	streams.clear();
}

void FVertexFactory::Set( class FBaseDeviceContextRHI* InDeviceContextRHI ) const
{
	check( IsInitialized() );
	for ( uint32 streamIndex = 0, streamNum = ( uint32 )streams.size(); streamIndex < streamNum; ++streamIndex )
	{
		const FVertexStream&		stream = streams[ streamIndex ];
		check( stream.vertexBuffer );
		GRHI->SetStreamSource( InDeviceContextRHI, streamIndex, stream.vertexBuffer, stream.stride, 0 );
	}
}

void FVertexFactory::SetupInstancing( class FBaseDeviceContextRHI* InDeviceContextRHI, const struct FMeshBatch& InMesh, const class FSceneView* InView, uint32 InNumInstances /* = 1 */, uint32 InStartInstanceID /* = 0 */ ) const
{
	appErrorf( TEXT( "FVertexFactory::SetupInstancing :: Not implemented" ) );
}

void FVertexFactory::SetShaderParameters( class FBaseDeviceContextRHI* InDeviceContextRHI )
{}

void FVertexFactory::InitDeclaration( const FVertexDeclarationElementList& InElements )
{
	declaration = GRHI->CreateVertexDeclaration( InElements );
}

void FVertexFactory::InitDeclaration( const FVertexDeclarationRHIParamRef InDeclaration )
{
	check( InDeclaration );
	declaration = InDeclaration;
}

void FVertexFactory::Init()
{
	BeginInitResource( this );
}

#if WITH_EDITOR
bool FVertexFactory::ShouldCache( EShaderPlatform InShaderPlatform )
{
	return true;
}

void FVertexFactory::ModifyCompilationEnvironment( EShaderPlatform InShaderPlatform, FShaderCompilerEnvironment& InEnvironment )
{}
#endif // WITH_EDITOR