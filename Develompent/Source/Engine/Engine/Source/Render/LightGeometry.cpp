#include "Misc/EngineGlobals.h"
#include "Render/LightGeometry.h"
#include "Render/IcosphereMeshBuilder.h"
#include "RHI/BaseRHI.h"
#include "RHI/BaseBufferRHI.h"

// -------------
// GLOBALS
// -------------
TGlobalResource<CLightSphereMesh>			GLightSphereMesh;
TGlobalResource<CLightQuadMesh>				GLightQuadMesh;


CLightSphereMesh::CLightSphereMesh()
	: numPrimitives( 0 )
	, vertexFactory( new CLightVertexFactory( LT_Point ) )
{}

void CLightSphereMesh::InitRHI()
{
	std::vector<SLightVertexType>		verteces;
	std::vector<uint32>					indeces;

	// Generate sphere from icosphere
	{
		CIcosphereMeshBuilder		icosphereMeshBuilder;
		icosphereMeshBuilder.SetRadius( 1.f );
		icosphereMeshBuilder.SetSubdivision( 3 );
		icosphereMeshBuilder.Build();

		// Convert SDynamicMeshVertexType to SLightVertexType
		const std::vector<SDynamicMeshVertexType>& tempVerteces = icosphereMeshBuilder.GetVerteces();
		verteces.resize( tempVerteces.size() );
		for ( uint32 index = 0, count = tempVerteces.size(); index < count; ++index )
		{
			verteces[index].position = tempVerteces[index].position;
		}

		indeces = icosphereMeshBuilder.GetIndeces();
		numPrimitives = icosphereMeshBuilder.GetNumPrimitives();
	}

	// Create vertex buffer
	uint32			numVerteces = verteces.size();
	if ( numVerteces > 0 )
	{
		vertexBufferRHI = GRHI->CreateVertexBuffer( TEXT( "LightSphere" ), sizeof( SLightVertexType ) * numVerteces, ( byte* )verteces.data(), RUF_Static );

		// Initialize vertex factory
		vertexFactory->AddVertexStream( SVertexStream{ vertexBufferRHI, sizeof( SLightVertexType ) } );		// 0 stream slot
		vertexFactory->Init();
	}

	// Create index buffer
	uint32			numIndeces = indeces.size();
	if ( numIndeces > 0 )
	{
		indexBufferRHI = GRHI->CreateIndexBuffer( TEXT( "LightSphere" ), sizeof( uint32 ), sizeof( uint32 ) * numIndeces, ( byte* )indeces.data(), RUF_Static );
	}
}

void CLightSphereMesh::ReleaseRHI()
{
	vertexBufferRHI.SafeRelease();
	indexBufferRHI.SafeRelease();
	vertexFactory->ReleaseResource();
}


CLightQuadMesh::CLightQuadMesh()
	: vertexFactory( new CLightVertexFactory( LT_Directional ) )
{}

void CLightQuadMesh::InitRHI()
{
	float	x0 = -1.f, y0 = -1.f;
	float	x1 = 1.f, y1 = 1.f;

	SLightVertexType		verteces[] =
	{
		//			POSITION			
		{ Vector4D( x0, y0, 0.f, 1.f ) },	// 0
		{ Vector4D( x0, y1, 0.f, 1.f ) },	// 1
		{ Vector4D( x1, y1, 0.f, 1.f ) },	// 2
		{ Vector4D( x1, y0, 0.f, 1.f ) }	// 3
	};
	uint32					indeces[] = { 0, 1, 2, 0, 2, 3 };

	vertexBufferRHI = GRHI->CreateVertexBuffer( TEXT( "LightQuad" ), sizeof( SLightVertexType ) * ARRAY_COUNT( verteces ), ( byte* )&verteces[0], RUF_Static );
	indexBufferRHI	= GRHI->CreateIndexBuffer( TEXT( "LightQuad" ), sizeof( uint32 ), sizeof( uint32 ) * ARRAY_COUNT( indeces ), ( byte* )&indeces[0], RUF_Static );

	// Initialize vertex factory
	vertexFactory->AddVertexStream( SVertexStream{ vertexBufferRHI, sizeof( SLightVertexType ) } );		// 0 stream slot
	vertexFactory->Init();
}

void CLightQuadMesh::ReleaseRHI()
{
	vertexBufferRHI.SafeRelease();
	indexBufferRHI.SafeRelease();
	vertexFactory->ReleaseResource();
}