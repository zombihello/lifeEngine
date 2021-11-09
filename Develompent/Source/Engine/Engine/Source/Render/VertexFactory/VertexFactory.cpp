#include "Misc/EngineGlobals.h"
#include "Misc/Misc.h"
#include "RHI/BaseRHI.h"
#include "Render/Shaders/ShaderManager.h"
#include "Render/VertexFactory/VertexFactory.h"

FVertexFactoryMetaType::FVertexFactoryMetaType( const std::wstring& InFactoryName, const std::wstring& InShaderName, const std::wstring& InFileName )
	: factoryName( InFactoryName )
	, shaderName( InShaderName )
	, hash( appCalcHash( InFactoryName ) )

#if WITH_EDITOR
	, sourceFilename( InFileName )
#endif // WITH_EDITOR
{
	FContainerVertexFactoryMetaType::Get()->RegisterType( this );
}

FVertexFactory::~FVertexFactory()
{}

void FVertexFactory::ReleaseRHI()
{
	declaration.SafeRelease();
	shader.SafeRelease();
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

void FVertexFactory::SetShaderParameters( class FBaseDeviceContextRHI* InDeviceContextRHI )
{}

void FVertexFactory::InitDeclaration( const FVertexDeclarationElementList& InElements )
{
	declaration = GRHI->CreateVertexDeclaration( InElements );
}

void FVertexFactory::Init()
{
	FVertexFactoryMetaType*			vertexFactoryType = GetType();
	shader = GShaderManager->FindInstance( vertexFactoryType->GetShaderName(), vertexFactoryType->GetHash() );
	check( shader );

	BeginInitResource( this );
}