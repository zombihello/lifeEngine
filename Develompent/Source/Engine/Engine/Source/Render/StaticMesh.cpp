#include "Containers/String.h"
#include "Logger/LoggerMacros.h"
#include "System/Archive.h"
#include "Render/StaticMesh.h"
#include "Render/SceneUtils.h"

FStaticMesh::FStaticMesh()
	: FAsset( AT_StaticMesh )
{}

FStaticMesh::~FStaticMesh()
{
	// Remove all drawing policy links from scenes
	for ( auto itElement = elementDrawingPolicyMap.begin(), itElementEnd = elementDrawingPolicyMap.end(); itElement != itElementEnd; ++itElement )
	{
		for ( uint32 index = 0, count = itElement->second->drawingPolicyLinks.size(); index < count; ++index )
		{
			itElement->first->staticMeshDrawList.RemoveItem( itElement->second->drawingPolicyLinks[ index ] );
		}
	}
}

void FStaticMesh::InitRHI()
{
	// Create vertex buffer
	uint32			numVerteces = ( uint32 )verteces.Num();
	if ( numVerteces > 0 )
	{
		vertexBufferRHI = GRHI->CreateVertexBuffer( FString::Format( TEXT( "%s" ), GetAssetName().c_str() ).c_str(), sizeof( FStaticMeshVertexType ) * numVerteces, ( byte* )verteces.GetData(), RUF_Static );

		// Initialize vertex factory
		vertexFactory = new FStaticMeshVertexFactory();
		vertexFactory->AddVertexStream( FVertexStream{ vertexBufferRHI, sizeof( FStaticMeshVertexType ) } );		// 0 stream slot
		vertexFactory->Init();
	}

	// Create index buffer
	uint32			numIndeces = ( uint32 )indeces.Num();
	if ( numIndeces > 0 )
	{
		indexBufferRHI = GRHI->CreateIndexBuffer( FString::Format( TEXT( "%s" ), GetAssetName().c_str() ).c_str(), sizeof( uint32 ), sizeof( uint32 ) * numIndeces, ( byte* )indeces.GetData(), RUF_Static );
	}

	if ( !GIsEditor && !GIsCommandlet )
	{
		verteces.RemoveAllElements();
		indeces.RemoveAllElements();
	}
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

	FAsset::Serialize( InArchive );

	if ( InArchive.Ver() < VER_CompressedZlib )
	{
		std::vector<FStaticMeshVertexType>		tmpVerteces;
		std::vector<uint32>						tmpIndeces;
		InArchive << tmpVerteces;
		InArchive << tmpIndeces;

		verteces = tmpVerteces;
		indeces = tmpIndeces;
		LE_LOG( LT_Warning, LC_Package, TEXT( "Deprecated package version, in future must be removed supports" ) );
	}
	else
	{
		InArchive << verteces;
		InArchive << indeces;
	}

	InArchive << surfaces;
	InArchive << materials;

	if ( InArchive.IsLoading() )
	{
		BeginUpdateResource( this );
	}
}

void FStaticMesh::SetData( const std::vector<FStaticMeshVertexType>& InVerteces, const std::vector<uint32>& InIndeces, const std::vector<FStaticMeshSurface>& InSurfaces, std::vector< TAssetHandle<FMaterial> >& InMaterials )
{
	// Copy new parameters of static mesh
	verteces		= InVerteces;
	indeces			= InIndeces;
	surfaces		= InSurfaces;
	materials		= InMaterials;

	// Mark dirty all drawing policy links
	for ( auto itElement = elementDrawingPolicyMap.begin(), itElementEnd = elementDrawingPolicyMap.end(); itElement != itElementEnd; ++itElement )
	{
		itElement->second->bDirty = true;
	}

	BeginUpdateResource( this );
}

void FStaticMesh::SetMaterial( uint32 InMaterialIndex, const TAssetHandle<FMaterial>& InNewMaterial )
{
	if ( InMaterialIndex > materials.size() )
	{
		return;
	}

	if ( materials[ InMaterialIndex ] != InNewMaterial )
	{
		MarkDirty();

		// Mark dirty all drawing policy links
		for ( auto itElement = elementDrawingPolicyMap.begin(), itElementEnd = elementDrawingPolicyMap.end(); itElement != itElementEnd; ++itElement )
		{
			itElement->second->bDirty = true;
		}
	}

	materials[ InMaterialIndex ] = InNewMaterial;
}

