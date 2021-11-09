#include "System/Archive.h"
#include "Render/StaticMesh.h"

FStaticMesh::FStaticMesh()
	: FAsset( AT_StaticMesh )
{}

FStaticMesh::~FStaticMesh()
{}

void FStaticMesh::InitRHI()
{
	// Create vertex buffer
	{
		uint32			numVerteces = ( uint32 )verteces.size();
		vertexBufferRHI = GRHI->CreateVertexBuffer( TEXT( "VertexBuffer" ), sizeof( FStaticMeshVertexType ) * numVerteces, ( byte* )verteces.data(), RUF_Static );
		verteces.clear();
	}

	// Create index buffer
	{
		uint32			numIndeces = ( uint32 )indeces.size();
		indexBufferRHI = GRHI->CreateIndexBuffer( TEXT( "IndexBuffer" ), sizeof( uint32 ), sizeof( uint32 ) * numIndeces, ( byte* )indeces.data(), RUF_Static );
		indeces.clear();
	}

	// Initialize vertex factory
	vertexFactory = new FStaticMeshVertexFactory();
	vertexFactory->AddVertexStream( FVertexStream{ vertexBufferRHI, sizeof( FStaticMeshVertexType ) } );		// 0 stream slot
	vertexFactory->Init();
}

void FStaticMesh::ReleaseRHI()
{
	vertexBufferRHI.SafeRelease();
	indexBufferRHI.SafeRelease();
	vertexFactory.SafeRelease();
}

void FStaticMesh::Serialize( class FArchive& InArchive )
{
	if ( InArchive.Ver() < VER_StaticMesh )
	{
		return;
	}

	InArchive << verteces;
	InArchive << indeces;
	InArchive << surfaces;
	InArchive << materials;

	BeginUpdateResource( this );
}

void FStaticMesh::SetData( const std::vector<FStaticMeshVertexType>& InVerteces, const std::vector<uint32>& InIndeces, const std::vector<FStaticMeshSurface>& InSurfaces, std::vector<FMaterialRef>& InMaterials )
{
	// Copy new parameters of static mesh
	verteces		= InVerteces;
	indeces			= InIndeces;
	surfaces		= InSurfaces;
	materials		= InMaterials;

	BeginUpdateResource( this );
}

void FStaticMesh::SetMaterial( uint32 InMaterialIndex, FMaterialRef InNewMaterial )
{
	if ( InMaterialIndex > materials.size() )
	{
		return;
	}

	materials[ InMaterialIndex ] = InNewMaterial;
}