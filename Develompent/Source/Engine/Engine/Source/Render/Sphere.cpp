#include "Misc/EngineGlobals.h"
#include "Render/Sphere.h"
#include "Render/IcosphereMeshBuilder.h"
#include "RHI/BaseRHI.h"
#include "RHI/BaseBufferRHI.h"

// -------------
// GLOBALS
// -------------
TGlobalResource<CSphereMesh>				g_SphereMesh;

/*
==================
CSphereMesh::CSphereMesh
==================
*/
CSphereMesh::CSphereMesh()
	: numPrimitives( 0 )
	, vertexFactory( new CDynamicMeshVertexFactory() )
{}

/*
==================
CSphereMesh::InitRHI
==================
*/
void CSphereMesh::InitRHI()
{
	std::vector< SDynamicMeshVertexType >		verteces;
	std::vector< uint32 >						indeces;

	// Generate sphere from icosphere
	{
		CIcosphereMeshBuilder		icosphereMeshBuilder;
		icosphereMeshBuilder.SetRadius( 1.f );
		icosphereMeshBuilder.SetSubdivision( 3 );
		icosphereMeshBuilder.Build();

		verteces		= icosphereMeshBuilder.GetVerteces();
		indeces			= icosphereMeshBuilder.GetIndeces();
		numPrimitives	= icosphereMeshBuilder.GetNumPrimitives();
	}

	// Create vertex buffer
	uint32			numVerteces = verteces.size();
	if ( numVerteces > 0 )
	{
		vertexBufferRHI = g_RHI->CreateVertexBuffer( TEXT( "Sphere" ), sizeof( SDynamicMeshVertexType ) * numVerteces, ( byte* )verteces.data(), RUF_Static );

		// Initialize vertex factory
		vertexFactory->AddVertexStream( SVertexStream{ vertexBufferRHI, sizeof( SDynamicMeshVertexType ) } );		// 0 stream slot
		vertexFactory->Init();
	}

	// Create index buffer
	uint32			numIndeces = indeces.size();
	if ( numIndeces > 0 )
	{
		indexBufferRHI = g_RHI->CreateIndexBuffer( TEXT( "Sphere" ), sizeof( uint32 ), sizeof( uint32 ) * numIndeces, ( byte* )indeces.data(), RUF_Static );
	}
}

/*
==================
CSphereMesh::ReleaseRHI
==================
*/
void CSphereMesh::ReleaseRHI()
{
	vertexBufferRHI.SafeRelease();
	indexBufferRHI.SafeRelease();
	vertexFactory->ReleaseResource();
}