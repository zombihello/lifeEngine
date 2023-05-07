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
TGlobalResource<CLightConeMesh>				GLightConeMesh;

/*
==================
CLightSphereMesh::CLightSphereMesh
==================
*/
CLightSphereMesh::CLightSphereMesh()
	: numPrimitives( 0 )
	, vertexFactory( new CLightVertexFactory( LT_Point ) )
{}

/*
==================
CLightSphereMesh::InitRHI
==================
*/
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
		vertexBufferRHI = g_RHI->CreateVertexBuffer( TEXT( "LightSphere" ), sizeof( SLightVertexType ) * numVerteces, ( byte* )verteces.data(), RUF_Static );

		// Initialize vertex factory
		vertexFactory->AddVertexStream( SVertexStream{ vertexBufferRHI, sizeof( SLightVertexType ) } );		// 0 stream slot
		vertexFactory->Init();
	}

	// Create index buffer
	uint32			numIndeces = indeces.size();
	if ( numIndeces > 0 )
	{
		indexBufferRHI = g_RHI->CreateIndexBuffer( TEXT( "LightSphere" ), sizeof( uint32 ), sizeof( uint32 ) * numIndeces, ( byte* )indeces.data(), RUF_Static );
	}
}

/*
==================
CLightSphereMesh::ReleaseRHI
==================
*/
void CLightSphereMesh::ReleaseRHI()
{
	vertexBufferRHI.SafeRelease();
	indexBufferRHI.SafeRelease();
	vertexFactory->ReleaseResource();
}


/*
==================
CLightQuadMesh::CLightQuadMesh
==================
*/
CLightQuadMesh::CLightQuadMesh()
	: vertexFactory( new CLightVertexFactory( LT_Directional ) )
{}

/*
==================
CLightQuadMesh::InitRHI
==================
*/
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

	vertexBufferRHI = g_RHI->CreateVertexBuffer( TEXT( "LightQuad" ), sizeof( SLightVertexType ) * ARRAY_COUNT( verteces ), ( byte* )&verteces[0], RUF_Static );
	indexBufferRHI	= g_RHI->CreateIndexBuffer( TEXT( "LightQuad" ), sizeof( uint32 ), sizeof( uint32 ) * ARRAY_COUNT( indeces ), ( byte* )&indeces[0], RUF_Static );

	// Initialize vertex factory
	vertexFactory->AddVertexStream( SVertexStream{ vertexBufferRHI, sizeof( SLightVertexType ) } );		// 0 stream slot
	vertexFactory->Init();
}

/*
==================
CLightQuadMesh::ReleaseRHI
==================
*/
void CLightQuadMesh::ReleaseRHI()
{
	vertexBufferRHI.SafeRelease();
	indexBufferRHI.SafeRelease();
	vertexFactory->ReleaseResource();
}


/*
==================
CLightConeMesh::CLightConeMesh
==================
*/
CLightConeMesh::CLightConeMesh()
	: numPrimitives( 0 )
	, vertexFactory( new CLightVertexFactory( LT_Spot ) )
{}

/*
==================
CLightConeMesh::InitRHI
==================
*/
void CLightConeMesh::InitRHI()
{
	const uint32					resolution = 30.f;
	const float						radius = 1.f;
	const float						height = 1.f;
	std::vector<SLightVertexType>	verteces;
	std::vector<uint32>				indeces;

	{
		float	step = 2.f * PI * ( 1.f / resolution );
		float	angle = 0.f;

		// Add vertices subdividing a circle
		for ( uint32 index = 0; index < resolution; ++index )
		{
			verteces.push_back( SLightVertexType{ Vector4D( SMath::Cos( angle ) * radius, -height, SMath::Sin( angle ) * radius, 1.f ) } );
			verteces.push_back( SLightVertexType{ Vector4D( SMath::Cos( angle ) * 0.f, 0.f, SMath::Sin( angle ) * 0.f, 1.f ) } );
			angle += step;
		}

		// Add the bottom of the cone
		verteces.push_back( SLightVertexType{ Vector4D( 0.f, -height, 0.f, 1.f ) } );
	}

	// Generate indices
	{
		// Indices for the main body
		for ( uint32 index = 0; index < resolution * 2 - 2; index += 2 )
		{
			indeces.push_back( index + 2 );
			indeces.push_back( index );
			indeces.push_back( index + 1 );

			indeces.push_back( index + 2 );
			indeces.push_back( index + 1 );
			indeces.push_back( index + 3 );
		}

		indeces.push_back( 0 );
		indeces.push_back( resolution * 2 - 2 );
		indeces.push_back( resolution * 2 - 1 );

		indeces.push_back( 0 );
		indeces.push_back( resolution * 2 - 1 );
		indeces.push_back( 1 );

		// Indices for the bottom
		for ( uint32 index = 0; index < resolution * 2 - 2; index += 2 )
		{
			indeces.push_back( index );
			indeces.push_back( index + 2 );
			indeces.push_back( resolution * 2 );
		}

		indeces.push_back( resolution * 2 - 2 );
		indeces.push_back( 0 );
		indeces.push_back( resolution * 2 );
	}
	numPrimitives = indeces.size() / 3;

	// Create vertex and index buffer
	vertexBufferRHI = g_RHI->CreateVertexBuffer( TEXT( "LightCone" ), sizeof( SLightVertexType ) * verteces.size(), ( byte* )verteces.data(), RUF_Static );
	indexBufferRHI = g_RHI->CreateIndexBuffer( TEXT( "LightCone" ), sizeof( uint32 ), sizeof( uint32 ) * indeces.size(), ( byte* )indeces.data(), RUF_Static );

	// Initialize vertex factory
	vertexFactory->AddVertexStream( SVertexStream{ vertexBufferRHI, sizeof( SLightVertexType ) } );		// 0 stream slot
	vertexFactory->Init();
}

/*
==================
CLightConeMesh::ReleaseRHI
==================
*/
void CLightConeMesh::ReleaseRHI()
{
	vertexBufferRHI.SafeRelease();
	indexBufferRHI.SafeRelease();
	vertexFactory->ReleaseResource();
}