TSharedPtr<FStaticMesh::FElementDrawingPolicyLink> FStaticMesh::MakeCustomDrawingPolicyLink( FSceneDepthGroup& InSDG, std::vector<TAssetHandle<FMaterial>>* InOverrideMaterials /* = nullptr */ )
{
	// Allocate new element
	TSharedPtr<FElementDrawingPolicyLink>	element					= MakeSharedPtr<FElementDrawingPolicyLink>();
	uint32									numOverrideMaterials	= InOverrideMaterials ? InOverrideMaterials->size() : 0;

	// Generate mesh batch for surface and add to new scene draw policy link
	for ( uint32 indexSurface = 0, numSurfaces = ( uint32 )surfaces.size(); indexSurface < numSurfaces; ++indexSurface )
	{
		const FStaticMeshSurface&		surface				= surfaces[ indexSurface ];
		FDrawingPolicyLinkRef           tmpDrawPolicyLink	= new FDrawingPolicyLink( DEC_STATIC_MESH );
		TAssetHandle<FMaterial>			material			= materials[ surface.materialID ];

		// If current material is override - use custom material
		if ( indexSurface < numOverrideMaterials )
		{
			TAssetHandle<FMaterial>			overrideMaterial = InOverrideMaterials->at( surface.materialID );
			if ( overrideMaterial.IsValid() )
			{
				material = overrideMaterial;
			}
		}

		tmpDrawPolicyLink->drawingPolicy.Init( vertexFactory, material );

		// Generate mesh batch of surface
		FMeshBatch					meshBatch;
		meshBatch.baseVertexIndex	= surface.baseVertexIndex;
		meshBatch.firstIndex		= surface.firstIndex;
		meshBatch.numPrimitives		= surface.numPrimitives;
		meshBatch.indexBufferRHI	= indexBufferRHI;
		meshBatch.primitiveType		= PT_TriangleList;
		tmpDrawPolicyLink->meshBatchList.insert( meshBatch );

		// Add to new scene draw policy link
		FDrawingPolicyLinkRef		drawingPolicyLink = InSDG.staticMeshDrawList.AddItem( tmpDrawPolicyLink );
		check( drawingPolicyLink );
		element->drawingPolicyLinks.push_back( drawingPolicyLink );

		// Get link to mesh batch. If not founded insert new
		FMeshBatchList::iterator        itMeshBatchLink = drawingPolicyLink->meshBatchList.find( meshBatch );
		if ( itMeshBatchLink != drawingPolicyLink->meshBatchList.end() )
		{
			element->meshBatchLinks.push_back( &( *itMeshBatchLink ) );
		}
		else
		{
			const FMeshBatch*		meshBatchLink = &( *drawingPolicyLink->meshBatchList.insert( meshBatch ).first );
			element->meshBatchLinks.push_back( meshBatchLink );
		}
	}

	return element;
}

void FStaticMesh::UnlinkDrawList( FSceneDepthGroup& InSDG, TSharedPtr<FElementDrawingPolicyLink>& InDrawingPolicyLink )
{
	// If pointer is not valid, we exist
	if ( !InDrawingPolicyLink )
	{
		return;
	}

	// Find element in cache, if not found - exist
	auto	itElement = elementDrawingPolicyMap.find( &InSDG );
	if ( itElement == elementDrawingPolicyMap.end() )
	{
		return;
	}

	// If this element in used other components, we reset only this link
	if ( InDrawingPolicyLink.GetSharedReferenceCount() > 2 )
	{
		InDrawingPolicyLink.Reset();
		return;
	}

	// Else we remove drawing policy link from SDG
	for ( uint32 index = 0, count = itElement->second->drawingPolicyLinks.size(); index < count; ++index )
	{
		InSDG.staticMeshDrawList.RemoveItem( itElement->second->drawingPolicyLinks[ index ] );
	}

	InDrawingPolicyLink.Reset();
	elementDrawingPolicyMap.erase( itElement );
}