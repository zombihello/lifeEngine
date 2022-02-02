#include "Containers/String.h"
#include "Misc/EngineGlobals.h"
#include "Logger/LoggerMacros.h"
#include "Render/WorldGrid.h"
#include "RHI/BaseRHI.h"

FWorldGrid::FWorldGrid()
	: numVerteces( 0 )
{}

FWorldGrid::~FWorldGrid()
{}

void FWorldGrid::InitRHI()
{
	// Create vertex buffer
	uint32			numVerteces = ( uint32 )verteces.Num();
	if ( numVerteces > 0 )
	{
		vertexBufferRHI = GRHI->CreateVertexBuffer( TEXT( "WorldGrid" ), sizeof( FWorldGridVertexType ) * numVerteces, ( byte* )verteces.GetData(), RUF_Static );

		// Initialize vertex factory
		vertexFactory = new FWorldGridVertexFactory();
		vertexFactory->AddVertexStream( FVertexStream{ vertexBufferRHI, sizeof( FWorldGridVertexType ) } );		// 0 stream slot
		vertexFactory->Init();
	}

	if ( !GIsEditor && !GIsCommandlet )
	{
		verteces.RemoveAllElements();
	}
}

void FWorldGrid::ReleaseRHI()
{
	vertexBufferRHI.SafeRelease();
	vertexFactory.SafeRelease();
}

void FWorldGrid::Update( float InMinSizeZ, float InMaxSizeZ, float InStepSize )
{
	// Generate world grid mesh
	for ( float index = InMinSizeZ; index <= InMaxSizeZ; index += InStepSize )
	{
		verteces.AddElement( FWorldGridVertexType{ FVector4D( index, 0, InMaxSizeZ, 1.f ) } );
		verteces.AddElement( FWorldGridVertexType{ FVector4D( index, 0, InMinSizeZ, 1.f ) } );
		verteces.AddElement( FWorldGridVertexType{ FVector4D( InMaxSizeZ, 0, index, 1.f ) } );
		verteces.AddElement( FWorldGridVertexType{ FVector4D( InMinSizeZ, 0, index, 1.f ) } );
	}
	numVerteces = verteces.Num();

	// Send resource to update RHI on render thread
	BeginUpdateResource( this );
}